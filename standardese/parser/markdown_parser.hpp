// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_PARSER_MARKDOWN_PARSER_HPP_INCLUDED
#define STANDARDESE_PARSER_MARKDOWN_PARSER_HPP_INCLUDED

#include <memory>
#include <functional>
#include <string>

#include "../forward.hpp"

extern "C" {
    typedef struct cmark_parser cmark_parser;
    typedef struct cmark_node cmark_node;
}

namespace standardese::parser {

/// A CommonMark Markdown parser that parser into `model::markup` entities.
class markdown_parser
{
  public:
    virtual ~markdown_parser();

    /// Parse the markdown formatted string into a tree of markup nodes.
    /// This method ignores standardese commands and just treats them as text.
    model::document parse(const std::string&) const;

    /// Escape the string such that the markdown parser treats it as plain text.
    static std::string escape(const std::string&);

  protected:
    template <typename T, auto free>
    using unique_cmark = std::unique_ptr<T, std::integral_constant<decltype(free), free>>;

    /// Invoke `callback` for each node under `root`.
    void visit(cmark_node* root, std::function<cmark_node*(cmark_node*)> callback) const;

    /// Invoke `callback` for each child of `parent`.
    void visit_children(cmark_node* parent, std::function<void(cmark_node*)> callback) const;

    /// Parse the contents of `node` into an equivalent model.
    virtual model::entity parse(cmark_node* node) const;

    static void cmark_parser_free_with_extensions(cmark_parser*);
};

}

#endif
