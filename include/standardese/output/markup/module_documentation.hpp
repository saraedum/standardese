// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_MODULE_DOCUMENTATION_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_MODULE_DOCUMENTATION_HPP_INCLUDED

#include "documentation_entity.hpp"
#include "entity_index_item.hpp"

namespace standardese::output::markup
{
    /// The documentation of a module.
    ///
    /// It is meant to be used in the module index.
    class module_documentation final : public documentation_entity,
                                       public container_entity<entity_index_item>
    {
    public:
        /// Builds the documentation of a module.
        class builder : public documentation_builder<container_builder<module_documentation>>
        {
        public:
            /// \effects Creates it giving the id and header.
            builder(block_id id, type_safe::optional<documentation_header> h)
            : documentation_builder(std::unique_ptr<module_documentation>(
                  new module_documentation(std::move(id), std::move(h), nullptr)))
            {}
        };

    private:
        entity_kind do_get_kind() const noexcept override;

        void do_visit(visitor_callback_t cb, void* mem) const override;

        std::unique_ptr<entity> do_clone() const override;

        using documentation_entity::documentation_entity;
    };
}

#endif

