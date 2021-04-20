// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/parser/parse_error.hpp"

#include <cassert>
#include <cppast/visitor.hpp>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <boost/type_erasure/any_cast.hpp>

#include <cmark-gfm-extension_api.h>
#include <cmark-gfm.h>

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_function_template.hpp>
#include <cppast/cpp_file.hpp>

#include "cmark-extension/cmark_extension.hpp"
#include "command-extension/command_extension.hpp"
#include "command-extension/user_data.hpp"
#include "ignore-html-extension/ignore_html_extension.hpp"
#include "verbatim-extension/verbatim_extension.hpp"

#include "../../standardese/parser/commands/section_command.hpp"
#include "../../standardese/parser/commands/special_command.hpp"
#include "../../standardese/parser/commands/inline_command.hpp"

#include "../../standardese/model/markup/block_quote.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/markup/emphasis.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/hard_break.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/link.hpp"
#include "../../standardese/model/markup/list_item.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/model/section.hpp"
#include "../../standardese/model/markup/soft_break.hpp"
#include "../../standardese/model/markup/strong_emphasis.hpp"
#include "../../standardese/model/markup/text.hpp"
#include "../../standardese/model/markup/thematic_break.hpp"
#include "../../standardese/model/markup/image.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/document.hpp"

// TODO: Better error reporting. It's a bit silly to report errors with XML that nobody understands?

