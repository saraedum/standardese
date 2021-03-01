// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/emphasis.hpp"
#include "../../../include/standardese/output/markup/verbatim.hpp"
#include "../../../include/standardese/output/markup/soft_break.hpp"
#include "../../../include/standardese/output/markup/hard_break.hpp"
#include "../../../include/standardese/output/markup/strong_emphasis.hpp"
#include "../../../include/standardese/output/markup/verbatim.hpp"
#include "../../../include/standardese/output/markup/code.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("text", "[markup]")
{
    auto a = text::build("Hello World!")->clone();
    REQUIRE(html_generator::render(*a) == "Hello World!");
    REQUIRE(xml_generator::render(*a) == html_generator::render(*a));
    REQUIRE(markdown_generator::render(*a) == "Hello World\\!\n");

    auto b = text::build("Hello\nWorld!");
    REQUIRE(html_generator::render(*b) == "Hello\nWorld!");
    REQUIRE(xml_generator::render(*b) == html_generator::render(*b));
    REQUIRE(markdown_generator::render(*b) == "Hello\nWorld\\!\n");

    auto c = text::build("<html>&\"'</html>");
    REQUIRE(html_generator::render(*c) == "&lt;html&gt;&amp;&quot;&#x27;&lt;&#x2F;html&gt;");
    REQUIRE(xml_generator::render(*c) == "&lt;html&gt;&amp;&quot;&apos;&lt;/html&gt;");
    REQUIRE(markdown_generator::render(*c) == R"(\<html\>&"'\</html\>
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

    auto a = T::build("foo")->clone();
    REQUIRE(html_generator::render(*a) == tag_str(html, "foo"));
    REQUIRE(xml_generator::render(*a) == tag_str(xml, "foo"));
    REQUIRE(markdown_generator::render(*a) == markdownify_str(markdown, "foo"));

    typename T::builder b;
    b.add_child(text::build("foo"));
    b.add_child(text::build("bar"));

    auto b_ptr = b.finish()->clone();
    REQUIRE(html_generator::render(*b_ptr) == tag_str(html, "foobar"));
    REQUIRE(xml_generator::render(*b_ptr) == tag_str(xml, "foobar"));
    REQUIRE(markdown_generator::render(*b_ptr) == markdownify_str(markdown, "foobar"));

    typename T::builder c;
    c.add_child(emphasis::build("foo"));
    c.add_child(text::build(">bar"));

    auto c_ptr = c.finish()->clone();
    REQUIRE(html_generator::render(*c_ptr) == tag_str(html, "<em>foo</em>&gt;bar"));
    REQUIRE(xml_generator::render(*c_ptr) == tag_str(xml, "<emphasis>foo</emphasis>&gt;bar"));
    if (!std::is_same<T, code>::value)
        REQUIRE(markdown_generator::render(*c_ptr) == markdownify_str(markdown, "*foo*\\>bar"));
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

TEST_CASE("verbatim", "[markup]")
{
    auto v = verbatim::build("*Hello* <i>World</i>!");
    REQUIRE(html_generator::render(*v) == "*Hello* <i>World</i>!");
    REQUIRE(xml_generator::render(*v) == "<verbatim>*Hello* &lt;i&gt;World&lt;/i&gt;!</verbatim>");
    REQUIRE(markdown_generator::render(*v) == "*Hello* <i>World</i>!\n");
}

TEST_CASE("soft_break", "[markup]")
{
    REQUIRE(html_generator::render(*soft_break::build()) == "\n");
    REQUIRE(xml_generator::render(*soft_break::build()) == "<soft-break></soft-break>\n");
    REQUIRE(markdown_generator::render(*soft_break::build()) == " \n");
}

TEST_CASE("hard_break", "[markup]")
{
    REQUIRE(html_generator::render(*hard_break::build()) == "<br/>\n");
    REQUIRE(xml_generator::render(*hard_break::build()) == "<hard-break></hard-break>\n");
    REQUIRE(markdown_generator::render(*hard_break::build()) == "  \n");
}
