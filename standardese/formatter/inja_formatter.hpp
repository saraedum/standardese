// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED
#define STANDARDESE_FORMATTER_INJA_FORMATTER_HPP_INCLUDED

#include <memory>
#include <string>
#include <functional>

#include <cppast/forward.hpp>
#include <nlohmann/json_fwd.hpp>

#include "../model/link_target.hpp"

namespace standardese::formatter {

/// Renders C++ entities using the Inja templating engine.
class inja_formatter {
 public:
  struct inja_formatter_options {
    inja_formatter_options();
    
    enum class namespace_display_options {
      /// Show all entities with full namespace qualification.
      /// This is the default behaviour.
      full,

      /// Omit namespaces that the documented entity is in.
      /// We do not handle name clashes here, i.e., the base class of `N::M::B`
      /// would be shown as `A` in this example:
      /// ```
      /// namespace N {
      ///   class A;
      ///   namespace M {
      ///     class A;
      ///     class B : N::A {}
      ///   }
      /// }
      /// ```
      relative,

      /// Completely hide all namespaces.
      hidden,
    } namespace_display_options;

    /// Formatting rule for functions.
    /// This includes member functions, constructors, destructors, operators.
    /// See https://en.cppreference.com/w/cpp/language/function.
    std::string function_format = R"({%
        if cppast_kind in ["constructor", "destructor", "conversion operator"] %}{{
          format(option("function_declarator_format"))
       }}{% else %}{{
        join(" ` ` ", reject("empty",list(
          format(option("declaration_specifiers_format")),
          format(option("return_type_format"), return_type),
          format(option("function_declarator_format")))))
      }}{% endif %} `(` {%
        if cppast_kind in ["destructor", "conversion operator"] %}{% else %}{{
        format(option("function_parameters_format")) }}{% endif %} `)` {% 
        set suffix = join(" ` ` ", reject("empty", list(
          format(option("const_qualification_format")),
          format(option("volatile_qualification_format")),
          format(option("ref_qualification_format")),
          format(option("noexcept_specification_format")))))
      %} {% if length(suffix) %}` `{% endif %} {{ suffix }})";

    std::string friend_format = R"(`friend` ` ` {{ md(code(entity)) }})";

