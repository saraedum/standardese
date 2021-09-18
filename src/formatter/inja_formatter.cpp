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

inja_formatter::inja_formatter_options::inja_formatter_options() :
  cpp_format(R"({%
      if cppast_kind in ["function", "member function", "constructor", "destructor", "conversion operator"] %}{{ format(option("function_format")) }}{%
      else if cppast_kind in ["friend"] %}{{ format(option("friend_format")) }}{%
      else if cppast_kind in ["variable", "member variable"] %}{{ format(option("variable_format")) }}{%
      else if cppast_kind in ["function template"] %}{{ format(option("template_function_format")) }}{%
      else %}TODO: not implemented {{ cppast_kind }}.{% endif %})"),
  function_format(R"({%
        if cppast_kind in ["constructor", "destructor", "conversion operator"] %}{{
          format(option("function_declarator_format"))
       }}{% else %}{{
        join(reject("empty", list(
          format(option("declaration_specifiers_format")),
          format(option("return_type_format"), return_type),
          format(option("function_declarator_format")))), " ")
      }}{% endif %}({%
        if cppast_kind in ["destructor", "conversion operator"] %}{% else %}{{
        format(option("function_parameters_format")) }}{% endif %}){%
        set suffix = join(reject("empty", list(
          format(option("const_qualification_format")),
          format(option("volatile_qualification_format")),
          format(option("ref_qualification_format")),
          format(option("noexcept_specification_format")))), " ")
      %}{% if length(suffix) %} {% endif %}{{ suffix }})"),
  template_function_format(R"(template&lt;{{ format(option("template_parameters_format")) }}&gt; {{ format(option("function_format"), entity) }})"),
  variable_format(R"({{ format(option("type_format"), variable_type) }} {{ md_escape(name) }})"),
  friend_format(R"(friend {% if cppast_kind(entity) in ["function", "member function", "constructor", "destructor", "conversion operator"] %}{{ format(option("function_format"), entity) }}{% else %}not implemented: formatting a {{ cppast_kind }} friend.{%endif%})"),
  type_format(R"({% if target != "" %}[{% endif
      %}{% if cppast_kind == "template instantiation" %}{{ format(option("type_declarator_format")) }}&lt;{% if isString(arguments) %}{{ arguments }}{% else %}TODO{% endif %}&gt;{%-
      else if cppast_kind == "reference" %}{{ format(option("type_format"), type) }}{{ format(option("ref_qualification_format"))
      }}{%- else if cppast_kind == "cv-qualified" %}{{ join(reject("empty", list(format(option("const_qualification_format")), format(option("volatile_qualification_format")), format(option("type_format"), type))), " ")
      }}{%- else %}{{ format(option("type_declarator_format")) }}{% endif %}{% if target != "" %}]({{ target }}){% endif %})"),
  return_type_format(type_format),
  parameter_type_format(type_format),
  type_declarator_format(R"({{ md_escape(join(reject("empty", list(namespace, scope, name)), "::")) }})"),
  template_parameters_format(R"({% for param in parameters %}{% if not loop.is_first %}, {% endif %}{{ format(option("template_parameter_format"), param) }}{% endfor %})"),
  // TODO
  template_parameter_format(R"({% if cppast_kind == "template type parameter" %}typename {% endif %}{{ md_escape(name) }})"),
    // TODO
  template_argument_format("TODO: template argument"),
  declaration_specifiers_format(R"({% if length(declaration_specifiers) != 0 %}{{ join(declaration_specifiers, " ") }}{% endif %})"),
  function_declarator_format(R"({{ md_escape(join(reject("empty", list(namespace, scope, name)), "::")) }})"),
  function_parameters_format(R"({% for param in parameters %}{% if not loop.is_first %}, {% endif %}{{ format(option("function_parameter_format"), param) }}{% endfor %})"),
  function_parameter_format(R"({{ format(option("parameter_type_format"), type) }}{% if name != "" %} {{ md_escape(name) }}{% endif %})"),
  const_qualification_format(R"({% if const_qualification != "" %}{{ const_qualification }}{% endif %})"),
  volatile_qualification_format(R"({% if volatile_qualification != "" %}{{ volatile_qualification }}{% endif %})"),
  ref_qualification_format(R"({% if ref_qualification != "" %}{{ md_escape(ref_qualification) }}{% endif %})"),
  // TODO
  noexcept_specification_format("")
  {}

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
  add_callback("namespace", [&]() {
    return namespace_callback(data());
  });
  add_callback("namespace", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("namespace", args, 1))
      return std::string{};
    return namespace_callback(*args[0]);
  });
  add_callback("scope", [&]() {
    return scope_callback(data());
  });
  add_callback("scope", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("scope", args, 1))
      return std::string{};
    return scope_callback(*args[0]);
  });
  add_callback("info", [&](const std::vector<const nlohmann::json*>& args) {
    if (!check_arg_count("info", args, 1))
      return nlohmann::json{};
    info_callback(*args[0]);
    return nlohmann::json{};
  });
}

inja_formatter::inja_formatter(struct inja_formatter_options options, parser::cpp_context cpp_context, const cppast::cpp_entity& context) : inja_formatter(std::move(options), std::move(cpp_context)) {
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
