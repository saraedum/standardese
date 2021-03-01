// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include <algorithm>

#include "../../../include/standardese/output/markup/paragraph.hpp"
#include "../../../include/standardese/output/markup/block_quote.hpp"
#include "../../../include/standardese/output/markup/text.hpp"
#include "../../../include/standardese/output/generator/xml/xml_generator.hpp"
#include "../../../include/standardese/output/generator/html/html_generator.hpp"
#include "../../../include/standardese/output/generator/markdown/markdown_generator.hpp"

using namespace standardese::output::markup;
using standardese::output::generator::xml::xml_generator;
using standardese::output::generator::html::html_generator;
using standardese::output::generator::markdown::markdown_generator;

TEST_CASE("block_quote", "[markup]")
{
    auto html = R"(<blockquote id="standardese-foo">
<p>some text</p>
<p>some more text</p>
</blockquote>
)";

    auto xml = R"(<block-quote id="foo">
<paragraph>some text</paragraph>
<paragraph>some more text</paragraph>
</block-quote>
)";

    auto md = std::string(R"(> some text
>$
> some more text
)");
    std::replace(md.begin(), md.end(), '$', ' ');

    block_quote::builder builder(block_id("foo"));
    builder.add_child(paragraph::builder().add_child(text::build("some text")).finish());
    builder.add_child(paragraph::builder().add_child(text::build("some more text")).finish());

    auto ptr = builder.finish()->clone();
    REQUIRE(html_generator::render(*ptr) == html);
    REQUIRE(xml_generator::render(*ptr) == xml);
    REQUIRE(markdown_generator::render(*ptr) == md);
}
