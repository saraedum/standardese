// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../include/standardese/model/heading.hpp"
#include "../../include/standardese/model/subheading.hpp"
#include "../../include/standardese/model/text.hpp"
#include "../../include/standardese/model/emphasis.hpp"
#include "../../include/standardese/output/xml/xml_generator.hpp"
#include "../../include/standardese/output/html/html_generator.hpp"
#include "../../include/standardese/output/markdown/markdown_generator.hpp"

using namespace standardese::model;
using standardese::output::xml::xml_generator;
using standardese::output::html::html_generator;
using standardese::output::markdown::markdown_generator;

TEST_CASE("heading", "[markup]")
{
    auto html = R"(<h4>A <em>heading</em>!</h4>
)";
    auto xml  = R"(<heading>A <emphasis>heading</emphasis>!</heading>
)";
    auto md   = R"(#### A *heading*\!
)";

    heading builder(block_id{});
    builder.add_child(text("A "));
    builder.add_child(emphasis(text("heading")));
    builder.add_child(text("!"));

    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
}

TEST_CASE("subheading", "[markup]")
{
    auto html = R"(<h5>A <em>subheading</em>!</h5>
)";
    auto xml  = R"(<subheading>A <emphasis>subheading</emphasis>!</subheading>
)";
    auto md   = R"(##### A *subheading*\!
)";

    subheading builder(block_id{});
    builder.add_child(text("A "));
    builder.add_child(emphasis(text("subheading")));
    builder.add_child(text("!"));

    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
}
*/