namespace standardese::parser
{

template <typename T, auto free>
using unique_cmark = std::unique_ptr<T, std::integral_constant<decltype(free), free>>;

using unique_node = unique_cmark<cmark_node, cmark_node_free>;
using unique_iter = unique_cmark<cmark_iter, cmark_iter_free>;


comment_parser::comment_parser(comment_parser_options options, const cpp_context& context) : options(std::move(options)), context(context) {}


model::document comment_parser::parse(const std::string& comment) {
    // Create fresh parser with extensions to detect standardese commands.
    using unique_parser = unique_cmark<cmark_parser, cmark_parser_free_with_extensions>;
    auto parser = unique_parser(cmark_parser_new(CMARK_OPT_SMART));

    verbatim_extension::verbatim_extension::create(parser.get());
    // TODO: Should we use this extension here? The underlying proble is that we use the comment parser to parse the original comments and our generated headings.
    ignore_html_extension::ignore_html_extension::create(parser.get());
    // TODO: Should we completely disable this extension?
    // command_extension::command_extension::create(parser.get(), options.command_extension_options);

    // Parse the comment into a tree of cmark nodes.
    cmark_parser_feed(parser.get(), comment.c_str(), comment.size());
    auto root = unique_node(cmark_parser_finish(parser.get()));

    model::document doc{""};
    visit_children(root.get(), [&](cmark_node* child) { doc.add_child(parse(child)); });

    return doc;
}


std::vector<model::entity> comment_parser::parse(const std::string& comment, const cppast::cpp_entity& entity_, entity_resolver entity_resolver)
{
    auto* entity = &entity_;

    // Create fresh parser with extensions to detect standardese commands.
    using unique_parser = unique_cmark<cmark_parser, cmark_parser_free_with_extensions>;
    auto parser = unique_parser(cmark_parser_new(CMARK_OPT_SMART));

    verbatim_extension::verbatim_extension::create(parser.get());
    ignore_html_extension::ignore_html_extension::create(parser.get());
    command_extension::command_extension::create(parser.get(), options.command_extension_options);

    // Parse the comment into a tree of cmark nodes.
    cmark_parser_feed(parser.get(), comment.c_str(), comment.size());
    auto root = unique_node(cmark_parser_finish(parser.get()));

    // Resolve (and remove) any \entity commands.
    auto resolved = resolve_entity(root.get(), *entity, entity_resolver);

    // Typically, `resolved` will be a C++ entity, but it could be a free
    // module description so we need to handle that case here.
    if (resolved.has_value(type_safe::variant_type<std::string>{})) {
        std::vector<model::entity> entities;

        model::module model(resolved.value(type_safe::variant_type<std::string>{}));
        visit_children(root.get(), [&](cmark_node* child) { parse(child, model); });
        entities.emplace_back(std::move(model));
        return entities;
    }

    entity = resolved.value(type_safe::variant_type<const cppast::cpp_entity*>{});

    // Process inline commands and remove their subtrees.
    auto result = extract_inlines(root.get(), *entity);

    // Consume what's left of the tree as the documentation for `resolved_entity` itself.
    parse(root.get(), *entity, result);

    return result;
}


type_safe::variant<const cppast::cpp_entity*, std::string> comment_parser::resolve_entity(cmark_node* root, const cppast::cpp_entity& entity_, entity_resolver entity_resolver) const
{
    auto* entity = &entity_;

    const auto special_command = command_extension::command_extension::node_type<commands::special_command>();

    // This comment is already implicitly bound if it is next to something in
    // the C++ code. Otherwise, i.e., if it is just in an isolated place in the
    // header file, it is unbound.
    bool bound = entity->kind() != cppast::cpp_entity_kind::file_t;

    visit(root, [&](cmark_node* node) -> cmark_node* {
        if (cmark_node_get_type(node) != special_command)
            return node;

        const auto& command = command_extension::user_data<commands::special_command>::get(node);

        if (command.command == commands::special_command::file) {
            if (bound)
                throw parse_error(node, "File command cannot be used here as this comment is already bound to the entity `{}`.", *entity);

            assert(entity->kind() == cppast::cpp_entity_kind::file_t && "an unbound comment must be implicitly bound to its file");
            bound = true;
            // Delete this command from the parsed comment.
            return nullptr;
        } else if (command.command == commands::special_command::entity) {
            auto [target] = command.arguments<1>();

            if (bound)
                throw parse_error(node, "Entity command cannot be used here as this comment is already bound to the entity `{}`.", *entity);

            const auto resolved = entity_resolver(target);
            if (!resolved.has_value())
                throw parse_error(node, "Failed to resolve entity `{}` specified in entity command.", target);

            entity = &resolved.value();

            bound = true;
            // Delete this command from the parsed comment.
            return nullptr;
        }

        return node;
    });

    if (bound)
        return entity;

    // We did not find any \entity or \file command in this comment.
    // If it's a free comment with a \module command, it's actually the documentation for a module.
    std::string module;

    visit(root, [&](cmark_node* node) -> cmark_node* {
        if (cmark_node_get_type(node) != special_command)
            return node;

        const auto& command = command_extension::user_data<commands::special_command>::get(node);

        if (command.command == commands::special_command::module) {
            auto [target] = command.arguments<1>();

            if (bound)
                throw parse_error(node, "Multiple module commands cannot be used in the same comment for `{}`.", *entity);

            bound = true;
            module = target;
            // Delete this command from the parsed comment.
            return nullptr;
        }

        return node;
    });

    if (bound)
        return module;

    // Finally, if there's nothing this could be attached to, we assume it's a comment for this header file.
    if (options.free_file_comments)
        return entity;

    throw parse_error(root, "Failed to determine entity for free file comment in `{}`. Enable \"free file comments\" if this comment describes the entire file or use an `entity` or `file` command.", *entity);
}


std::vector<model::entity> comment_parser::extract_inlines(cmark_node* root, const cppast::cpp_entity& entity) const
{
    const auto inline_command = command_extension::command_extension::node_type<commands::inline_command>();

    std::vector<model::entity> inlines;

    visit(root, [&](cmark_node* node) -> cmark_node* {
        if (cmark_node_get_type(node) != inline_command)
            return node;

        const auto& command = command_extension::user_data<commands::inline_command>::get(node);
        const auto [target] = command.arguments<1>();

        if (command.command == commands::inline_command::base) {
            parse(node, resolve_base(entity, target), inlines);
        } else if (command.command == commands::inline_command::param) {
            parse(node, resolve_param(entity, target), inlines);
        } else if (command.command == commands::inline_command::tparam) {
            parse(node, resolve_tparam(entity, target), inlines);
        } else {
            throw std::logic_error("not implemented: unknown inline command");
        }

        // Delete this node from the tree.
        return nullptr;
    });

    return inlines;
}

const cppast::cpp_entity& comment_parser::resolve_base(const cppast::cpp_entity& entity, const std::string& name) const
{
    throw std::logic_error("not implemented: comment_parser::resolve_base");
}

const cppast::cpp_entity& comment_parser::resolve_param(const cppast::cpp_entity& entity_, const std::string& name) const
{
    auto* entity = &entity_;

    switch(entity->kind())
    {
        case cppast::cpp_entity_kind::function_template_t:
            entity = &static_cast<const cppast::cpp_function_template&>(*entity).function();
            [[fallthrough]];
        case cppast::cpp_entity_kind::function_t:
            for (const auto& param : static_cast<const cppast::cpp_function&>(*entity).parameters())
                if (param.name() == name)
                    return param;
            throw parse_error("Could not resolve parameter `{}` in `{}`.", name, *entity);
    }

    throw std::logic_error("not implemented: comment_parser::resolve_param");
}

const cppast::cpp_entity& comment_parser::resolve_tparam(const cppast::cpp_entity& entity, const std::string& name) const
{
    if (cppast::is_template(entity.kind())) {
        for (const auto& param : static_cast<const cppast::cpp_template&>(entity).parameters())
            if (param.name() == name)
                return param;
        throw parse_error("Could not resolve template parameter `{}` in `{}`.", name, entity);
    }

    throw std::logic_error("not implemented: comment_parser::resolve_tparam");
}

void comment_parser::parse(cmark_node* root, const cppast::cpp_entity& entity, std::vector<model::entity>& entities) const
{
    assert((cmark_node_get_type(root) == CMARK_NODE_DOCUMENT || cmark_node_get_type(root) == command_extension::command_extension::node_type<commands::inline_command>()) && "root node of a comment must be <document> or an inline node");

    auto model = model::cpp_entity_documentation(entity, context);
    visit_children(root, [&](cmark_node* child) { parse(child, model); });
    entities.emplace_back(std::move(model));
}

template <typename T>
void comment_parser::parse(cmark_node* node, T& model) const
{
    const auto special_command = command_extension::command_extension::node_type<commands::special_command>();
    const auto section_command = command_extension::command_extension::node_type<commands::section_command>();
    
    if (cmark_node_get_type(node) == special_command)
        apply_command(node, model);
    else if (cmark_node_get_type(node) == section_command)
        model.add_child(parse_section(node));
    else
        throw std::logic_error("not implemented: unexpected top-level node in parsed comment: " + std::string(cmark_node_get_type_string(node)));
}

template <typename T>
void comment_parser::apply_command(cmark_node* node, T& model) const
{
    const auto& command = command_extension::user_data<commands::special_command>::get(node);

    switch (command.command)
    {
      case commands::special_command::synopsis:
        {
          auto [synopsis] = command.arguments<1>();
          if (model.synopsis)
              throw parse_error(node, "Found multiple synopsis commands for `{}` but only one is allowed.", model);
          model.synopsis = synopsis;
          return;
        }
      case commands::special_command::exclude:
        {
          auto [target] = command.arguments<1>();
          model::exclude_mode mode;
          if (target == "")
              mode = model::exclude_mode::exclude;
          else if (target == "return")
              mode = model::exclude_mode::exclude_return_type;
          else if (target == "target")
              mode = model::exclude_mode::exclude_target;
          else
              throw parse_error(node, "Found unsupported exclude mode `{}` for `{}`.", target, model);
          if (model.exclude_mode != model::exclude_mode::include)
              throw parse_error(node, "Cannot set exclude mode more than once for `{}`.", model);
          model.exclude_mode = mode;
          return;
        }
      case commands::special_command::unique_name:
        {
          auto [name] = command.arguments<1>();
          if (model.id != "")
              throw parse_error(node, "Cannot set unique name for `{}` to `{}` since it already has a unique name `{}`.", model, name, model.id);
          model.id = name;
          return;
        }
      case commands::special_command::output_name:
        {
          auto [name] = command.arguments<1>();
          if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
              if (model.output_name != "")
                  throw parse_error(node, "Cannot reset output name for `{}` to `{}` since it already has an output name `{}`.", model, name, model.output_name);
              model.output_name = name;
          } else {
            throw parse_error(node, "Output name command can only be used for C++ entity documentation not for `{}`.", model);
          }
          return;
        }
      case commands::special_command::group:
        {
          auto [name, heading] = command.arguments<2>();
          if (model.group)
              throw parse_error(node, "Group cannot be set to `{}` for `{}` since it is already in the group `{}`.", name, model, model.group.value());
          if (model.output_section)
              throw parse_error(node, "Group cannot be set to `{}` for `{}` since it has already an explicit output section `{}`.", name, model, model.output_section.value());
          if (name.empty())
              throw parse_error(node, "Group name cannot be empty for `{}`.", model);

          if (name.front() == '-') {
              // name starts with -, erase it, and don't consider it a section
              name.erase(name.begin());
          } else if (heading.empty()) {
              heading = name;
          }

          model.group = name;
          if (!heading.empty())
              model.output_section = heading;
          return;
        }
      case commands::special_command::output_section:
        {
          auto [heading] = command.arguments<1>();
          if (model.group)
              throw parse_error(node, "Output section cannot be set to `{}` for `{}` since it is already in the group `{}`.", heading, model, model.group.value());
          if (model.output_section)
              throw parse_error(node, "Output section cannot be set to `{}` for `{}` since it has already an explicit output section `{}`.", heading, model, model.output_section.value());

          model.output_section = heading;
          return;
        }
      case commands::special_command::module:
        {
          auto [module] = command.arguments<1>();
          if constexpr (std::is_same_v<T, model::module>) {
            throw std::logic_error(fmt::format("Module command can not appear in the module description for `{}`.", model.name));
          } else {
            if (model.module)
                throw parse_error(node, "Module cannot be set to `{}` for `{}` since currently each entity can only be in a single module and `{}` is already in the module `{}`.", module, model, model, model.module.value());

            model.module = module;
          }
          return;
        }
      case commands::special_command::entity:
        throw std::logic_error("This is not possible: Entity commands have been removed when determining the entity of a command and before applying commands.");
      case commands::special_command::file:
        throw std::logic_error("This is not possible: File commands have been removed when determining the entity of a command and before applying commands.");
      case commands::special_command::end:
        throw std::logic_error("This is not possible: End commands have been removed during the CommonMark parsing.");
      default:
        throw std::logic_error(fmt::format("not implemented: unexpected special command `{}`", command.command));
    }
}