    std::string type_format = R"({% if target != "" %}[{% endif
      %}{% if cppast_kind == "template instantiation" %}`{{ name }}` `<` {% if isString(arguments) %}`{{ arguments }}`{% else %}`TODO`{% endif %} `>` {%-
      else if cppast_kind == "reference" %}{{ format(option("type_format"), type) }}{{ format(option("ref_qualification_format"))
      }}{%- else if cppast_kind == "cv-qualified" %}{{ join(" ` ` ", reject("empty", list(format(option("const_qualification_format")), format(option("type_format"), type))))
      }}{%- else %} `{{ code_escape(name) }}` {% endif %}{% if target != "" %}]({{ target }}){% endif %})";

    /// Formatting rule for a function's return type.
    /// This controls the formatting of a function's return type with its
    /// specifiers.
    std::string return_type_format = type_format;

    std::string parameter_type_format = type_format;

    std::string template_argument_format = "template-argument";

    std::string declaration_specifiers_format = R"({% if length(declaration_specifiers) != 0 %} `{{ join(" ", declaration_specifiers) }}` {% endif %})";

    std::string function_declarator_format = " `{{ name }}` ";

    std::string function_parameters_format = R"({% for param in parameters %}{% if not loop.is_first %} `, ` {% endif %} {{ format(option("function_parameter_format"), param) }} {% endfor %})";

    std::string function_parameter_format = R"({{ format(option("parameter_type_format"), type) }}{% if name != "" %}` {{ name }}`{% endif %})";

    std::string const_qualification_format = R"({% if const_qualification != "" %} `{{ const_qualification }}` {% endif %})";

    std::string volatile_qualification_format = R"({% if volatile_qualification != "" %} `{{ volatile_qualification }}` {% endif %})";

    std::string ref_qualification_format = R"({% if ref_qualification != "" %} `{{ ref_qualification }}` {% endif %})";

    // TODO
    std::string noexcept_specification_format = "";
  };

  explicit inja_formatter(inja_formatter_options);
  ~inja_formatter();

  inja_formatter(inja_formatter_options, const model::mixin::documentation& context);

  /// Render the inja template `format`.
  std::string format(const std::string& format) const;

  /// Render the inja template `format` and parse the result as MarkDown.
  /// Returns the root node of the generated markup tree.
  model::document build(const std::string& format) const;

  nlohmann::json& data();
  const nlohmann::json& data() const;

  void add_callback(const std::string& name, std::function<nlohmann::json()>);
  void add_callback(const std::string& name, std::function<nlohmann::json(const std::vector<const nlohmann::json*>)>);
  void add_void_callback(const std::string& name, std::function<void()>);
  void add_void_callback(const std::string& name, std::function<void(std::vector<const nlohmann::json*>)>);

  nlohmann::json to_json(const cppast::cpp_entity&) const;

  nlohmann::json to_json(const cppast::cpp_type&) const;

  nlohmann::json to_json(const model::link_target&) const;

  nlohmann::json to_json(const model::entity&) const;

  /// Return a short name of this entity.
  /// This method can be invoked in inja templates as `{{ name }}` or as `{{ name(entity) }}`.
  std::string name(const cppast::cpp_entity&) const;

  /// Return a short name of this type.
  /// This method can be invoked in inja templates as `{{ name }}` or as `{{ name(type) }}`.
  std::string name(const cppast::cpp_type&) const;

  /// Return the name of this module.
  /// This method can be invoked in inja templates as `{{ name }}` or as `{{ name(entity) }}`.
  std::string name(const model::module&) const;

  /// Render the entity as MarkDown.
  /// The returned string might contain inja-formatter specific MarkDown to
  /// encode what cannot be represented with standard MarkDown.
  /// This method can be invoked in inja templates as `{{ md(entity) }}`.
  std::string md(const model::entity&) const;

  /// Return the path to the header where this entity is defined.
  /// This method can be invoked in inja templates as `{{ path }}` or as `{{
  /// path(entity) }}`.
  std::string path(const cppast::cpp_entity&) const;

  /// Return the filename part of the path.
  /// Returns `header.hpp` for `/directory/header.hpp`.
  /// This method can be invoked in inja templates as `{{ filename(path) }}`.
  std::string filename(const std::string& path) const;

  /// Return `name` as string suitable for a filename without any surprising
  /// characters.
  /// This method can be invoked in inja templates as `{{
  /// sanitize_basename(name) }}`.
  std::string sanitize_basename(const std::string& name) const;

  /// Return the argument escaped for MarkDown code (blocks.)
  std::string code_escape(const std::string&) const;

  /// Return the argument escaped for MarkDown.
  /// Ideally, this would return a string that produces the input when parsed
  /// as MarkDown, i.e., something that roundtrips. However, this is not
  /// possible since newlines and (leading) spaces cannot be properly escaped.
  std::string md_escape(const std::string&) const;

  /// Return the `items` without the entries that satisfy `predicate`.
  /// This method can be invoked in inja templates as `{{ reject("predicate",
  /// array) }}`
  std::vector<nlohmann::json> reject(const std::string& predicate, std::vector<nlohmann::json> items) const;

  /// Return the `items` joined with `separator`.
  /// This method can be invoked in inja templates as `{{ join("separator",
  /// array) }}
  std::string join(const std::string& separator, std::vector<std::string> items) const;

  /// Return the declaration specifiers such as `virtual` or `static` for this
  /// entity, see https://en.cppreference.com/w/cpp/language/declarations.
  std::vector<std::string> declaration_specifiers(const cppast::cpp_entity&) const;

  /// Return a link to this entity for MarkDown.
  /// This method can be invoked in inja templates as `{{ target }}` or `{{
  /// target(entity) }}`.
  std::string target(const cppast::cpp_entity&) const;

  /// Return a link to this type for MarkDown.
  /// Returns an empty string when no link to type can be generated.
  /// This method can be invoked in inja templates as `{{ target }}` or `{{
  /// target(type) }}`.
  std::string target(const cppast::cpp_type&) const;

  /// Return the parameters of this entity.
  /// This method can be invoked in inja templates as `{{ parameters }}` or
  /// `{{ parameters(entity) }}`.
  std::vector<const cppast::cpp_entity*> parameters(const cppast::cpp_entity&) const;

  std::string const_qualification(const cppast::cpp_entity&) const;

  std::string const_qualification(const cppast::cpp_type&) const;

  std::string volatile_qualification(const cppast::cpp_entity&) const;

  std::string ref_qualification(const cppast::cpp_entity&) const;

  std::string ref_qualification(const cppast::cpp_type&) const;

  std::string cppast_kind(const cppast::cpp_entity&) const;

  std::string cppast_kind(const cppast::cpp_type&) const;

  std::string kind(const cppast::cpp_entity&) const;

  model::document synopsis(const cppast::cpp_entity&) const;

  model::document code(const cppast::cpp_entity&) const;

  model::document code(const std::string& format, const cppast::cpp_entity&) const;

  std::string option(const std::string& key) const;

  const cppast::cpp_type& return_type(const cppast::cpp_entity&) const;

  /// Return the template arguments of this type.
  /// Normally, this returns a vector of arguments. However, when the
  /// arguments are not exposed by libclang/cppast, returns a simple string
  /// giving the arguments as they are in the source code.
  /// This method can be invoked in inja templates as `{{ arguments }}` or `{{
  /// arguments(type) }}`.
  std::variant<std::vector<std::variant<const cppast::cpp_type*, std::string>>, std::string> arguments(const cppast::cpp_type&) const;

  const cppast::cpp_type& type(const cppast::cpp_entity&) const;

  const cppast::cpp_type& type(const cppast::cpp_type&) const;

  const cppast::cpp_entity& entity(const cppast::cpp_entity&) const;

  std::string text(const model::document&) const;

  std::string replace(const std::string&, const std::string& pattern, const std::string& replacement) const;

 private:
  std::string name_callback(const nlohmann::json&) const;
  std::string md_callback(const nlohmann::json&) const;
  std::string path_callback(const nlohmann::json&) const;
  std::string filename_callback(const nlohmann::json&) const;
  std::string sanitize_basename_callback(const nlohmann::json&) const;
  std::string code_escape_callback(const nlohmann::json&) const;
  std::string md_escape_callback(const nlohmann::json&) const;
  std::string format_callback(const nlohmann::json&) const;
  std::string format_callback(const nlohmann::json&, const nlohmann::json&) const;
  nlohmann::json reject_callback(const nlohmann::json&, const nlohmann::json&) const;
  std::string join_callback(const nlohmann::json&, const nlohmann::json&) const;
  nlohmann::json declaration_specifiers_callback(const nlohmann::json&) const;
  std::string target_callback(const nlohmann::json&) const;
  nlohmann::json parameters_callback(const nlohmann::json&) const;
  std::string const_qualification_callback(const nlohmann::json&) const;
  std::string volatile_qualification_callback(const nlohmann::json&) const;
  std::string ref_qualification_callback(const nlohmann::json&) const;
  std::string cppast_kind_callback(const nlohmann::json&) const;
  std::string kind_callback(const nlohmann::json&) const;
  nlohmann::json synopsis_callback(const nlohmann::json&) const;
  nlohmann::json code_callback(const nlohmann::json&) const;
  nlohmann::json code_callback(const nlohmann::json&, const nlohmann::json&) const;
  std::string option_callback(const nlohmann::json&) const;
  nlohmann::json return_type_callback(const nlohmann::json&) const;
  nlohmann::json arguments_callback(const nlohmann::json&) const;
  nlohmann::json type_callback(const nlohmann::json&) const;
  nlohmann::json entity_callback(const nlohmann::json&) const;
  std::string text_callback(const nlohmann::json&) const;
  std::string replace_callback(const nlohmann::json&, const nlohmann::json&, const nlohmann::json&) const;

  struct impl;
  std::unique_ptr<struct impl> self;
};

}

#endif
