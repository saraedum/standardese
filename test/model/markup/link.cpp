// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/html/html_generator.hpp"
#include "../../../include/standardese/output/markdown/markdown_generator.hpp"
#include "../../../include/standardese/model/markup/emphasis.hpp"
#include "../../../include/standardese/model/markup/link.hpp"
#include "../../../include/standardese/model/markup/text.hpp"
#include "../../../include/standardese/output/xml/xml_generator.hpp"

using namespace standardese::model::markup;
using standardese::output::xml::xml_generator;
using standardese::output::html::html_generator;
using standardese::output::markdown::markdown_generator;

TEST_CASE("external_link", "[markup]")
{
    external_link a(url("http://foonathan.net/"));
    a.add_child(emphasis(text("awesome")));
    a.add_child(text(" website"));

    REQUIRE(html_generator::render(a) == "<a href=\"http://foonathan.net/\"><em>awesome</em> website</a>");
    REQUIRE(
        xml_generator::render(a)
        == R"(<external-link url="http://foonathan.net/"><emphasis>awesome</emphasis> website</external-link>)");
    REQUIRE(markdown_generator::render(a) == R"*([*awesome* website](http://foonathan.net/)
)*");

    external_link b("title\"", url("foo/bar/< &>"));
    b.add_child(text("with title"));

    REQUIRE(html_generator::render(b)
            == "<a href=\"foo/bar/%3C%20&amp;%3E\" title=\"title&quot;\">with title</a>");
    REQUIRE(
        xml_generator::render(b)
        == R"(<external-link title="title&quot;" url="foo/bar/&lt; &amp;&gt;">with title</external-link>)");
    REQUIRE(markdown_generator::render(b)
            == "[with title](foo/bar/\\<%20&\\> \"title\\\"\")\n"); // MSVC doesn't like a raw
                                                                    // string here :(
}

TEST_CASE("documentation_link", "[markup]")
{
    // non existing link, but doesn't matter
    documentation_link builder2("", block_reference(output_name::from_name("doc2"),
                                                             block_id("p3")));
    builder2.add_child(text("link 3"));

    REQUIRE(html_generator::render(builder2) == R"(<a href="doc2.html#standardese-p3">link 3</a>)");
    REQUIRE(
        xml_generator::render(builder2)
        == R"(<documentation-link destination-document="doc2" destination-id="p3">link 3</documentation-link>)");
    REQUIRE(markdown_generator::render(builder2) == R"([link 3](doc2.md#standardese-p3)
)");

    // URL link
    auto ptr3 = documentation_link("");
    ptr3.add_child(text("link 4"));
    ptr3->resolve_destination(url("http://foonathan.net"));
    REQUIRE(html_generator::render(*ptr3->clone()) == R"(<a href="http://foonathan.net">link 4</a>)");
    REQUIRE(
        xml_generator::render(*ptr3->clone())
        == R"(<documentation-link destination-url="http://foonathan.net">link 4</documentation-link>)");
    REQUIRE(markdown_generator::render(*ptr3) == R"([link 4](http://foonathan.net)
)");
}
*/
