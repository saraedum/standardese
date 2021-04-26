// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_COMMENT_PARSER_HPP_INCLUDED
#define STANDARDESE_PARSER_COMMENT_PARSER_HPP_INCLUDED

#include <regex>
#include <cppast/forward.hpp>
#include <type_safe/reference.hpp>
#include <type_safe/optional_ref.hpp>
#include <type_safe/variant.hpp>

#include "../model/module.hpp"
#include "../model/unordered_entities.hpp"
#include "cpp_context.hpp"
#include "markdown_parser.hpp"

namespace standardese::parser
{
    /// A CommonMark Markdown parser with standardese syntax extensions.
    class comment_parser : public markdown_parser
    {
    public:
        // TODO: The options options scheme we are using elsewhere is going to
        // bite us with things such as cppyy. Let's use this scheme instead,
        // i.e., `comment_parser_options options;`.

        struct comment_parser_options {
          /// \param command_character form commands by prefixing this to the command name.
          ///
          /// \param command_patterns Override or complement command patterns with the ones given here.
          /// E.g., `brief=SUMMARY:` lets us write `SUMMARY:` instead of `\brief`.
          comment_parser_options(char command_character = '\\', const std::vector<std::string>& command_patterns = {});

          /// Whether commands should be applied to the entire file if they
          /// cannot be matched to another entity.
          bool free_file_comments = false;

          struct command_extension_options {
            std::regex end_command_pattern;
            std::regex exclude_command_pattern;
            std::regex unique_name_command_pattern;
            std::regex output_name_command_pattern;
            std::regex synopsis_command_pattern;
            std::regex group_command_pattern;
            std::regex module_command_pattern;
            std::regex output_section_command_pattern;
            std::regex entity_command_pattern;
            std::regex file_command_pattern;

            std::regex brief_command_pattern;
            std::regex details_command_pattern;
            std::regex requires_command_pattern;
            std::regex effects_command_pattern;
            std::regex synchronization_command_pattern;
            std::regex postconditions_command_pattern;
            std::regex returns_command_pattern;
            std::regex throws_command_pattern;
            std::regex complexity_command_pattern;
            std::regex remarks_command_pattern;
            std::regex error_conditions_command_pattern;
            std::regex notes_command_pattern;
            std::regex preconditions_command_pattern;
            std::regex constraints_command_pattern;
            std::regex diagnostics_command_pattern;
            std::regex see_command_pattern;
            std::regex parameters_command_pattern;
            std::regex bases_command_pattern;

            std::regex param_command_pattern;
            std::regex tparam_command_pattern;
            std::regex base_command_pattern;
          } command_extension_options;
        };

        comment_parser(comment_parser_options options, const cpp_context& context);

        using entity_resolver = std::function<type_safe::optional_ref<const cppast::cpp_entity>(const std::string&)>;

        /// Parse a `comment` attached to the C++ `entity`.
        /// If the comment is not right next to an entity in the source code,
        /// `entity` should be the containing header file.
        ///
        /// \param entity_resolver A callback which resolves an `\entity`
        /// command to the corresponding C++ entity.
        ///
        /// \throws [*parse_error]() if an error occurred.
        ///
        /// \notes This operation is thread-safe.
        ///
        /// \returns The vector of C++ entities with their corresponding
        /// documentation. Additionally, this contains entities for modules
        /// which have no correspondance in C++ source code.
        std::vector<model::entity> parse(const std::string& comment, const cppast::cpp_entity& entity, entity_resolver entity_resolver);

        /// Add all entities from this file to the parse result that are
        /// lacking explicit comments.
        /// TODO: Should be a transformation? Anyway, it should not live here.
        void add_uncommented_entities(model::unordered_entities&, const cppast::cpp_file&) const;

        /// Add modules to the parse result that are mentioned in other
        /// comments but lack explicit documentation.
        /// TODO: Should be a transformation? Anyway, it should not live here.
        void add_uncommented_modules(model::unordered_entities&) const;

    private:
        /// Return the effective C++ entity (or the module name) the comment in
        /// `root` is referencing given that comment was found next to
        /// `entity`.
        type_safe::variant<const cppast::cpp_entity*, std::string> resolve_entity(cmark_node* root, const cppast::cpp_entity& entity, entity_resolver entity_resolver) const;

        /// Resolve any inline commands such as \param relative to `entity` and
        /// remove their subtrees from `root`.
        std::vector<model::entity> extract_inlines(cmark_node* root, const cppast::cpp_entity& entity) const;

        /// Return the base `name` of the type `entity`.
        const cppast::cpp_entity& resolve_base(const cppast::cpp_entity& entity, const std::string& name) const;

        /// Return the parameter `name` of the `entity`.
        const cppast::cpp_entity& resolve_param(const cppast::cpp_entity& entity, const std::string& name) const;

        /// Return the template parameter `name` of the function `entity`.
        const cppast::cpp_entity& resolve_tparam(const cppast::cpp_entity& entity, const std::string& name) const;

        /// Parse the children of `root` as a description for `entity` into the `entities` set.
        void parse(cmark_node* root, const cppast::cpp_entity& entity, std::vector<model::entity>& entities) const;

        /// Add the contents of `node` to the documentation of `model`.
        template <typename T>
        void parse(cmark_node* node, T& model) const;

        /// Add the contents of the special `command` to the documentation of `model`.
        template <typename T>
        void apply_command(cmark_node* command, T& model) const;

        /// Parse the contents of the `section` into an equivalent model.
        model::entity parse_section(cmark_node* section) const;

        /// Parse the contents of `node` into an equivalent model.
        model::entity parse(cmark_node* node) const override;

        const comment_parser_options options;
        const cpp_context context;
    };
}
#endif
