// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_LINK_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_DOCUMENTATION_LINK_HPP_INCLUDED

#include <type_safe/variant.hpp>

#include "link_base.hpp"
#include "container_entity.hpp"
#include "block_reference.hpp"
#include "url.hpp"

namespace standardese::output::markup
{
    /// A link to another part of the documentation.
    ///
    /// Precisely, a link to another [standardese::markup::block_entity]() or some external URL.
    class documentation_link final : public link_base
    {
    public:
        /// Builds a documentation link.
        class builder : public container_builder<documentation_link>
        {
        public:
            /// \effects Creates it giving the title and unresolved destination.
            builder(std::string title, std::string dest)
            : container_builder(std::unique_ptr<documentation_link>(
                  new documentation_link(std::move(title), std::move(dest))))
            {}

            /// \effects Creates it giving the unresolved destination only.
            builder(std::string dest) : builder("", std::move(dest)) {}

            /// \effects Creates it giving the title and an internal destination.
            builder(std::string title, block_reference dest) : builder(std::move(title), "")
            {
                peek().resolve_destination(std::move(dest));
            }

        private:
            using container_builder::peek;

            friend documentation_link;
        };

        /// \returns The internal destination of the link, if it has been resolved to an internal
        /// destination.
        type_safe::optional_ref<const block_reference> internal_destination() const noexcept
        {
            return dest_.optional_value(type_safe::variant_type<block_reference>{});
        }

        /// \returns The external destination of the link, if it has been resolved to an external
        /// destination.
        type_safe::optional_ref<const markup::url> external_destination() const noexcept
        {
            return dest_.optional_value(type_safe::variant_type<markup::url>{});
        }

        /// \returns The unresolved destination id of the link, if it hasn't been resolved already.
        /// It might have been unresolved on purpose and should render just the content.
        type_safe::optional_ref<const std::string> unresolved_destination() const noexcept
        {
            return dest_.optional_value(type_safe::variant_type<std::string>{});
        }

        /// \effects Resolves the destination of the link.
        /// \notes This function is not thread safe.
        /// \group resolve
        void resolve_destination(block_reference ref) const
        {
            dest_ = std::move(ref);
        }

        /// \group resolve
        void resolve_destination(markup::url url) const
        {
            dest_ = std::move(url);
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        std::unique_ptr<entity> do_clone() const override;

        documentation_link(std::string title, std::string dest)
        : link_base(std::move(title)), dest_(std::move(dest))
        {}

        mutable type_safe::variant<block_reference, markup::url, std::string> dest_;
    };
}

#endif

