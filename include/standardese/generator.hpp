// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_GENERATOR_HPP_INCLUDED
#define STANDARDESE_GENERATOR_HPP_INCLUDED

#include <standardese/synopsis.hpp>
#include <standardese/translation_unit.hpp>

namespace standardese
{
    class parser;

    class md_document final : public md_container
    {
    public:
        static md_entity::type get_entity_type() STANDARDESE_NOEXCEPT
        {
            return md_entity::document_t;
        }

        static md_ptr<md_document> make();

        md_entity_ptr clone() const
        {
            return do_clone(nullptr);
        }

    protected:
        md_entity_ptr do_clone(const md_entity* parent) const override;

    private:
        md_document(cmark_node* node) : md_container(get_entity_type(), node)
        {
        }

        friend detail::md_ptr_access;
    };

    const char* get_entity_type_spelling(cpp_entity::type t);

    void generate_doc_entity(const parser& p, md_document& document, unsigned level,
                             const doc_entity& e);

    md_ptr<md_document> generate_doc_file(const parser& p, const cpp_file& f);
} // namespace standardese

#endif // STANDARDESE_GENERATOR_HPP_INCLUDED
