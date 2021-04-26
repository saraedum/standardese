// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/cpp_class.hpp>
#include <cppast/cpp_template.hpp>
#include <cppast/code_generator.hpp>
#include <cppast/cpp_type_alias.hpp>
#include <fmt/format.h>

#include "../../standardese/transformation/entity_heading_transformation.hpp"
#include "../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/heading.hpp"
#include "../../standardese/model/markup/code.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/output_generator/markdown/markdown_generator.hpp"
#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/parser/cpp_context.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/parser/cppast_parser.hpp"

namespace standardese::transformation {

namespace {

// TODO: Move implementation to the bottom and only define the class here.
struct visitor : public model::visitor::recursive_visitor<false> {
  visitor(struct entity_heading_transformation::options& options) : options(options) {}

  void visit(model::cpp_entity_documentation& documentation) override {
    // TODO: Do not go beyond H5. Since H6 is reserved for sections.

    auto heading = model::markup::heading(level++, "");
    for (auto& child : title(documentation))
      heading.emplace_child(std::move(child));
    documentation.insert_child(heading);

    model::visitor::recursive_visitor<false>::visit(documentation);
    level--;
  }

  model::markup::paragraph title(const model::cpp_entity_documentation& documentation) {
    // TODO: Eliminate the brief section if it is already used in the heading.

    const auto& entity = documentation.entity();

    std::string format;
    switch(entity.kind()) {
      case cppast::cpp_entity_kind::function_t:
        format = options.function_heading;
        break;
      case cppast::cpp_entity_kind::file_t:
        format = options.file_heading;
        break;
      case cppast::cpp_entity_kind::class_t:
        format = options.class_heading;
        break;
      case cppast::cpp_entity_kind::type_alias_t:
        format = options.type_alias_heading;
        break;
      case cppast::cpp_entity_kind::member_variable_t:
        format = options.member_variable_heading;
        break;
      default:
        format = "{kind} {name}";
        break;
    }

    // TODO: Use inja instead. Escape all our replacements, then parse the output as MarkDown into a markup tree.
    std::string formatted = fmt::format(format,
        fmt::arg("name", name(entity)),
        fmt::arg("brief", brief(documentation)),
        fmt::arg("kind", kind(entity)),
        fmt::arg("synopsis", synopsis(documentation))
    );

    // TODO: Use a MarkDown parser without the command extension instead.
    auto options = parser::comment_parser::comment_parser_options{};
    options.free_file_comments = true;
    auto parser = parser::comment_parser(options, documentation.context()); 
    auto entities = parser.parse(formatted, entity, [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
        throw std::logic_error("not implemented: resolve in entity_heading_transformation");
    });

    assert(entities.size());

    return entities.begin()->as<model::cpp_entity_documentation>().begin()->as<model::section>().begin()->as<model::markup::paragraph>();
  }

  std::string name(const cppast::cpp_entity& entity) {
    switch (entity.kind()) {
      case cppast::cpp_entity_kind::file_t:
        return boost::filesystem::path(entity.name()).filename().generic_string();
      default:
        return entity.name();
    }
  }

