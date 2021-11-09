// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../../..//external/catch/single_include/catch2/catch.hpp"

#include "../../../standardese/model/markup/paragraph.hpp"
#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/model/markup/emphasis.hpp"
#include "../../../standardese/model/markup/code.hpp"
#include "../../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../../standardese/output_generator/html/html_generator.hpp"
#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"

using namespace standardese::model::markup;
using standardese::output_generator::xml::xml_generator;
using standardese::output_generator::html::html_generator;
using standardese::output_generator::markdown::markdown_generator;

TEST_CASE("paragraph", "[markup]")
{
    auto html = R"(<p id="standardese-foo">a<em>b</em><code><em>c</em>d</code></p>
)";
    auto xml =
        R"(<paragraph id="foo">a<emphasis>b</emphasis><code><emphasis>c</emphasis>d</code></paragraph>
)";
    auto md = R"(a*b*`d`
)";

    paragraph builder;
    builder.add_child(text("a"));
    builder.add_child(emphasis(text("b")));
    builder.add_child(code(emphasis(text("c")), text("d")));
    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
}
*/
