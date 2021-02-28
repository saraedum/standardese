// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../../include/standardese/output/markup/main_document.hpp"
#include "../../../include/standardese/output/markup/paragraph.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/markup/subdocument.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

template <typename T>
void test_main_sub_document(const char* name)
{
    auto html = R"(<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="utf-8">
<title>Hello World!</title>
</head>
<body>
<p>foo</p>
</body>
</html>
)";

    auto xml = R"(<?xml version="1.0" encoding="UTF-8"?>
<)" + std::string(name)
               + R"( output-name="my-file" title="Hello World!">
<paragraph>foo</paragraph>
</)" + name + ">\n";

    auto md = R"(foo
)";

    typename T::builder builder("Hello World!", "my-file");
    builder.add_child(paragraph::builder(block_id("")).add_child(text::build("foo")).finish());

    auto entity = builder.finish()->clone();
    auto doc    = static_cast<T*>(entity.get());
    REQUIRE(doc->output_name().name() == "my-file");
    REQUIRE(doc->output_name().needs_extension());
    REQUIRE(html_generator::render(*doc) == html);
    REQUIRE(xml_generator::render(*doc) == xml);
    REQUIRE(markdown_generator::render(*doc) == md);
}

TEST_CASE("main_document", "[markup]")
{
    test_main_sub_document<main_document>("main-document");
}

TEST_CASE("subdocument", "[markup]")
{
    test_main_sub_document<subdocument>("subdocument");
}
