// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "../../../external/catch/single_include/catch2/catch.hpp"

#include <algorithm>

#include "../../../standardese/model/markup/paragraph.hpp"
#include "../../../standardese/model/markup/block_quote.hpp"
#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/output_generator/xml/xml_generator.hpp"
#include "../../../standardese/output_generator/html/html_generator.hpp"
#include "../../../standardese/output_generator/markdown/markdown_generator.hpp"

using namespace standardese::model::markup;
using standardese::output_generator::xml::xml_generator;
using standardese::output_generator::html::html_generator;
using standardese::output_generator::markdown::markdown_generator;

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

    block_quote builder;

    paragraph paragraph0{};
    paragraph0.add_child(text("some text"));
    builder.add_child(paragraph0);

    paragraph paragraph1{};
    paragraph1.add_child(text("some text"));
    builder.add_child(paragraph1);

    REQUIRE(html_generator::render(builder) == html);
    REQUIRE(xml_generator::render(builder) == xml);
    REQUIRE(markdown_generator::render(builder) == md);
}
*/
