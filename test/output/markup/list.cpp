// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include <algorithm>

#include "../../../include/standardese/output/markup/unordered_list.hpp"
#include "../../../include/standardese/output/markup/ordered_list.hpp"
#include "../../../include/standardese/output/markup/list_item.hpp"
#include "../../../include/standardese/output/markup/paragraph.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/term.hpp"
#include "../../../include/standardese/output/markup/description.hpp"
#include "../../../include/standardese/output/markup/term_description_item.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("unordered_list", "[markup]")
{
    auto html = R"(<ul id="standardese-list">
<li>
<p></p>
<p></p>
</li>
<li>
<p>text</p>
</li>
<li>
<dl class="standardese-term-description-item">
<dt>A term</dt>
<dd>&mdash; A description</dd>
</dl>
</li>
</ul>
)";
    auto xml  = R"(<unordered-list id="list">
<list-item>
<paragraph></paragraph>
<paragraph></paragraph>
</list-item>
<list-item>
<paragraph>text</paragraph>
</list-item>
<term-description-item>
<term>A term</term>
<description>A description</description>
</term-description-item>
</unordered-list>
)";
    auto md   = std::string(R"(  -$

  - text

  - A term &mdash; A description
)");
    std::replace(md.begin(), md.end(), '$', ' ');

    unordered_list::builder builder(block_id("list"));
    builder.add_item(list_item::builder()
                         .add_child(paragraph::builder().finish())
                         .add_child(paragraph::builder().finish())
                         .finish());
    builder.add_item(
        list_item::build(paragraph::builder().add_child(text::build("text")).finish()));
    builder.add_item(
        term_description_item::build(block_id(), term::build(text::build("A term")),
                                     description::build(text::build("A description"))));

    auto ptr = builder.finish()->clone();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}

TEST_CASE("ordered_list", "[markup]")
{
    auto html = R"(<ol id="standardese-list">
<li>
<p></p>
<p></p>
</li>
<li>
<p>text</p>
</li>
</ol>
)";
    auto xml  = R"(<ordered-list id="list">
<list-item>
<paragraph></paragraph>
<paragraph></paragraph>
</list-item>
<list-item>
<paragraph>text</paragraph>
</list-item>
</ordered-list>
)";
    auto md   = std::string(R"(1.$$

2.  text
)");
    std::replace(md.begin(), md.end(), '$', ' ');

    ordered_list::builder builder(block_id("list"));
    builder.add_item(list_item::builder()
                         .add_child(paragraph::builder().finish())
                         .add_child(paragraph::builder().finish())
                         .finish());
    builder.add_item(
        list_item::build(paragraph::builder().add_child(text::build("text")).finish()));
    auto ptr = builder.finish()->clone();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}
