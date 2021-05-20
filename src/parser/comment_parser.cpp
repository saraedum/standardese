// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/parser/parse_error.hpp"

#include <cassert>
#include <cppast/forward.hpp>
#include <cppast/visitor.hpp>
#include <cstring>
#include <stdexcept>
#include <type_traits>

#include <cmark-gfm.h>
#include <cmark-gfm-extension_api.h>

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_class_template.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_function_template.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_preprocessor.hpp>
#include <cppast/cpp_friend.hpp>

#include "cmark-extension/cmark_extension.hpp"
#include "command-extension/command_extension.hpp"
#include "command-extension/user_data.hpp"
#include "ignore-html-extension/ignore_html_extension.hpp"
#include "verbatim-extension/verbatim_extension.hpp"

#include "../../standardese/parser/commands/section_command.hpp"
#include "../../standardese/parser/commands/special_command.hpp"
#include "../../standardese/parser/commands/inline_command.hpp"

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/symbols.hpp"

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
#include "../../standardese/logger.hpp"

// TODO: Better error reporting. It's a bit silly to report errors with XML
// that nobody understands? Generally, it would be nice to always report
// errors with some context, i.e., the source file (and location) responsible
// and the component of standardese that produced the message. The latter is
// maybe not necessary with enough verbosity. Anyway, we might want to have a
// threa-static context stack that's somehow built with RAII. Additionally,
// complex values should always be pushed to following lines, i.e., `Could not
// find {} in xml tree {}.` should only inline the variables if they are short
// and in particular single line. Otherwise, there should be [references] that
// are then resolved later. We could maybe even throw something like
// https://stackoverflow.com/questions/3899870/print-call-stack-in-c-or-c into
// the mix if enabled through a parameter.

namespace standardese::parser
{

comment_parser::comment_parser(comment_parser_options options, const cpp_context& context) : options(std::move(options)), context(context) {}

std::vector<model::entity> comment_parser::parse(const std::string& comment, const cppast::cpp_entity& entity_, entity_resolver entity_resolver)
{
    auto* entity = &entity_;

    // Create fresh parser with extensions to detect standardese commands.
    using unique_parser = unique_cmark<cmark_parser, cmark_parser_free_with_extensions>;
    auto parser = unique_parser(cmark_parser_new(CMARK_OPT_SMART));

    // TODO: Fix verbatim parser.
    // verbatim_extension::verbatim_extension::create(parser.get());
    ignore_html_extension::ignore_html_extension::create(parser.get());
    command_extension::command_extension::create(parser.get(), options.command_extension_options);

    // Parse the comment into a tree of cmark nodes.
    cmark_parser_feed(parser.get(), comment.c_str(), comment.size());
    using unique_node = unique_cmark<cmark_node, cmark_node_free>;
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
  inventory::cppast_inventory inventory{{&entity}, context};
  inventory::symbols symbols{inventory};

  // TODO: Limit lookup to only bases.
  const auto base = symbols.find(name, entity);

  if (!base.has_value())
    throw parse_error("Could not resolve base `{}` of `{}`.", name, entity);

  return *base.value().accept([&](auto&& target) -> type_safe::object_ref<const cppast::cpp_entity> {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
      if (target.target->kind() != cppast::cpp_base_class::kind())
        throw parse_error("Could not resolve `{}` to a base class of `{}`.", name, entity);
      return target.target;
    }

    throw std::logic_error("not implemented: unexpected symbol lookup result when looking for base class");
  });
}

const cppast::cpp_entity& comment_parser::resolve_param(const cppast::cpp_entity& entity, const std::string& name) const
{
  inventory::cppast_inventory inventory{{&entity}, context};
  inventory::symbols symbols{inventory};

  // TODO: Limit lookup to only parameters.
  const auto param = symbols.find(name, entity);

  if (!param.has_value())
    throw parse_error("Could not resolve parameter `{}` of `{}`.", name, entity);

  return *param.value().accept([&](auto&& target) -> type_safe::object_ref<const cppast::cpp_entity> {
    using T = std::decay_t<decltype(target)>;
    if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
      switch(target.target->kind()) {
        case cppast::cpp_entity_kind::function_parameter_t:
        case cppast::cpp_entity_kind::macro_parameter_t:
          return target.target;
        default:
          throw parse_error("Could not resolve `{}` to a parameter of `{}`.", name, entity);
      }
    }

    throw std::logic_error("not implemented: unexpected symbol lookup result when looking for parameter");
  });
}

const cppast::cpp_entity& comment_parser::resolve_tparam(const cppast::cpp_entity& entity, const std::string& name) const
{
  // TODO: Use symbols
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
          // TODO: Why should a group use an output section? Should this not rather be a "heading" for the group?
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
      visit_children(parent, [&](cmark_node* child) {
        container.add_child(parse(child));
      });
      return container;
    };

    return parse_into(node, model::section(command.command));
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
    const std::function<void(const cppast::cpp_entity&, cppast::visitor_info)> visitor = [&](const auto& e, auto info) {
        ensure_entity(e);

        if (cppast::is_template(e.kind())) {
            for (const auto& param : static_cast<const cppast::cpp_template&>(e).parameters())
                ensure_entity(param);
        }
        if (cppast::is_function(e.kind())) {
            for (const auto& param : static_cast<const cppast::cpp_function_base&>(e).parameters())
                ensure_entity(param);
        }
        if (e.kind() == cppast::cpp_macro_definition::kind()) {
            for (const auto& param : static_cast<const cppast::cpp_macro_definition&>(e).parameters())
                ensure_entity(param);
        }
        if (e.kind() == cppast::cpp_class::kind()) {
            for (const auto& base : static_cast<const cppast::cpp_class&>(e).bases())
                ensure_entity(base);
        }
        if (e.kind() == cppast::cpp_friend::kind()) {
            auto frend = static_cast<const cppast::cpp_friend&>(e).entity();
            if (frend.has_value())
              cppast::visit(frend.value(), visitor);
        }
    };

    cppast::visit(header, visitor);
}

void comment_parser::add_uncommented_modules(model::unordered_entities& entities) const {
    const auto ensure_module = [&](const std::string& name) {
      if (entities.find_module(name) == entities.end()) {
        auto module = model::module(name);
        module.exclude_mode = model::exclude_mode::uncommented;
        entities.insert(std::move(module));
 #include <cmark-gfm-extension_api.h>
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

model::entity comment_parser::parse(cmark_node* node) const {
  if (cmark_node_get_type(node) == verbatim_extension::verbatim_extension::node_type())
    return model::markup::text(cmark_node_get_string_content(node));
  return markdown_parser::parse(node);
}

}