  std::string synopsis(const model::cpp_entity_documentation& entity) {
    // TODO: This should live somewhere else and be consolidated with the
    // synopsis_transformation code.
    // TODO: It should be configurable whether this generates a plain `synopsis`
    // or an actual code block with links; probably directly in HTML with eg
    // rouge classes.
    struct generator : public cppast::code_generator {
      generator(const parser::cpp_context& context) : context(context) {}

      void do_indent() override {}
      void do_unindent() override {}
      void do_write_newline() override {}
      bool do_write_reference(type_safe::array_ref<const cppast::cpp_entity_id> id, cppast::string_view tokens) override {
        assert(id.begin() != id.end());

        auto declaration = context.index().lookup(*id.begin());
        // TODO: This code is duplicated. The same lives in symbols lookup.
        // It is customary to write `typedef struct S {} S;` or `typedef struct {}
        // S;` especially in C. Technically, an "S" would refer to the typedef, but
        // this is never what people want, instead we refer to the struct.
        // TODO: How should we do this exactly? Should we run this until we are at a non-trivial typedef?
        if (declaration && declaration.value().kind() == cppast::cpp_type_alias::kind()) {
          const auto& alias = static_cast<const cppast::cpp_type_alias&>(declaration.value());
          const auto& type = alias.underlying_type();
          if (type.kind() == cppast::cpp_type_kind::user_defined_t) {
            declaration = context.index().lookup(*static_cast<const cppast::cpp_user_defined_type&>(type).entity().id().begin());
          }
        }

        std::string href = "";
        if (declaration.has_value()) {
          // TODO: Use an inja callback instead and put this reference into some state bag. See the corresponding hack in link_target_transformation.
          std::stringstream s;
          s << "standardese://@";
          s << (long)&declaration.value() << std::flush;
          href = s.str();
        }

        std::string name{tokens.c_str(), tokens.c_str() + tokens.length()};
        // TODO: Allow certain type replacements here, e.g., mp_limb_signed_t should be long with a tooltip explaining that this is not what it seems.
        // TODO: Emit a link to the external type if this is defined externally.
        if (href == "") {
          do_write_token_seq(name);
        } else {
          disable();
          write("[`" + name + "`](" + href + ")");
        }
        return true;
      }
      void do_write_whitespace() override {
        ws = true;
      }
      void do_write_punctuation(cppast::string_view tokens) override {
        if (tokens.length() == 1 && tokens.c_str()[0] == ';') return;
        if (tokens.length() == 1 && tokens.c_str()[0] == ',') {
            enable();
            write(",");
            ws = true;
        } else do_write_token_seq(tokens);
      }
      void do_write_token_seq(cppast::string_view tokens) override {
        enable();
        write(tokens);
      }

      void enable() {
        if (!incode) {
          if (ws) {
            code+=" ";
            ws = false;
          }
          code += "`";
        }
        incode = true;
      }

      void disable() {
        if (incode) {
          code += "`";
          if (ws) {
            code+=" ";
            ws = false;
          }
        }
        incode = false;
      }

      void write(cppast::string_view tokens) {
        if (ws) {
          code+=" ";
          ws = false;
        }
        code += std::string(tokens.c_str(), tokens.c_str() + tokens.length());
      }

      operator std::string() {
        disable();
        return code;
      };

      // TODO: This code is obviously duplicated with the block below.
      std::string kind(type_safe::object_ref<const cppast::cpp_entity> entity)
      {
          switch (entity->kind())
          {
          case cppast::cpp_entity_kind::file_t:
              return "Header file";

          case cppast::cpp_entity_kind::macro_parameter_t:
              return "Macro Parameter";
          case cppast::cpp_entity_kind::macro_definition_t:
              return "Macro";
          case cppast::cpp_entity_kind::include_directive_t:
              return "Inclusion directive";

          case cppast::cpp_entity_kind::language_linkage_t:
              return "Language linkage";
          case cppast::cpp_entity_kind::namespace_t:
              return "Namespace";
          case cppast::cpp_entity_kind::namespace_alias_t:
              return "Namespace alias";
          case cppast::cpp_entity_kind::using_directive_t:
              return "Using directive";
          case cppast::cpp_entity_kind::using_declaration_t:
              return "Using declaration";

          case cppast::cpp_entity_kind::type_alias_t:
              return "Type alias";

          case cppast::cpp_entity_kind::enum_t:
              return "Enumeration";
          case cppast::cpp_entity_kind::enum_value_t:
              return "Enumeration constant";

          case cppast::cpp_entity_kind::class_t:
              switch (static_cast<const cppast::cpp_class&>(*entity).class_kind())
              {
              case cppast::cpp_class_kind::class_t:
                  return "Class";
              case cppast::cpp_class_kind::struct_t:
                  return "Struct";
              case cppast::cpp_class_kind::union_t:
                  return "Union";
              }
              break;
          case cppast::cpp_entity_kind::access_specifier_t:
              return "Access specifier";
          case cppast::cpp_entity_kind::base_class_t:
              return "Base class";

          case cppast::cpp_entity_kind::member_variable_t:
              return "Member variable";

          case cppast::cpp_entity_kind::variable_t:
          case cppast::cpp_entity_kind::bitfield_t:
              return "Variable";

          case cppast::cpp_entity_kind::function_parameter_t:
              return "Parameter";
          case cppast::cpp_entity_kind::function_t:
          case cppast::cpp_entity_kind::member_function_t:
              // TODO: recognize (special) operators
              return "Function";
          case cppast::cpp_entity_kind::conversion_op_t:
              return "Conversion operator";
          case cppast::cpp_entity_kind::constructor_t:
              // TODO: recognize special constructors
              return "Constructor";
          case cppast::cpp_entity_kind::destructor_t:
              return "Destructor";
          case cppast::cpp_entity_kind::friend_t:
              return "Friend function";

          case cppast::cpp_entity_kind::template_type_parameter_t:
          case cppast::cpp_entity_kind::non_type_template_parameter_t:
          case cppast::cpp_entity_kind::template_template_parameter_t:
              return "Template parameter";

          case cppast::cpp_entity_kind::alias_template_t:
              return "Alias template";
          case cppast::cpp_entity_kind::variable_template_t:
              return "Variable template";

          case cppast::cpp_entity_kind::function_template_t:
          case cppast::cpp_entity_kind::function_template_specialization_t:
          case cppast::cpp_entity_kind::class_template_t:
          case cppast::cpp_entity_kind::class_template_specialization_t:
              return kind(type_safe::ref(*static_cast<const cppast::cpp_template&>(*entity).begin()));

          case cppast::cpp_entity_kind::static_assert_t:
              return "Static assertion";

          case cppast::cpp_entity_kind::unexposed_t:
              return "Unexposed entity";

          case cppast::cpp_entity_kind::count:
              break;
          }

          throw std::logic_error("not implemented: unsupported entity kind");
      }

      const parser::cpp_context context;
      bool ws = false;
      bool incode = false;
      std::string code;
    } generator{entity.context()};

    cppast::generate_code(generator, entity.entity());

    return generator;
  }

