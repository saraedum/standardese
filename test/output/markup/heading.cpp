// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/markup/heading.hpp"
#include "../../../include/standardese/output/markup/subheading.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/emphasis.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("heading", "[markup]")
{
    auto html = R"(<h4>A <em>heading</em>!</h4>
)";
    auto xml  = R"(<heading>A <emphasis>heading</emphasis>!</heading>
)";
    auto md   = R"(#### A *heading*\!
)";

    heading::builder builder(block_id{});
    builder.add_child(text::build("A "));
    builder.add_child(emphasis::build("heading"));
    builder.add_child(text::build("!"));

    auto ptr = builder.finish();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr->clone()) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}

TEST_CASE("subheading", "[markup]")
{
    auto html = R"(<h5>A <em>subheading</em>!</h5>
)";
    auto xml  = R"(<subheading>A <emphasis>subheading</emphasis>!</subheading>
)";
    auto md   = R"(##### A *subheading*\!
)";

    subheading::builder builder(block_id{});
    builder.add_child(text::build("A "));
    builder.add_child(emphasis::build("subheading"));
    builder.add_child(text::build("!"));

    auto ptr = builder.finish();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr->clone()) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}
