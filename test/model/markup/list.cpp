// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../../../external/catch/single_include/catch2/catch.hpp"

#include <algorithm>

#include "../../../standardese/model/markup/list.hpp"
#include "../../../standardese/model/markup/list_item.hpp"
#include "../../../standardese/model/markup/paragraph.hpp"
#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../../standardese/output_generator/html/html_generator.hpp"
#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"

using namespace standardese::model;
using standardese::output_generator::xml::xml_generator;
using standardese::output_generator::html::html_generator;
using standardese::output_generator::markdown::markdown_generator;

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

    list builder(
        false,
        list_item(paragraph(), paragraph()),
        list_item(paragraph(text("text"))));
        //term_description_item("", term(text("A term")), description(text("A description"))));

    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
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

    REQUIRE(false);
    list builder(
        true,
        list_item(paragraph(), paragraph()),
        list_item(paragraph(text("text"))));
    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
}
*/