model::entity comment_parser::parse_section(cmark_node* node) const
{
    const auto& command = command_extension::user_data<commands::section_command>::get(node);

    const auto parse_into = [&](cmark_node* parent, auto&& container) {
      visit_children(parent, [&](cmark_node* child) { container.add_child(parse(child)); });
      return container;
    };

    return parse_into(node, model::section(command.command));
}

template <typename T>
T comment_parser::parse(cmark_node* node) const
{
    const auto force_type = [&](auto&& entity) -> T {
      if constexpr (std::is_convertible_v<decltype(entity), T>)
        return entity;
      throw std::logic_error("not implemented: invalid CommonMark node type in this context: " + std::string(cmark_node_get_type_string(node)));
    };

    const auto parse_into = [&](cmark_node* parent, auto&& container) -> T {
      using entity = typename std::remove_reference_t<decltype(container)>::entity;
      visit_children(parent, [&](cmark_node* child) { container.add_child(parse<entity>(child)); });
      return force_type(container);
    };

    if (cmark_node_get_type(node) == verbatim_extension::verbatim_extension::node_type())
      return force_type(model::markup::text(cmark_node_get_string_content(node)));

    switch(cmark_node_get_type(node)) {
      case CMARK_NODE_PARAGRAPH:
        return parse_into(node, model::markup::paragraph());
      case CMARK_NODE_SOFTBREAK:
        assert(cmark_node_first_child(node) == nullptr && "softbreak is not supposed to have any child nodes");
        return force_type(model::markup::soft_break());
      case CMARK_NODE_TEXT:
        assert(cmark_node_first_child(node) == nullptr && "text is not supposed to have any child nodes");
        return force_type(model::markup::text(cmark_node_get_literal(node)));
      case CMARK_NODE_CODE:
        assert(cmark_node_first_child(node) == nullptr && "code is not supposed to have any child nodes");
        return force_type(model::markup::code(cmark_node_get_literal(node)));
      case CMARK_NODE_EMPH:
        return parse_into(node, model::markup::emphasis());
      case CMARK_NODE_STRONG:
        return parse_into(node, model::markup::strong_emphasis());
      case CMARK_NODE_LINK:
        {
          // TODO: Make much of this configurable.
          // TODO: Read the commonmark spec to make sure we're not doing anything stupid here.

          // TODO: It would be nice to do something like the following:
          //       * do a fuzzy lookup as the default
          //       * when there is standardese://* do a fuzzy lookup.
          //       * when there is standardese:// do an exact lookup
          //       * when there is standardese://kind/* do a fuzzy lookup with a fixed type.
          //       * when there is standardese://kind/ do a lookup with a fixed type.

          std::string target = std::string(cmark_node_get_url(node));
          std::string title = std::string(cmark_node_get_title(node));

          if (title == "" && target == "") {
            cmark_node* child = cmark_node_first_child(node);
            if (child != nullptr && cmark_node_last_child(node) == child && cmark_node_get_type(child) == CMARK_NODE_TEXT) {
                // For a link of the form `[text]()`, `text` is the target of this link with standardese syntax.
                target = cmark_node_get_literal(child);
                if (target.size() != 0 && (target[0] == '?' || target[0] == '*'))
                    cmark_node_set_literal(child, target.substr(1).c_str());
            }
            return parse_into(node, model::markup::link(target, title));
          } else if (target == "") {
            // For a link of the form `[text](<> "title")` the `title` that is
            // usually used for the tooltip is the target.
            target = title;
            title = "";
            return parse_into(node, model::markup::link(target, title));
          } else {
            // Otherwise, we assume that this is a standard MarkDown link
            // (though it might still be using a special schema such as
            // standardese://.)
            return parse_into(node, model::markup::link(model::link_target::uri_target(target), title));
          }
        }
      case CMARK_NODE_IMAGE:
        return parse_into(node, model::markup::image(cmark_node_get_url(node), cmark_node_get_title(node)));
      case CMARK_NODE_BLOCK_QUOTE:
        return parse_into(node, model::markup::block_quote());
      case CMARK_NODE_LIST:
        return parse_into(node, model::markup::list(cmark_node_get_list_type(node) == CMARK_ORDERED_LIST));
      case CMARK_NODE_ITEM:
        return parse_into(node, model::markup::list_item());
      case CMARK_NODE_CODE_BLOCK:
        return force_type(model::markup::code_block(cmark_node_get_fence_info(node), cmark_node_get_literal(node)));
      case CMARK_NODE_HEADING:
        return parse_into(node, model::markup::heading(cmark_node_get_heading_level(node)));
      case CMARK_NODE_THEMATIC_BREAK:
        return force_type(model::markup::thematic_break());
    }

    throw std::logic_error("not implemented: unexpected CommonMark node type: " + std::string(cmark_node_get_type_string(node)) + " at " + cmark_extension::cmark_extension::to_xml(node));
}

