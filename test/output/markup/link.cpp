// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/markup/external_link.hpp"
#include "../../../include/standardese/output/markup/emphasis.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/documentation_link.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("external_link", "[markup]")
{
    external_link::builder a(url("http://foonathan.net/"));
    a.add_child(emphasis::build("awesome"));
    a.add_child(text::build(" website"));

    auto a_ptr = a.finish()->clone();
    REQUIRE(html_generator::render(*a_ptr) == "<a href=\"http://foonathan.net/\"><em>awesome</em> website</a>");
    REQUIRE(
        xml_generator::render(*a_ptr)
        == R"(<external-link url="http://foonathan.net/"><emphasis>awesome</emphasis> website</external-link>)");
    REQUIRE(markdown_generator::render(*a_ptr) == R"*([*awesome* website](http://foonathan.net/)
)*");

    external_link::builder b("title\"", url("foo/bar/< &>"));
    b.add_child(text::build("with title"));

    auto b_ptr = b.finish()->clone();
    REQUIRE(html_generator::render(*b_ptr)
            == "<a href=\"foo/bar/%3C%20&amp;%3E\" title=\"title&quot;\">with title</a>");
    REQUIRE(
        xml_generator::render(*b_ptr)
        == R"(<external-link title="title&quot;" url="foo/bar/&lt; &amp;&gt;">with title</external-link>)");
    REQUIRE(markdown_generator::render(*b_ptr)
            == "[with title](foo/bar/\\<%20&\\> \"title\\\"\")\n"); // MSVC doesn't like a raw
                                                                    // string here :(
}

TEST_CASE("documentation_link", "[markup]")
{
    // non existing link, but doesn't matter
    documentation_link::builder builder2("", block_reference(output_name::from_name("doc2"),
                                                             block_id("p3")));
    builder2.add_child(text::build("link 3"));

    auto ptr2 = builder2.finish()->clone();
    REQUIRE(html_generator::render(*ptr2) == R"(<a href="doc2.html#standardese-p3">link 3</a>)");
    REQUIRE(
        xml_generator::render(*ptr2)
        == R"(<documentation-link destination-document="doc2" destination-id="p3">link 3</documentation-link>)");
    REQUIRE(markdown_generator::render(*ptr2) == R"([link 3](doc2.md#standardese-p3)
)");

    // URL link
    auto ptr3 = documentation_link::builder("").add_child(text::build("link 4")).finish();
    ptr3->resolve_destination(url("http://foonathan.net"));
    REQUIRE(html_generator::render(*ptr3->clone()) == R"(<a href="http://foonathan.net">link 4</a>)");
    REQUIRE(
        xml_generator::render(*ptr3->clone())
        == R"(<documentation-link destination-url="http://foonathan.net">link 4</documentation-link>)");
    REQUIRE(markdown_generator::render(*ptr3) == R"([link 4](http://foonathan.net)
)");
}
