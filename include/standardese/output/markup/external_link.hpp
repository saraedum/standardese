// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_MARKUP_EXTERNAL_LINK_HPP_INCLUDED
#define STANDARDESE_OUTPUT_MARKUP_EXTERNAL_LINK_HPP_INCLUDED

#include "link_base.hpp"
#include "url.hpp"

namespace standardese::output::markup
{
    /// A link to some external URL.
    class external_link final : public link_base
    {
    public:
        /// Builds an external link.
        class builder : public container_builder<external_link>
        {
        public:
            /// \effects Creates it giving the title and URL.
            builder(std::string title, markup::url u)
            : container_builder(
                  std::unique_ptr<external_link>(new external_link(std::move(title), std::move(u))))
            {}

            /// \effects Creates it giving the URL only.
            builder(markup::url url) : builder("", std::move(url)) {}
        };

        /// \returns The URL of the external location it links to.
        const markup::url& url() const noexcept
        {
            return url_;
        }

    private:
        entity_kind do_get_kind() const noexcept override;

        std::unique_ptr<entity> do_clone() const override;

        external_link(std::string title, markup::url url)
        : link_base(std::move(title)), url_(std::move(url))
        {}

        markup::url url_;
    };
}

#endif