void comment_parser::cmark_parser_free_with_extensions(cmark_parser* parser)
{
    auto cur = cmark_parser_get_syntax_extensions(parser);
    while (cur)
    {
        cmark_syntax_extension_free(cmark_get_default_mem_allocator(),
                                    static_cast<cmark_syntax_extension*>(cur->data));
        cur = cur->next;
    }
    cmark_parser_free(parser);
}

void comment_parser::visit(cmark_node* root, std::function<cmark_node*(cmark_node*)> callback) const
{
    auto iter = unique_iter(cmark_iter_new(root));

    while (true)
    {
        switch(cmark_iter_get_event_type(iter.get()))
        {
            case CMARK_EVENT_DONE:
                return;
            case CMARK_EVENT_EXIT:
            case CMARK_EVENT_NONE:
                cmark_iter_next(iter.get());
                break;
            case CMARK_EVENT_ENTER:
                cmark_node* current = cmark_iter_get_node(iter.get());
                cmark_node* next = callback(cmark_iter_get_node(iter.get()));
                if (next == nullptr) {
                  // Delete the node the iterator is pointing to and advance the iterator manually.
                  cmark_iter_reset(iter.get(), current, CMARK_EVENT_EXIT);
                  cmark_iter_next(iter.get());
                  cmark_node_free(current);
                  continue;
                } else {
                  cmark_iter_reset(iter.get(), next, CMARK_EVENT_ENTER);
                  cmark_iter_next(iter.get());
                }
                break;
        }
    }
}

