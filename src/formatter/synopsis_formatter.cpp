// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/code_generator.hpp>
#include <cppast/cpp_type_alias.hpp>

#include "../../standardese/formatter/synopsis_formatter.hpp"
#include "../../standardese/formatter/code_formatter.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/document.hpp"

namespace standardese::formatter {

synopsis_formatter::synopsis_formatter_options::synopsis_formatter_options() {}

synopsis_formatter::synopsis_formatter(synopsis_formatter_options options, parser::cpp_context context) : options(options), cpp_context(std::move(context)) {}

model::document synopsis_formatter::build(const model::cpp_entity_documentation& documentation) const {
  /*
  // TODO: Use synopsis override if set.
  // TODO: It should be configurable whether this generates a plain `synopsis`
  // or an actual code block with links; probably directly in HTML with eg
  // rouge classes.
  // TODO: Move this class out.
  struct generator : public cppast::code_generator {
    generator(const parser::cpp_context& context) : context(context) {}

    void do_indent() override {}
    void do_unindent() override {}
    void do_write_newline() override {}
    bool do_write_reference(type_safe::array_ref<const cppast::cpp_entity_id> id, cppast::string_view tokens) override {
      // TODO: assert(id.begin() != id.end());

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
      // // TODO: Use an inja callback instead and put this reference into some state bag. See the corresponding hack in link_target_transformation.
      // if (declaration.has_value()) {
      //   std::stringstream s;
      //   s << "standardese://@";
      //   s << (long)&declaration.value() << std::flush;
      //   href = s.str();
      // }

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

    const parser::cpp_context context;
    bool ws = false;
    bool incode = false;
    std::string code;
  } generator{documentation.context()};

  cppast::generate_code(generator, documentation.entity());

  return generator;
  */

  code_formatter::code_formatter_options options;
  static_cast<inja_formatter::inja_formatter_options&>(options) = this->options;

  return code_formatter{options, cpp_context}.build(documentation.entity(), documentation);
}

}
