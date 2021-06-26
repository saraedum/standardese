// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/logger.hpp"
#include "inja_formatter.impl.hpp"

// TODO: Move all regex instantiations to a shared place. Calling the
// non-default constructor is extremely expensive for compile time.

// TODO: Drop external/ from everything but test/. Instead explain how to grab
//       dependencies and set compiler flags.

namespace standardese::formatter {

namespace {

bool check_arg_count(const std::string& name, const std::vector<const nlohmann::json*>& args, int min, int max = -1) {
  if (max == -1)
    max = min;

  if (args.size() < min) {
    logger::error(fmt::format("Inja callback `{}` requires at least {} arguments but only {} found.", name, min, args.size()));
    return false;
  }
  if (args.size() > max) {
    logger::error(fmt::format("Inja callback `{}` expects at most {} arguments. Ignoring trailing argument `{}`.", name, max, nlohmann::to_string(*args[max])));
  }

  return true;
}

}

inja_formatter::inja_formatter_options::inja_formatter_options() {}

inja_formatter::inja_formatter(struct inja_formatter_options options, parser::cpp_context cpp_context) : self(std::make_unique<impl>(std::move(options), std::move(cpp_context))) {
  add_callback("name", [&]() {
    return name_callback(self->data);
  });
  add_callback("name", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("name", args, 1))
      return std::string{};
    return name_callback(*args[0]);
  });
  add_callback("md", [&]() {
    return md_callback(self->data);
  });
  add_callback("md", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("md", args, 1))
      return std::string{};
    return md_callback(*args[0]);
  });
  add_callback("text", [&]() {
    return text_callback(self->data);
  });
  add_callback("text", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("text", args, 1))
      return std::string{};
    return text_callback(*args[0]);
  });
  add_callback("path", [&]() {
    return path_callback(self->data);
  });
  add_callback("path", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("path", args, 1))
      return std::string{};
    return path_callback(*args[0]);
  });
  add_callback("filename", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("filename", args, 1))
      return std::string{};
    return filename_callback(*args[0]);
  });
  add_callback("sanitize_basename", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("sanitize_basename", args, 1))
      return std::string{};
    return sanitize_basename_callback(*args[0]);
  });
  add_callback("code_escape", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("code_escape", args, 1))
      return std::string{};
    return code_escape_callback(*args[0]);
  });
  add_callback("md_escape", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("md_escape", args, 1))
      return std::string{};
    return md_escape_callback(*args[0]);
  });
  add_callback("format", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("format", args, 1, 2))
      return std::string{};
    if (args.size() == 2)
      return format_callback(*args[0], *args[1]);
    else
      return format_callback(*args[0]);
  });
  add_callback("list", [&](const std::vector<const nlohmann::json*>& args) {
    nlohmann::json ret = nlohmann::json::array();
    for (const auto& arg : args)
      ret.push_back(*arg);
    return ret;
  });
  add_callback("reject", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("reject", args, 2))
      return nlohmann::json::array();
    return reject_callback(*args[0], *args[1]);
  });
  add_callback("join", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("join", args, 2))
      return std::string{};
    return join_callback(*args[0], *args[1]);
  });
  add_callback("declaration_specifiers", [&]() {
    return declaration_specifiers_callback(data());
  });
  add_callback("declaration_specifiers", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("declaration_specifiers", args, 1))
      return nlohmann::json::array();
    return declaration_specifiers_callback(*args[0]);
  });
  add_callback("target", [&]() {
    return target_callback(data());
  });
  add_callback("target", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("target", args, 1))
      return std::string{};
    return target_callback(*args[0]);
  });
  add_callback("parameters", [&]() {
    return parameters_callback(data());
  });
  add_callback("parameters", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("parameters", args, 1))
      return nlohmann::json::array();
    return parameters_callback(*args[0]);
  });
  add_callback("const_qualification", [&]() {
    return const_qualification_callback(data());
  });
  add_callback("const_qualification", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("const_qualification", args, 1))
      return std::string{};
    return const_qualification_callback(*args[0]);
  });
  add_callback("volatile_qualification", [&]() {
    return volatile_qualification_callback(data());
  });
  add_callback("volatile_qualification", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("volatile_qualification", args, 1))
      return std::string{};
    return volatile_qualification_callback(*args[0]);
  });
  add_callback("ref_qualification", [&]() {
    return ref_qualification_callback(data());
  });
  add_callback("ref_qualification", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("ref_qualification", args, 1))
      return std::string{};
    return ref_qualification_callback(*args[0]);
  });
  add_callback("cppast_kind", [&]() {
    return cppast_kind_callback(data());
  });
  add_callback("cppast_kind", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("cppast_kind", args, 1))
      return std::string{};
    return cppast_kind_callback(*args[0]);
  });
  add_callback("kind", [&]() {
    return kind_callback(data());
  });
  add_callback("kind", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("kind", args, 1))
      return std::string{};
    return kind_callback(*args[0]);
  });
  add_callback("synopsis", [&]() {
    return synopsis_callback(data());
  });
  add_callback("synopsis", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("synopsis", args, 1))
      return to_json(model::markup::text{""});
    return synopsis_callback(*args[0]);
  });
  add_callback("option", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("option", args, 1))
      return std::string{};
    return option_callback(*args[0]);
  });
  add_callback("return_type", [&]() {
    return return_type_callback(data());
  });
  add_callback("return_type", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("return_type", args, 1))
      return nlohmann::json{};
    return return_type_callback(*args[0]);
  });
  add_callback("variable_type", [&]() {
    return variable_type_callback(data());
  });
  add_callback("variable_type", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("variable_type", args, 1))
      return nlohmann::json{};
    return variable_type_callback(*args[0]);
  });
  add_callback("type", [&]() {
    return type_callback(data());
  });
  add_callback("type", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("type", args, 1))
      return nlohmann::json{};
    return type_callback(*args[0]);
  });
  add_callback("arguments", [&]() {
    return arguments_callback(data());
  });
  add_callback("arguments", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("arguments", args, 1))
      return nlohmann::json{};
    return arguments_callback(*args[0]);
  });
  add_callback("code", [&]() {
    return code_callback(data());
  });
  add_callback("code", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("code", args, 1, 2))
      return nlohmann::json{};
    if (args.size() == 1)
      return code_callback(*args[0]);
    return code_callback(*args[0], *args[1]);
  });
  add_callback("entity", [&]() {
    return entity_callback(data());
  });
  add_callback("entity", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("entity", args, 1))
      return nlohmann::json{};
    return entity_callback(*args[0]);
  });
  add_callback("replace", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("replace", args, 3))
      return std::string{};
    return replace_callback(*args[0], *args[1], *args[2]);
  });
}