void comment_parser::add_uncommented_entities(model::unordered_entities& entities, const cppast::cpp_file& header) const {
    if (header.kind() != cppast::cpp_file::kind())
      throw std::invalid_argument("header entity must be a file");

    const auto ensure_entity = [&](const cppast::cpp_entity& entity) {
        if (entities.find_cpp_entity(entity) == entities.end()) {
          auto documentation = model::cpp_entity_documentation(entity, context);
          documentation.exclude_mode = model::exclude_mode::uncommented;
          entities.insert(std::move(documentation));
        }
    };

    cppast::visit(header, [&](const cppast::cpp_entity& e, auto info) {
        ensure_entity(e);
        if (cppast::is_template(e.kind())) {
            for (const auto& param : static_cast<const cppast::cpp_template&>(e).parameters())
                ensure_entity(param);
        }
        if (cppast::is_function(e.kind())) {
            for (const auto& param : static_cast<const cppast::cpp_function_base&>(e).parameters())
                ensure_entity(param);
        }
    });
}

void comment_parser::add_uncommented_modules(model::unordered_entities& entities) const {
    const auto ensure_module = [&](const std::string& name) {
      if (entities.find_module(name) == entities.end()) {
        auto module = model::module(name);
        module.exclude_mode = model::exclude_mode::uncommented;
        entities.insert(std::move(module));
      }
    };

    for (const auto& entity : entities) {
      model::visitor::visit([&](auto&& documentation) {
        using T = std::decay_t<decltype(documentation)>;
        if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
          if (documentation.module) ensure_module(documentation.module.value());
        }
      }, entity);
    }
}

void comment_parser::add_missing_sections(model::unordered_entities& entities) const {
    for (auto& entity : entities) {
      model::visitor::visit([&](auto&& documentation) {
        using T = std::decay_t<decltype(documentation)>;
        if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
          auto kind = documentation.entity().kind();

          const bool needs_requires = cppast::is_template(kind);
          const bool needs_parameters = cppast::is_function(kind) || (cppast::is_template(kind) && cppast::is_function(static_cast<const cppast::cpp_template&>(documentation.entity()).begin()->kind()));

          if (needs_requires)
            if (!documentation.section(parser::commands::section_command::requires))
              documentation.add_child(model::section(parser::commands::section_command::requires));

          if (needs_parameters)
            if (!documentation.section(parser::commands::section_command::parameters))
              documentation.add_child(model::section(parser::commands::section_command::parameters));
        }
      }, entity);
    }
}

void comment_parser::visit_children(cmark_node* parent, std::function<void(cmark_node*)> callback) const
{
    for (cmark_node* child = cmark_node_first_child(parent); child != nullptr; child = cmark_node_next(child))
        callback(child);
}

}

/*
namespace
{
class ast_root
{
public:
    explicit ast_root(cmark_node* root) : root_(root) {}

    ast_root(ast_root&& other) noexcept : root_(other.root_)
    {
        other.root_ = nullptr;
    }

    ~ast_root() noexcept
    {
        if (root_)
            cmark_node_free(root_);
    }

    ast_root& operator=(ast_root&& other) noexcept
    {
        ast_root tmp(std::move(other));
        std::swap(root_, tmp.root_);
        return *this;
    }

    /// \returns The node.
    cmark_node* get() const noexcept
    {
        return root_;
    }

private:
    cmark_node* root_;
};

ast_root read_ast(const parser& p, const std::string& comment)
{
    cmark_parser_feed(p.get(), comment.c_str(), comment.size());
    auto root = cmark_parser_finish(p.get());
    return ast_root(root);
}
*/

/*
template <class Builder>
void add_children(const config& c, Builder& b, bool has_matching_entity, cmark_node* parent);

[[noreturn]] void error(cmark_node* node, std::string msg)
{
    throw parse_error(unsigned(cmark_node_get_start_line(node)),
                      unsigned(cmark_node_get_start_column(node)), std::move(msg));
}

output::markup::block_quote parse_block_quote(const config& c, bool has_matching_entity,
                                                       cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_BLOCK_QUOTE);

    output::markup::block_quote builder{output::markup::block_id{}};
    add_children(c, builder, has_matching_entity, node);
    return builder;
}

output::markup::block_entity parse_list(const config& c, bool has_matching_entity,
                                                 cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_LIST);

    auto type = cmark_node_get_list_type(node);
    if (type == CMARK_BULLET_LIST)
    {
        output::markup::unordered_list builder{output::markup::block_id{}};
        add_children(c, builder, has_matching_entity, node);
        return builder;
    }
    else if (type == CMARK_ORDERED_LIST)
    {
        output::markup::ordered_list builder{output::markup::block_id{}};
        add_children(c, builder, has_matching_entity, node);
        return builder;
    }

    throw std::logic_error("not implemented: unsupported kind of list");
}

output::markup::list_item parse_item(const config& c, bool has_matching_entity,
                                              cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_ITEM);

    output::markup::list_item builder{};
    add_children(c, builder, has_matching_entity, node);
    return builder;
}

output::markup::code_block parse_code_block(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_CODE_BLOCK);
    return output::markup::code_block(output::markup::block_id{}, cmark_node_get_fence_info(node),
                                     cmark_node_get_literal(node));
}

output::markup::paragraph parse_paragraph(const config& c, bool has_matching_entity,
                                                   cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_PARAGRAPH);

    output::markup::paragraph builder;
    add_children(c, builder, has_matching_entity, node);
    return builder;
}

output::markup::block_entity parse_heading(const config& c, bool has_matching_entity,
                                                    cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_HEADING);

    auto level = cmark_node_get_heading_level(node);
    if (level == 1)
    {
        output::markup::heading builder{output::markup::block_id{}};
        add_children(c, builder, has_matching_entity, node);
        return builder;
    }
    else
    {
        out        return parse_into(node, model::list(cmark_node_get_list_type(node) == CMARK_ORDERED_LIST));
put::markup::subheading builder{output::markup::block_id{}};
        add_children(c, builder, has_matching_entity, node);
        return builder;
    }
}

output::markup::thematic_break parse_thematic_break(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_THEMATIC_BREAK);
    (void)node;
    return output::markup::thematic_break();
}

output::markup::text parse_text(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_TEXT);
    return output::markup::text(cmark_node_get_literal(node));
}

output::markup::soft_break parse_softbreak(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_SOFTBREAK);
    return output::markup::soft_break{};
}

output::markup::hard_break parse_linebreak(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_LINEBREAK);
    return output::markup::hard_break{};
}

output::markup::code parse_code(const config&, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_CODE);
    return output::markup::code{output::markup::text(cmark_node_get_literal(node))};
}

output::markup::verbatim parse_verbatim(const config&, bool, cmark_node* node)
{
    return output::markup::verbatim(cmark_node_get_string_content(node));
}

output::markup::emphasis parse_emph(const config& c, bool has_matching_entity,
                                             cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_EMPH);
    output::markup::emphasis builder;
    add_children(c, builder, has_matching_entity, node);
    return builder;
}

output::markup::strong_emphasis parse_strong(const config& c, bool has_matching_entity,
                                                      cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_STRONG);
    output::markup::strong_emphasis builder;
    add_children(c, builder, has_matching_entity, node);
    return builder;
}
*/

