// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/markup/paragraph.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/emphasis.hpp"
#include "../../../include/standardese/output/markup/code.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("paragraph", "[markup]")
{
    auto html = R"(<p id="standardese-foo">a<em>b</em><code><em>c</em>d</code></p>
)";
    auto xml =
        R"(<paragraph id="foo">a<emphasis>b</emphasis><code><emphasis>c</emphasis>d</code></paragraph>
)";
    auto md = R"(a*b*`d`
)";

    paragraph::builder builder(block_id("foo"));
    builder.add_child(text::build("a"));
    builder.add_child(emphasis::build("b"));
    builder.add_child(
        code::builder().add_child(emphasis::build("c")).add_child(text::build("d")).finish());
    auto ptr = builder.finish()->clone();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}
