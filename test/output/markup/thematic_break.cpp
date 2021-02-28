// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../include/standardese/output/markup/thematic_break.hpp"
#include "../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;
using standardese::output::markup::thematic_break;

TEST_CASE("thematic_break", "[markup]")
{
    REQUIRE(html_generator::render(*thematic_break::build()) == "<hr />\n");
    REQUIRE(xml_generator::render(*thematic_break::build()->clone()) == "<thematic-break></thematic-break>\n");
    REQUIRE(markdown_generator::render(*thematic_break::build()) == "-----\n");
}