/*
output::markup::link parse_external_link(const config& c, bool has_matching_entity, cmark_node* node, const char* title, const char* url)
{
    throw std::logic_error("not implemented: parse external link");
    output::markup::external_link builder(title, output::markup::url(url));
    add_children(c, builder, has_matching_entity, node);
    return builder;
}
*/

/*
output::markup::entity parse_link(const config& c, bool has_matching_entity,
                                              cmark_node* node)
{
    assert(cmark_node_get_type(node) == CMARK_NODE_LINK);

    auto title = cmark_node_get_title(node);
    auto url   = cmark_node_get_url(node);
    if (*url == '\0' && *title == '\0')
    {
        // both empty, unique name is child iff
        // child is a single text node
        auto child = cmark_node_first_child(node);
        if (child == nullptr || child != cmark_node_last_child(node)
            || cmark_node_get_type(child) != CMARK_NODE_TEXT)
            // not an entity link
            return parse_external_link(c, has_matching_entity, node, title, url);
        else
        {
            auto unique_name = cmark_node_get_literal(child);
            auto is_relative = *unique_name == '?' || *unique_name == '*';
            return output::markup::documentation_link{unique_name, is_relative ? unique_name + 1 : unique_name};
        }
    }
    else if (*url == '\0')
    {
        // url is empty, unique name is title
        output::markup::documentation_link builder(title);
        add_children(c, builder, has_matching_entity, node);
        return builder.finish();
    }
    else if (std::strncmp(url, "standardese://", std::strlen("standardese://")) == 0)
    {
        // standardese:// URL
        auto unique_name = std::string(url + std::strlen("standardese://"));
        if (!unique_name.empty() && unique_name.back() == '/')
            unique_name.pop_back();

        output::markup::documentation_link::builder builder(title, unique_name);
        add_children(c, builder, has_matching_entity, node);
        return builder.finish();
    }
    else
    {
        // regular link
        return parse_external_link(c, has_matching_entity, node, title, url);
    }
}
*/

/*
output::markup::entity parse_key(const char* key)
{
    if (*key == '[')
    {
        // look for a closing ']', might have a link
        auto begin = ++key;
        while (*key && *key != ']')
            ++key;

        if (*key)
        {
            // found one
            auto unique_name = std::string(begin, std::size_t(key - begin));
            return output::markup::documentation_link{unique_name, unique_name};
        }
    }

    // normal text
    return output::markup::text(key);
}
*/

