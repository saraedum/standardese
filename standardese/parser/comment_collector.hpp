// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_COMMENT_COLLECTOR_HPP_INCLUDED
#define STANDARDESE_PARSER_COMMENT_COLLECTOR_HPP_INCLUDED

#include <cppast/cppast_fwd.hpp>
#include <type_safe/reference.hpp>
#include <vector>

namespace standardese::parser
{
    /// Picks up all source code comments in a C/C++ source file.
    class comment_collector
    {
    public:
        struct comment_collector_options {
        };

        explicit comment_collector(comment_collector_options options);

        // TODO(0.6.0-alpha): Drop type_safe::object_ref where possible.
        using comment = std::tuple<std::string, type_safe::object_ref<const cppast::cpp_entity>>;

        /// Return a collection of all the source code comments and the
        /// corresponding C++ entities (children of [header]()) they are
        /// commenting.
        std::vector<comment> collect(const cppast::cpp_file* header);

      private:
        comment_collector_options options;
    };
}
#endif
