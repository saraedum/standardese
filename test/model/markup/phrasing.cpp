// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../../../external/catch/single_include/catch2/catch.hpp"

#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/model/markup/emphasis.hpp"
#include "../../../standardese/model/markup/soft_break.hpp"
#include "../../../standardese/model/markup/hard_break.hpp"
#include "../../../standardese/model/markup/strong_emphasis.hpp"
#include "../../../standardese/model/markup/code.hpp"
#include "../../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../../standardese/output_generator/html/html_generator.hpp"
#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"

using namespace standardese::model::markup;
using standardese::output_generator::xml::xml_generator;
using standardese::output_generator::html::html_generator;
using standardese::output_generator::markdown::markdown_generator;

TEST_CASE("text", "[markup]")
{
    auto a = text("Hello World!");
    REQUIRE(html_generator::render(a) == "Hello World!");
    REQUIRE(xml_generator::render(a) == html_generator::render(a));
    REQUIRE(markdown_generator::render(a) == "Hello World\\!\n");

    auto b = text("Hello\nWorld!");
    REQUIRE(html_generator::render(b) == "Hello\nWorld!");
    REQUIRE(xml_generator::render(b) == html_generator::render(b));
    REQUIRE(markdown_generator::render(b) == "Hello\nWorld\\!\n");

    auto c = text("<html>&\"'</html>");
    REQUIRE(html_generator::render(c) == "&lt;html&gt;&amp;&quot;&#x27;&lt;&#x2F;html&gt;");
    REQUIRE(xml_generator::render(c) == "&lt;html&gt;&amp;&quot;&apos;&lt;/html&gt;");
    REQUIRE(markdown_generator::render(c) == R"(\<html\>&"'\</html\>
)");
}

template <typename T>
void test_phrasing(const std::string& html, const std::string& xml, const std::string& markdown)
{
    auto tag_str = [](const std::string& tag, const char* content) {
        return "<" + tag + ">" + content + "</" + tag + ">";
    };

    auto markdownify_str
        = [](const std::string& tag, const char* content) { return tag + content + tag + "\n"; };

    auto a = T("foo");
    REQUIRE(html_generator::render(a) == tag_str(html, "foo"));
    REQUIRE(xml_generator::render(a) == tag_str(xml, "foo"));
    REQUIRE(markdown_generator::render(a) == markdownify_str(markdown, "foo"));

    a = T();
    a.add_child(text("foo"));
    a.add_child(text("bar"));

    REQUIRE(html_generator::render(a) == tag_str(html, "foobar"));
    REQUIRE(xml_generator::render(a) == tag_str(xml, "foobar"));
    REQUIRE(markdown_generator::render(a) == markdownify_str(markdown, "foobar"));

    a = T();
    a.add_child(emphasis("foo"));
    a.add_child(text(">bar"));

    REQUIRE(html_generator::render(a) == tag_str(html, "<em>foo</em>&gt;bar"));
    REQUIRE(xml_generator::render(a) == tag_str(xml, "<emphasis>foo</emphasis>&gt;bar"));
    if (!std::is_same<T, code>::value)
        REQUIRE(markdown_generator::render(a) == markdownify_str(markdown, "*foo*\\>bar"));
}

TEST_CASE("emphasis", "[markup]")
{
    test_phrasing<emphasis>("em", "emphasis", "*");
}

TEST_CASE("strong_emphasis", "[markup]")
{
    test_phrasing<strong_emphasis>("strong", "strong-emphasis", "**");
}

TEST_CASE("code", "[markup]")
{
    test_phrasing<code>("code", "code", "`");
}

TEST_CASE("soft_break", "[markup]")
{
    REQUIRE(html_generator::render(soft_break()) == "\n");
    REQUIRE(xml_generator::render(soft_break()) == "<soft-break></soft-break>\n");
    REQUIRE(markdown_generator::render(soft_break()) == " \n");
}

TEST_CASE("hard_break", "[markup]")
{
    REQUIRE(html_generator::render(hard_break()) == "<br/>\n");
    REQUIRE(xml_generator::render(hard_break()) == "<hard-break></hard-break>\n");
    REQUIRE(markdown_generator::render(hard_break()) == "  \n");
}
*/