inja_formatter::inja_formatter(struct inja_formatter_options options, parser::cpp_context cpp_context, const model::mixin::documentation& context) : inja_formatter(std::move(options), std::move(cpp_context)) {
  self->context = type_safe::ref(context);
}

inja_formatter::impl::impl(inja_formatter_options options, parser::cpp_context cpp_context) : options(std::move(options)), cpp_context(std::move(cpp_context)) {}

inja_formatter::~inja_formatter() {}

nlohmann::json& inja_formatter::data() {
  return self->data;
}

const nlohmann::json& inja_formatter::data() const {
  return self->data;
}

}

/*
namespace standardese::formatter {

// TODO: All functions should type check and argument count check.


inja_formatter::inja_formatter(struct inja_formatter_options options) : options(options), self(std::make_unique<impl>()) {
  self->env.add_callback("filename", 1, [](inja::Arguments& args) {
    return boost::filesystem::path(args.at(0)->get<std::string>()).filename().string();
  });
  self->env.add_callback("code_escape", 1, [](inja::Arguments& args) {
    // TODO: Implement me.
    return args.at(0)->get<std::string>();
  });
  // });
  // TODO: Do we need these?
  // self->env.add_callback("code", 1, [this](inja::Arguments& args) {
  //   return self->render(self->transform([](std::string&& text) {
  //       return model::markup::code(text);
  //     }, [](model::mixin::container<>&& inlines) {
  //       model::markup::code code;
  //       for (auto&& entity : inlines)
  //         code.emplace_child(std::move(entity));

  //       return code;
  //     }, [](model::document&& blocks) {
  //       // TODO: Add optional parameter for language?
  //       model::markup::code_block code("c++");
  //       for (auto&& block : blocks)
  //         code.emplace_child(std::move(block));

  //       return code;
  //     }, self->parse(args.at(0)->get<std::string>())));
  // });
  self->env.add_callback("space", 3, [this](inja::Arguments& args) {
    std::string value = args.at(1)->get<std::string>();
    if (!value.empty())
      value = args.at(0)->get<std::string>() + value + args.at(2)->get<std::string>();
    return value;
  });
  self->env.add_callback("join", 2, [this](inja::Arguments& args) {
    std::string separator = args.at(0)->get<std::string>();
    if (!args.at(1)->is_array()) {
      logger::error(fmt::format("Ignoring argument {} to `join()` in inja template since it is not an array.", nlohmann::to_string(*args.at(1))));
      return std::string();
    }

    std::stringstream joined;
    auto items = args.at(1)->items();
    for (auto it = items.begin(); it != items.end(); ++it) {
      if (it != items.begin())
        joined << separator;
      joined << it.value().get<std::string>();
    }

    return joined.str();
  });
  self->env.add_callback("format", [this](inja::Arguments& args) {
    if (args.size() == 0) {
      logger::error("Inja template callback `format()` requires at least one argument.");
      return std::string{};
    }
    if (args.size() > 2) {
      logger::warn(fmt::format("Ignoring extra argument {} to template callback `format()`. Expects at most two arguments.", nlohmann::to_string(*args.at(2))));
    }
    if (!args.at(0)->is_string()) {
      logger::error(fmt::format("Template callback `format()` expects a string argument but found {}.", nlohmann::to_string(*args.at(0))));
      return std::string{};
    }

    auto formatter = *this;
    if (args.size() >= 2)
      formatter.data() = *args.at(1);

    return formatter.format(args.at(0)->get<std::string>());
  });
  self->env.add_callback("list", [](inja::Arguments& args) {
    nlohmann::json ret = nlohmann::json::array();
    for (const auto& arg : args)
      ret.push_back(*arg);
    return ret;
  });
  self->env.add_callback("reject", [](inja::Arguments& args) {
    const std::string predicate = args.at(0)->get<std::string>();

    static const auto empty = [](const nlohmann::json& arg) -> bool {
      if (arg.is_string())
        return arg.get<std::string>().empty();
      logger::error(fmt::format("Treating argument {} as empty since predicate empty not applicable to this type.", nlohmann::to_string(arg)));
      return true;
    };

    nlohmann::json filtered = nlohmann::json::array();
    for (const auto& arg : *args.at(1)) {
      if (predicate == "empty") {
        if (!empty(arg))
          filtered.push_back(arg);
      } else {
        logger::error(fmt::format(R"(Unknown predicate "{}" in reject invocation. Rejecting argument {}.)", predicate, nlohmann::to_string(arg)));
      }
    }

    return filtered;
  });
  self->env.add_callback("md", 1, [](inja::Arguments& args) {
    if (args.at(0)->is_null()) {
      return std::string{};
    } else if (args.at(0)->is_object()) {
      if (args.at(0)->contains("md")) {
        return args.at(0)->operator[]("md").get<std::string>();
      } else {
        logger::error(fmt::format("Cannot render {} as MarkDown as argument to template callback `md()`. Expected an object with a key 'md' instead.", nlohmann::to_string(*args.at(0))));
      }
    } else {
      logger::error(fmt::format("Cannot render {} as MarkDown as argument of invocation of template callback `md()`. Expected an object instead.", nlohmann::to_string(*args.at(0))));
    }
    return std::string{};
  });
}

const std::any& inja_formatter::impl::at(const std::string& id) const {
  auto search = embedded.find(id);
  if (search == embedded.end()) {
    // TODO: We should distinguish implementation error and user error when logging.
    logger::error(fmt::format("Unknown inja id `{}` found while parsing text node.", id));
    static std::any nothing;
    return nothing;
  }

  return search->second;
}

template <typename T>
type_safe::optional_ref<const T> inja_formatter::impl::at(const std::string& id) const {
  auto& search = at(id);

  if (!search.has_value())
    return type_safe::nullopt;

  if (search.type() != typeid(T)) {
    logger::error(fmt::format("Unsupported inja id `{}`. Found a {} but expected a {}.", id, search.type().name(), boost::typeindex::type_id<T>().pretty_name()));
    return type_safe::nullopt;
  }

  return type_safe::ref(std::any_cast<const T&>(search));
}

std::string inja_formatter::impl::embed(model::entity&& entity) {
  model::visitor::visit([&](auto& entity, auto&& recurse) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_same_v<T, model::markup::link>) {
      model::markup::link& link = entity;
      link.target.accept([&](auto&& target) {
        using T = std::decay_t<decltype(target)>;

        if constexpr (std::is_same_v<T, model::link_target::cppast_target>) {
          link.target = model::link_target::uri_target(embed(std::move(link.target)));
          
        }
      });
    }

    recurse();
  }, entity);

  bool is_inline = false;

  if (entity.is<model::document>()) {
    const auto& document = entity.as<model::document>();
    auto block = document.begin();
    if (block != document.end()) {
      if (++document.begin() == document.end() && block->is<model::markup::paragraph>()) {
        // This document consists of a single paragraph. Treat it as sequence of inline element.
        is_inline = true;
      }
    }
  }

  std::string md = output_generator::markdown::markdown_generator::render(entity);
  if (is_inline && !md.empty() && *md.rbegin() == '\n')
    md.pop_back();

  return md;
}

std::string inja_formatter::impl::embed(model::link_target&& target) {
  std::string id = this->id();

  embedded.insert({id, std::move(target)});

  return id_prefix() + id;
}

// TODO
// model::document inja_formatter::environment::impl::transform(std::function<model::entity(std::string&&)> transform_string, std::function<model::entity(model::mixin::container<>&&)> transform_inlines, std::function<model::entity(model::document&&)> transform_blocks, model::document&& document) {
//   if (document.begin() == document.end())
//     return document;
// 
//   // TODO: Consolidate this with the code in render().
//   auto as_document = [](model::entity&& entity) -> model::document {
//     if (entity.is<model::document>()) {
//       return entity.as<model::document>();
//     } else if (entity.is<model::markup::paragraph>()) {
//       model::document document {"inlines", "inlines"};
//       document.add_child(std::move(entity));
//       return document;
//     } else {
//       // TODO: This is not correct if the entity is a block.
//       model::document document {"inlines", "inlines"};
//       document.add_child(model::markup::paragraph{std::move(entity)});
//       return document;
//     }
//   };
// 
//   if (++document.begin() == document.end() && document.begin()->is<model::markup::paragraph>()) {
//     auto& paragraph = document.begin()->as<model::markup::paragraph>();
//     if (paragraph.begin() == paragraph.end())
//       return document;
// 
//     if (++paragraph.begin() == paragraph.end() && paragraph.begin()->is<model::markup::text>()) {
//       return as_document(transform_string(std::move(paragraph.begin()->as<model::markup::text>().value)));
//     } else {
//       return as_document(transform_inlines(std::move(document.begin()->as<model::markup::paragraph>())));
//     }
//   } else {
//     return as_document(transform_blocks(std::move(document)));
//   }
// }

void inja_formatter::impl::foreach(std::function<void(const model::entity&)> callback, const model::document& document) {
  if (document.begin() == document.end())
    return;

  if (++document.begin() == document.end() && document.begin()->is<model::markup::paragraph>()) {
    for (const auto & child : document.begin()->as<model::markup::paragraph>())
      callback(child);
  } else {
    for (const auto & block : document)
      callback(block);
  }
}

const std::regex& inja_formatter::impl::id_pattern() {
  const static std::regex pattern{"(.*?)" + id_prefix() + "([0-9a-f]{8})" + "(.*)"};
  return pattern;
}

std::string inja_formatter::impl::id() const {
  // TODO: It's better to randomize so ids between different formatters do not get mixed up.
  std::stringstream stream;
  stream << std::setfill ('0') << std::setw(8) << std::hex << embedded.size();
  return stream.str();
}

const std::string& inja_formatter::impl::id_prefix() {
  // TODO: Randomize
  const static std::string prefix = "id-iePh3Aic-";
  return prefix;
}

}
*/