/*
output::markup::brief_section parse_brief(const config& c, bool has_matching_entity,
                                                   cmark_node* node)
{
    assert(cmark_node_get_type(cmark_node_first_child(node)) == CMARK_NODE_PARAGRAPH);

    output::markup::brief_section builder;
    add_children(c, builder, has_matching_entity, cmark_node_first_child(node));
    return builder;
}

output::markup::details_section parse_details(const config& c, bool has_matching_entity,
                                                       cmark_node* node)
{
    output::markup::details_section builder;
    add_children(c, builder, has_matching_entity, node);
    return builder;
}

output::markup::entity parse_section(const config& c, bool has_matching_entity,
                                                   cmark_node*& node)
{
    const auto& data = command_extension::user_data<section_type>::get(node);

    switch (data.command)
    {
    case section_type::brief:
        return parse_brief(c, has_matching_entity, node);

    case section_type::details:
        return parse_details(c, has_matching_entity, node);

    case section_type::requires:
    case section_type::effects:
    case section_type::synchronization:
    case section_type::postconditions:
    case section_type::returns:
    case section_type::throws:
    case section_type::complexity:
    case section_type::remarks:
    case section_type::error_conditions:
    case section_type::notes:
    case section_type::preconditions:
    case section_type::constraints:
    case section_type::diagnostics:
    case section_type::see:
    {
        output::markup::inline_section builder{data.command, c.inline_section_name(data.command)};

        auto first = true;
        for (auto child = cmark_node_first_child(node); child; child = cmark_node_next(child))
        {
            if (first)
                first = false;
            else
                builder.paragraph.add_child(output::markup::soft_break());

            for (auto& paragraph_child : parse_paragraph(c, has_matching_entity, child))
                builder.paragraph.add_child(paragraph_child);
        }

        return builder;
    }

    default:
        throw std::logic_error("not implemented: unsupported section type");
    }
}

struct comment_builder
{
    matching_entity entity;

    metadata data;

    type_safe::optional<output::markup::brief_section>            brief;
    std::vector<output::markup::entity> sections;
    std::vector<unmatched_doc_comment>                inlines;
};

exclude_mode parse_exclude_mode(cmark_node* node)
{
    const auto& data = command_extension::user_data<command_type>::get(node);

    const auto [target] = data.arguments<1>();

    if (target == "return")
        return exclude_mode::return_type;
    else if (target == "target")
        return exclude_mode::target;
    else if (target != "")
        error(node, "invalid argument for exclude");
    else
        return exclude_mode::entity;
}

member_group parse_group(cmark_node* node)
{
    const auto& data = command_extension::user_data<command_type>::get(node);
    auto [name, heading_] = data.arguments<2>();

    type_safe::optional<std::string> heading;
    if (heading_.size() != 0)
        heading = heading_;

    if (name.front() == '-')
    {
        // name starts with -, erase it, and don't consider it a section
        name.erase(name.begin());
        return member_group(std::move(name), std::move(heading), false);
    }
    else
        return member_group(std::move(name), std::move(heading), true);
}

// builder is nullptr when parsing an inline comment
void parse_command_impl(comment_builder* builder, bool has_matching_entity, metadata& data,
                        cmark_node* node)
{
    const auto& data_ = command_extension::user_data<command_type>::get(node);

    switch (data_.command)
    {
    case command_type::exclude:
        if (!data.set_exclude(parse_exclude_mode(node)))
            error(node, "multiple exclude commands for entity");
        break;
    case command_type::unique_name:
        {
            auto [name] = data_.arguments<1>();
            if (!data.set_unique_name(name))
              error(node, "multiple unique name commands for entity");
        }
        break;
    case command_type::output_name:
        {
            auto [name] = data_.arguments<1>();
            if (!data.set_output_name(name))
              error(node, "multiple output name commands for entity");
        }
        break;
    case command_type::synopsis:
        {
            auto [synopsis] = data_.arguments<1>();
            if (!data.set_synopsis(synopsis))
                error(node, "multiple synopsis commands for entity");
        }
        break;

    case command_type::group:
        if (data.output_section())
            error(node, "cannot have group and output section");
        else if (!data.set_group(parse_group(node)))
            error(node, "multiple group commands for entity");
        break;
    case command_type::module:
    {
        auto [module] = data_.arguments<1>();

        if (!has_matching_entity && builder && !builder->entity.has_value())
            // non-inline comment and not remote, treat as module comment
            builder->entity = comment::module(module);
        // otherwise treat as module specification
        else if (!data.set_module(module))
            error(node, "multiple module commands for entity");
        break;
    }
    case command_type::output_section:
    {
        const auto [heading] = data_.arguments<1>();
        if (data.group())
            error(node, "cannot have group and output section");
        else if (!data.set_output_section(heading))
            error(node, "multiple output section commands for entity");
        break;
    }
    case command_type::entity:
    {
        const auto [id] = data_.arguments<1>();

        if (!builder || has_matching_entity)
            error(node, "entity command not allowed for this node");
        else if (builder->entity.has_value())
            error(node, "multiple file/entity/module commands for entity");
        else
            builder->entity = remote_entity(id);
        break;
    }
    case command_type::file:
        if (!builder || has_matching_entity)
            error(node, "file command not allowed for this node");
        else if (builder->entity.has_value())
            error(node, "multiple file/entity/module commands for entity");
        else
            builder->entity = current_file{};
        break;
    case command_type::end:
        error(node, R"(unmatched \end command)");
        break;
    default:
        throw std::logic_error("not implemented: unsupported node type");
    }
}

void parse_command(metadata& data, bool has_matching_entity, cmark_node* node)
{
    parse_command_impl(nullptr, has_matching_entity, data, node);
}

void parse_command(comment_builder& builder, bool has_matching_entity, cmark_node* node)
{
    parse_command_impl(&builder, has_matching_entity, builder.data, node);
}

template <typename T>
void parse_command(const T&, bool, cmark_node*)
{
    assert(!static_cast<bool>("unexpected command"));
}

matching_entity parse_matching(cmark_node* node)
{
    const auto& data = command_extension::user_data<inline_type>::get(node);
    const auto [entity] = data.arguments<1>();

    switch (data.command)
    {
    case inline_type::param:
    case inline_type::tparam:
        return inline_param(entity);
    case inline_type::base:
        return inline_base(entity);
    default:
        throw std::logic_error("not implemented: unsupported inline type");
    }
}
*/

