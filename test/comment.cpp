// Copyright (C) 2016 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <standardese/comment.hpp>

#include <catch.hpp>

#include <standardese/md_blocks.hpp>
#include <standardese/md_inlines.hpp>
#include <standardese/parser.hpp>

using namespace standardese;

std::string get_text(const md_paragraph& paragraph)
{
    std::string result;
    for (auto& child : paragraph)
        if (child.get_entity_type() == md_entity::text_t)
            result += dynamic_cast<const md_text&>(child).get_string();
        else if (child.get_entity_type() == md_entity::soft_break_t)
            result += '\n';
    return result;
}

TEST_CASE("md_comment", "[doc]")
{
    parser p;

    SECTION("comment styles")
    {
        auto source = R"(/// C++ style.
            
            //! C++ exclamation.
            
            /** C style. */
            
            // ignored
            /* ignored as well */
            
            /*! C exclamation.
            */
            
            /** C style
              * multiline.
              */
              
            /** C style
            /// C++ multiline. */
            
            /// Multiple
            /// C++
            /** and C
            */
            /// style.
            
            //< End line style.
        )";

        auto comments = detail::read_comments(source);
        REQUIRE(comments.size() == 8);

        REQUIRE(comments[0].content == "C++ style.");
        REQUIRE(comments[0].count_lines == 1u);
        REQUIRE(comments[0].end_line == 1u);

        REQUIRE(comments[1].content == "C++ exclamation.");
        REQUIRE(comments[1].count_lines == 1u);
        REQUIRE(comments[1].end_line == 3u);

        REQUIRE(comments[2].content == "C style.");
        REQUIRE(comments[2].count_lines == 1u);
        REQUIRE(comments[2].end_line == 5u);

        REQUIRE(comments[3].content == "C exclamation.");
        REQUIRE(comments[3].count_lines == 2u);
        REQUIRE(comments[3].end_line == 11u);

        REQUIRE(comments[4].content == "C style\nmultiline.");
        REQUIRE(comments[4].count_lines == 3u);
        REQUIRE(comments[4].end_line == 15u);

        REQUIRE(comments[5].content == "C style\n/// C++ multiline.");
        REQUIRE(comments[5].count_lines == 2u);
        REQUIRE(comments[5].end_line == 18u);

        REQUIRE(comments[6].content == "Multiple\nC++\nand C\nstyle.");
        REQUIRE(comments[6].count_lines == 5u);
        REQUIRE(comments[6].end_line == 24u);
        
        REQUIRE(comments[6].content == "End line style.");
        REQUIRE(comments[6].count_lines == 1u);
        REQUIRE(comments[6].end_line == 26u);
    }
    SECTION("simple parsing")
    {
        auto comment = md_comment::parse(p, "", R"(Hello World.)");
        auto count   = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);

            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            REQUIRE(get_text(paragraph) == std::string("Hello World."));
            REQUIRE(paragraph.get_section_type() == section_type::brief);
            ++count;
        }
        REQUIRE(count == 1u);
    }
    SECTION("multiple sections explicit")
    {
        auto comment = md_comment::parse(p, "", R"(
\brief A

\details B
C
)");

        auto count = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);

            if (get_text(paragraph) == "A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::brief);
            }
            else if (get_text(paragraph) == "B\nC")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::details);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
    SECTION("multiple sections implicit")
    {
        auto comment = md_comment::parse(p, "", R"(
A
 
B
C /// C
)");

        auto count = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);

            if (get_text(paragraph) == "A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::brief);
            }
            else if (get_text(paragraph) == "B\nC /// C")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::details);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
    SECTION("cherry pick other commands")
    {
        auto comment = md_comment::parse(p, "", R"(
\effects A A
A A

\returns B B

\error_conditions C C
)");

        auto count = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO(get_text(paragraph));

            if (get_text(paragraph) == " A A\nA A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::effects);
            }
            else if (get_text(paragraph) == " B B")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::returns);
            }
            else if (get_text(paragraph) == " C C")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::error_conditions);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 3u);
    }
    SECTION("merging")
    {
        auto comment = md_comment::parse(p, "", R"(
\effects A

\effects A

\requires B
)");

        auto count = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);
            INFO('"' + get_text(paragraph) + '"');

            if (get_text(paragraph) == " A A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::effects);
            }
            else if (get_text(paragraph) == " B")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::requires);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
    SECTION("implicit paragraph")
    {
        p.get_comment_config().set_implicit_paragraph(true);

        auto comment = md_comment::parse(p, "", R"(
\effects A
\effects A
\requires B
)");

        auto count = 0u;
        for (auto& child : *comment)
        {
            REQUIRE(child.get_entity_type() == md_entity::paragraph_t);
            auto& paragraph = dynamic_cast<const md_paragraph&>(child);

            if (get_text(paragraph) == " A A")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::effects);
            }
            else if (get_text(paragraph) == " B")
            {
                ++count;
                REQUIRE(paragraph.get_section_type() == section_type::requires);
            }
            else
                REQUIRE(false);
        }
        REQUIRE(count == 2u);
    }
}