  std::string brief(const model::cpp_entity_documentation& documentation) {
    for (auto& child : documentation) {
      if (child.is<model::section>() && child.as<model::section>().type == parser::commands::section_command::brief)
        return output_generator::markdown::markdown_generator::render(child);
    }
    return "";
  }

  std::string kind(const cppast::cpp_entity& entity)
  {
      switch (entity.kind())
      {
      case cppast::cpp_entity_kind::file_t:
          return "Header file";

      case cppast::cpp_entity_kind::macro_parameter_t:
          return "Macro Parameter";
      case cppast::cpp_entity_kind::macro_definition_t:
          return "Macro";
      case cppast::cpp_entity_kind::include_directive_t:
          return "Inclusion directive";

      case cppast::cpp_entity_kind::language_linkage_t:
          return "Language linkage";
      case cppast::cpp_entity_kind::namespace_t:
          return "Namespace";
      case cppast::cpp_entity_kind::namespace_alias_t:
          return "Namespace alias";
      case cppast::cpp_entity_kind::using_directive_t:
          return "Using directive";
      case cppast::cpp_entity_kind::using_declaration_t:
          return "Using declaration";

      case cppast::cpp_entity_kind::type_alias_t:
          return "Type alias";

      case cppast::cpp_entity_kind::enum_t:
          return "Enumeration";
      case cppast::cpp_entity_kind::enum_value_t:
          return "Enumeration constant";

      case cppast::cpp_entity_kind::class_t:
          switch (static_cast<const cppast::cpp_class&>(entity).class_kind())
          {
          case cppast::cpp_class_kind::class_t:
              return "Class";
          case cppast::cpp_class_kind::struct_t:
              return "Struct";
          case cppast::cpp_class_kind::union_t:
              return "Union";
          }
          break;
      case cppast::cpp_entity_kind::access_specifier_t:
          return "Access specifier";
      case cppast::cpp_entity_kind::base_class_t:
          return "Base class";

      case cppast::cpp_entity_kind::member_variable_t:
          return "Member variable";

      case cppast::cpp_entity_kind::variable_t:
      case cppast::cpp_entity_kind::bitfield_t:
          return "Variable";

      case cppast::cpp_entity_kind::function_parameter_t:
          return "Parameter";
      case cppast::cpp_entity_kind::function_t:
      case cppast::cpp_entity_kind::member_function_t:
          // TODO: recognize (special) operators
          return "Function";
      case cppast::cpp_entity_kind::conversion_op_t:
          return "Conversion operator";
      case cppast::cpp_entity_kind::constructor_t:
          // TODO: recognize special constructors
          return "Constructor";
      case cppast::cpp_entity_kind::destructor_t:
          return "Destructor";
      case cppast::cpp_entity_kind::friend_t:
          return "Friend function";

      case cppast::cpp_entity_kind::template_type_parameter_t:
      case cppast::cpp_entity_kind::non_type_template_parameter_t:
      case cppast::cpp_entity_kind::template_template_parameter_t:
          return "Template parameter";

      case cppast::cpp_entity_kind::alias_template_t:
          return "Alias template";
      case cppast::cpp_entity_kind::variable_template_t:
          return "Variable template";

      case cppast::cpp_entity_kind::function_template_t:
      case cppast::cpp_entity_kind::function_template_specialization_t:
      case cppast::cpp_entity_kind::class_template_t:
      case cppast::cpp_entity_kind::class_template_specialization_t:
          return kind(*static_cast<const cppast::cpp_template&>(entity).begin());

      case cppast::cpp_entity_kind::static_assert_t:
          return "Static assertion";

      case cppast::cpp_entity_kind::unexposed_t:
          return "Unexposed entity";

      case cppast::cpp_entity_kind::count:
          break;
      }

      throw std::logic_error("not implemented: unsupported entity kind");
  }

 private:
  const struct entity_heading_transformation::options options;

  int level = 1;
};
}

entity_heading_transformation::entity_heading_transformation(model::unordered_entities& entities, struct options options) : transformation(entities), options(options) {}

entity_heading_transformation::options::options() {}

void entity_heading_transformation::do_transform(model::entity& entity) {
  visitor v(options);
  entity.accept(v);
}

}