/*
void parse_inline(const config& c, comment_builder& builder, bool, cmark_node* node)
{
    assert(cmark_node_get_type(node) == command_extension::command_extension::node_type<inline_type>());

    metadata data;
    type_safe::optional<output::markup::brief_section> brief;
    std::vector<output::markup::entity> sections;
    for (auto child = cmark_node_first_child(node); child; child = cmark_node_next(child))
    {
        if (cmark_node_get_type(child) == command_extension::command_extension::node_type<command_type>())
            parse_command(data, true, child);
        else if (cmark_node_get_type(child) == command_extension::command_extension::node_type<section_type>())
        {
            auto section = parse_section(c, true, child);
            const auto& data_ = command_extension::user_data<section_type>::get(child);
            if (data_.command == section_type::brief)
            {
                assert(!brief);
                brief = std::unique_ptr<output::markup::brief_section>(
                    static_cast<output::markup::brief_section*>(section.release()));
            }
            else
                sections.push_back(std::move(section));
        }
        else
            assert(!static_cast<bool>("unexpected child"));
    }

    builder.inlines.push_back(
        unmatched_doc_comment(parse_matching(node),
                              doc_comment(std::move(data), std::move(brief), std::move(sections))));
}
*/

/*
template <typename T>
void parse_inline(const config&, const T&, bool, cmark_node*)
{
    assert(!static_cast<bool>("unexpected inline"));
}

void add_child(int, comment_builder& builder, output::markup::entity&& section)
{
    if (sectionkind() == output::markup::entity_kind::brief_section)
    {
        auto brief = std::unique_ptr<output::markup::brief_section>(
            static_cast<output::markup::brief_section*>(ptr.release()));
        if (builder.brief)
            error(nullptr, "multiple brief sections for comment");
        else
            builder.brief = std::move(brief);
    }typename T
    else
        builder.sections.push_back(std::move(ptr));
}
*/

/*
template <class Builder>
void add_children(const config& c, Builder& b, bool has_matching_entity, cmark_node* parent)
{
    for (auto cur = cmark_node_first_child(parent); cur; cur = cmark_node_next(cur))
    {
        using command_extension = command_extension::command_extension;
        using verbatim_extension = verbatim_extension::verbatim_extension;

        if (cmark_node_get_type(cur) == command_extension::node_type<section_type>())
            add_child(b, parse_section(c, has_matching_entity, cur));
        else if (cmark_node_get_type(cur) == command_extension::node_type<command_type>())
            parse_command(b, has_matching_entity, cur);
        else if (cmark_node_get_type(cur) == command_extension::node_type<inline_type>())
            parse_inline(c, b, has_matching_entity, cur);
        else if (cmark_node_get_type(cur) == verbatim_extension::node_type())
            add_child(b, parse_verbatim(c, has_matching_entity, cur));
        else
            switch (cmark_node_get_type(cur))
            {
            case CMARK_NODE_BLOCK_QUOTE:
                add_child(b, parse_block_quote(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_LIST:
                add_child(b, parse_list(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_ITEM:
                add_child(b, parse_item(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_CODE_BLOCK:
                add_child(b, parse_code_block(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_PARAGRAPH:
                add_child(b, parse_paragraph(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_HEADING:
                add_child(b, parse_heading(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_THEMATIC_BREAK:
                add_child(b, parse_thematic_break(c, has_matching_entity, cur));
                break;

            case CMARK_NODE_TEXT:
                add_child(b, parse_text(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_SOFTBREAK:
                add_child(b, parse_softbreak(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_LINEBREAK:
                add_child(b, parse_linebreak(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_CODE:
                add_child(b, parse_code(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_EMPH:
                add_child(b, parse_emph(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_STRONG:
                add_child(b, parse_strong(c, has_matching_entity, cur));
                break;
            case CMARK_NODE_LINK:
                add_child(b, parse_link(c, has_matching_entity, cur));
                break;

            case CMARK_NODE_HTML_BLOCK:
            case CMARK_NODE_HTML_INLINE:
            case CMARK_NODE_CUSTOM_BLOCK:
            case CMARK_NODE_CUSTOM_INLINE:
            case CMARK_NODE_IMAGE:
            case CMARK_NODE_FOOTNOTE_DEFINITION:
            case CMARK_NODE_FOOTNOTE_REFERENCE:
                error(cur, std::string("forbidden CommonMark node of type \"")
                               + cmark_node_get_type_string(cur) + "\"");
                break;

            case CMARK_NODE_NONE:
            case CMARK_NODE_DOCUMENT:
                assert(!static_cast<bool>("invalid node type"));
                break;
            }
    }
}
*/

/*
parse_result comment::parse(const parser& p, const std::string& comment, bool has_matching_entity)
{
    auto root = read_ast(p, comment);

    comment_builder builder;
    add_children(p.config(), builder, has_matching_entity, root.get());

    if (builder.brief || !builder.sections.empty() || !builder.data.is_empty())
        return parse_result{doc_comment(std::move(builder.data), std::move(builder.brief),
                                        std::move(builder.sections)),
                            std::move(builder.entity), std::move(builder.inlines)};
    else
        return parse_result{type_safe::nullopt, std::move(builder.entity),
                            std::move(builder.inlines)};
}
*/
