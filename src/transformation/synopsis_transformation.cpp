// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/code_generator.hpp>
#include <cppast/cpp_class.hpp>

#include "../../standardese/transformation/synopsis_transformation.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"

namespace standardese::transformation {
namespace {

model::entity synopsis(const cppast::cpp_entity& entity) {
  // TODO: Bring 'hidden' functionality back:
  /*
  const char* synopsis_config::default_hidden_name() noexcept { return "'hidden'"; }
  bool write_link(const doc_entity& entity, cppast::string_view name)
  {
      if (is_documented(entity))
      {
          // only generate link if the entity has actual documentation
          standardese::model::documentation_link link(entity.link_name());
          link.add_child(standardese::model::code_block::identifier::build(name.c_str()));
          builder_.add_child(link.finish());
      }
      else if (entity.is_excluded())
      {
          write_excluded();
          return false;
      }
      else
          write_identifier(name);

      return true;
  }
  */

  // TODO: Bring group/output sections back:
  /*
  synopsis_config::flags synopsis_config::default_flags() noexcept { return synopsis_config::show_group_output_section; }
  void doc_cpp_entity::do_generate_synopsis_prefix(const cppast::code_generator::output& output,
                                                   const synopsis_config& config, bool is_main) const
  {
      auto metadata
          = comment().map([](const comment::doc_comment& c) { return type_safe::ref(c.metadata()); });

      if (metadata)
      {
          generate_output_section(output, is_main,
                                  config.is_flag_set(synopsis_config::show_group_output_section),
                                  metadata.value(), group_member_no_);
          if (is_main)
              generate_group_number(output, metadata.value(), group_member_no_);

          generate_synopsis_override(output, metadata.value());
      }
  }

  void doc_metadata_entity::do_generate_synopsis_prefix(const cppast::code_generator::output& output,
                                                        const synopsis_config&                config,
                                                        bool is_main) const
  {
      auto metadata = comment().value().metadata();

      generate_output_section(output, is_main,
                              config.is_flag_set(synopsis_config::show_group_output_section),
                              metadata, type_safe::nullopt);
      generate_synopsis_override(output, metadata);
  }

  void doc_cpp_namespace::do_generate_synopsis_prefix(const cppast::code_generator::output& code,
                                                      const synopsis_config&, bool is_main) const
  {
      auto metadata
          = comment().map([](const comment::doc_comment& c) { return type_safe::ref(c.metadata()); });

      if (metadata)
          // only support output section
          generate_output_section(code, is_main, metadata.value());
  }

  bool generate_output_section(const cppast::code_generator::output& code, bool is_main,
                               const comment::metadata& metadata)
  {
      if (!is_main && metadata.output_section())
      {
          code << cppast::comment("//=== ") << cppast::comment(metadata.output_section().value())
               << cppast::comment(" ===//") << cppast::newl;
          return true;
      }
      else
          return false;
  }

  void generate_output_section(const cppast::code_generator::output& code, bool is_main,
                               bool show_group_section, const comment::metadata& metadata,
                               type_safe::optional<unsigned> group_member_no)
  {
      if (generate_output_section(code, is_main, metadata))
          return;
      else if (!is_main && metadata.group() && show_group_section
               && metadata.group().value().output_section() && group_member_no == 1u)
          code << cppast::comment("//=== ")
               << cppast::comment(metadata.group().value().output_section().value())
               << cppast::comment(" ===//") << cppast::newl;
  }

  void generate_group_number(const cppast::code_generator::output& code,
                             const comment::metadata&              metadata,
                             type_safe::optional<unsigned>         group_member_no)
  {
      assert(metadata.group().has_value() == group_member_no.has_value());

      if (metadata.group())
      {
          if (group_member_no.value() != 1u)
              code << cppast::newl;
          code << cppast::comment("(" + std::to_string(group_member_no.value()) + ") ");
      }
  }
  */
  // TODO: Implement ordering (was that ever implemented?):
  // entity_index::order generation_config::default_order() noexcept { return entity_index::namespace_inline_sorted; }

  // TODO: Bring back the different exclude modes:
  /*
    cppast::code_generator::generation_options get_exclude_mode(
        type_safe::optional_ref<const comment::metadata> metadata)
    {
        if (!metadata || !metadata.value().exclude())
            return {};

        switch (metadata.value().exclude().value())
        {
        case comment::exclude_mode::entity:
            assert(false);
        case comment::exclude_mode::return_type:
            return cppast::code_generator::exclude_return;
        case comment::exclude_mode::target:
            return cppast::code_generator::exclude_target;
        }

        assert(false);
        return {};
    }
   */

  // TODO: Bring back synopsis override:
  /*
    void generate_synopsis_override(const cppast::code_generator::output& code,
                                    const comment::metadata&              metadata)
    {
        if (metadata.synopsis())
            code << cppast::token_seq(metadata.synopsis().value());
    }
   */

  // TODO: Bring back macro replacement logic
  /*
      cppast::code_generator::generation_options result;
      if (!config_->is_flag_set(synopsis_config::show_complex_noexcept))
          result |= generation_flags::exclude_noexcept_condition;
      else if (e.kind() == cppast::cpp_macro_definition::kind()
               && !config_->is_flag_set(synopsis_config::show_macro_replacement))
          result |= generation_flags::declaration;
  */

  // TODO: Bring back noexcept logic
  /*
      show_complex_noexcept,     //< If set, complex noexcept conditions will be shown.
  */

  struct generator : public cppast::code_generator {
    void do_indent() override {
      // TODO: This used to be configurable:
      // unsigned synopsis_config::default_tab_width() noexcept { return 4u; }
      indent += 2;
    }
    void do_unindent() override {
      if (indent) indent -= 2;
    }
    void do_write_punctuation(cppast::string_view tokens) override {
      if (tokens.length() == 1 && tokens.c_str()[0] == ',')
        do_write_token_seq(", ");
      else
        do_write_token_seq(tokens);
    }
    bool do_write_reference(type_safe::array_ref<const cppast::cpp_entity_id> id, cppast::string_view tokens) override {
      (void)id;
      std::string name{tokens.c_str(), tokens.c_str() + tokens.length()};
      do_write_token_seq(tokens);
      return true;
    }
    void do_write_token_seq(cppast::string_view tokens) override {
      if (need_indent) {
        code += std::string(indent, ' ');
        need_indent = false;
      }
      code += std::string(tokens.c_str(), tokens.c_str() + tokens.length());
    }
    void do_write_newline() override {
      // Omit empty lines.
      if (need_indent) return;
      do_write_token_seq("\n");
      need_indent = true;
    }
    
    bool need_indent = true;
    int indent = 0;
    std::string code;
  } generator;

  cppast::generate_code(generator, entity);

  return model::markup::code_block("cpp", generator.code);
}

}

void synopsis_transformation::do_transform(model::entity& document) {
  model::visitor::visit([&](auto&& documentation) {
    using T = std::decay_t<decltype(documentation)>;

    if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
      const cppast::cpp_entity& entity = documentation.entity();

      // TODO: Make configurable
      switch (entity.kind()) {
        case cppast::cpp_entity_kind::class_t:
          switch (static_cast<const cppast::cpp_class&>(entity).class_kind()) {
            case cppast::cpp_class_kind::struct_t:
              documentation.insert_child(synopsis(documentation.entity()));
              break;
            default:
              break;
          }
        case cppast::cpp_entity_kind::type_alias_t:
        default:
          break;
      }
    }

    return model::visitor::recursion::RECURSE;
  }, document);
}

}
