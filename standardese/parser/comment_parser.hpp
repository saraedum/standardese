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
#include "comment_parser_options.hpp"
#include "cpp_context.hpp"

extern "C" {
    typedef struct cmark_parser cmark_parser;
    typedef struct cmark_node cmark_node;
}

namespace standardese::parser
{
    /// A CommonMark Markdown parser with standardese syntax extensions.
    class comment_parser
    {
    public:
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

        /// TODO: This should not live in this class probably and should share code with the above.
        model::document parse(const std::string& comment);

        /// Add all entities from this file to the parse result that are
        /// lacking explicit comments.
        /// TODO: Should be a transformation?
        void add_uncommented_entities(model::unordered_entities&, const cppast::cpp_file&) const;

        /// Add modules to the parse result that are mentioned in other
        /// comments but lack explicit documentation.
        /// TODO: Should be a transformation?
        void add_uncommented_modules(model::unordered_entities&) const;

    private:
        // TODO: Hide all this in the .cpp file.

        static void cmark_parser_free_with_extensions(cmark_parser*);

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

        /// Parse the contents of `node` into an equivalent model.
        template <typename T = model::entity>
        T parse(cmark_node* node) const;

        /// Add the contents of the special `command` to the documentation of `model`.
        template <typename T>
        void apply_command(cmark_node* command, T& model) const;

        /// Parse the contents of the `section` into an equivalent model.
        model::entity parse_section(cmark_node* section) const;

        /// Invoke `callback` for each node under `root`.
        void visit(cmark_node* root, std::function<cmark_node*(cmark_node*)> callback) const;

        /// Invoke `callback` for each child of `parent`.
        void visit_children(cmark_node* parent, std::function<void(cmark_node*)> callback) const;

        const comment_parser_options options;
        const cpp_context context;
    };
}
#endif
