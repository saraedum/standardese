// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../external/catch/single_include/catch2/catch.hpp"

#include "../util/unindent.hpp"
#include "../util/parsed_comments.hpp"
#include "../util/logger.hpp"

#include "../../standardese/parser/comment_parser.hpp"
#include "../../standardese/parser/parse_error.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

#include <cppast/cpp_entity_kind.hpp>
#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/forward.hpp>

#include <boost/algorithm/string/replace.hpp>

namespace standardese::test::parser {

using namespace standardese::parser;

using output_generator::xml::xml_generator;
using util::unindent;
using util::cpp_file;
using util::parsed_comments;

TEST_CASE("Markdown Markup", "[comment_parser]")
{
    auto logger = util::logger::throwing_logger();

    cpp_file header("void f();");

    SECTION("Markdown Inline Markup is Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            A brief which is not relevant for this test
    
            This line starts the details.
            `code`
            *emphasis with `code`*\
            **strong emphasis with _emphasis_**
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>A brief which is not relevant for this test</paragraph>
              </section>
              <section name="Details">
                <paragraph>This line starts the details.<soft-break />
                  <code>code</code>
                  <soft-break />
                  <emphasis>emphasis with <code>code</code>
                  </emphasis>
                </paragraph>
                <paragraph>
                  <strong>strong emphasis with <emphasis>emphasis</emphasis>
                  </strong>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Regular Markdown Links are Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            [external link](https://foonathan.net)
            [external link `with markup` and tooltip](https://standardese.foonathan.net/ "tooltip")
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>
                  <link href="https://foonathan.net">external link</link>
                  <soft-break />
                  <link title="tooltip" href="https://standardese.foonathan.net/">external link <code>with markup</code> and tooltip</link>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("The Target of an Internal Link is the Title")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            [internal link](<> "target<T>")
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>
                  <link target="target&lt;T>">internal link</link>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("The Target of an Internal Link is Implicitly the Text")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            [target<T>]()
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>
                  <link target="target&lt;T>">target&lt;T&gt;</link>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("The Target of an Internal Link can not be taken from the Actual Target since we still Support Standard MarkDown Links")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            [target](target<T>)
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>
                  <link href="target&lt;T>">target</link>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("The Target of an Internal Link can be Searched for with * and ?")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            [*target]()
            [?target]()
            [internal link](<> "*target")
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>
                  <link target="*target">target</link>
                  <soft-break />
                  <link target="?target">target</link>
                  <soft-break />
                  <link target="*target">internal link</link>
                </paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("HTML Markup is not Supported so we can Write vector<T> Without Escaping")
    {
        SECTION("Any Inline HTML is Treated as Text")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                This brief contains <i>some</i> HTML but it won't render as markup so we can write vector<T> without having to escape things.
                )");

            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>This brief contains &lt;i&gt;some&lt;/i&gt; HTML but it won’t render as markup so we can write vector&lt;T&gt; without having to escape things.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION("Any HTML Blocks are Treated as Text")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                <p>
                    This paragraph will be part of the brief. But <b>any</b> HTML <p>markup</p> in here will be escaped, even if it is <a><b><i>malformed</b></i>.
                    <p>Also this nested block will be part of the brief.</p>

                A line break ends the preceding block in Markdown. Even though the <p> is still open. So this will be the details and not the brief anymore.
                </p>
                )");

            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>&lt;p&gt;
                    This paragraph will be part of the brief. But &lt;b&gt;any&lt;/b&gt; HTML &lt;p&gt;markup&lt;/p&gt; in here will be escaped, even if it is &lt;a&gt;&lt;b&gt;&lt;i&gt;malformed&lt;/b&gt;&lt;/i&gt;.
                    &lt;p&gt;Also this nested block will be part of the brief.&lt;/p&gt;
                </paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>A line break ends the preceding block in Markdown. Even though the &lt;p&gt; is still open. So this will be the details and not the brief anymore.</paragraph>
                    <paragraph>&lt;/p&gt;
                </paragraph>
                  </section>
                </entity-documentation>
                )"));
        }

    }

    SECTION("Images are Supported")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
          ![The Standardese Logo](https://raw.githubusercontent.com/standardese/standardese/master/standardese-logo.svg "Standardese Logo")
          )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
          <?xml version="1.0"?>
          <entity-documentation name="f">
            <section name="Brief">
              <paragraph>
                <img src="https://raw.githubusercontent.com/standardese/standardese/master/standardese-logo.svg" title="Standardese Logo">The Standardese Logo</img>
              </paragraph>
            </section>
          </entity-documentation>
          )"));
    }

    SECTION("Markdown Quotes are Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            > This initial quote is not used for the brief but goes into details.
            >
            > Second line of the quote.
            
            > A second quote.
            > The second line of the second quote.
            )");
    
        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Details">
                <block-quote>
                  <paragraph>This initial quote is not used for the brief but goes into details.</paragraph>
                  <paragraph>Second line of the quote.</paragraph>
                </block-quote>
                <block-quote>
                  <paragraph>A second quote.<soft-break />The second line of the second quote.</paragraph>
                </block-quote>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Markdown Lists are Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            * This list.
            * is tight.
            
            List break.
            
            * An item with a paragraph.
            
              And another paragraph.
            
            * And a different item.
            
            List break.
            
            1. An
            2. ordered
            3. list
            
            List break.
            
            * A list
            
            * with another
              1. list
              2. inside
            
            * *great*
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Details">
                <unordered-list>
                  <list-item>
                    <paragraph>This list.</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>is tight.</paragraph>
                  </list-item>
                </unordered-list>
                <paragraph>List break.</paragraph>
                <unordered-list>
                  <list-item>
                    <paragraph>An item with a paragraph.</paragraph>
                    <paragraph>And another paragraph.</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>And a different item.</paragraph>
                  </list-item>
                </unordered-list>
                <paragraph>List break.</paragraph>
                <ordered-list>
                  <list-item>
                    <paragraph>An</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>ordered</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>list</paragraph>
                  </list-item>
                </ordered-list>
                <paragraph>List break.</paragraph>
                <unordered-list>
                  <list-item>
                    <paragraph>A list</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>with another</paragraph>
                    <ordered-list>
                      <list-item>
                        <paragraph>list</paragraph>
                      </list-item>
                      <list-item>
                        <paragraph>inside</paragraph>
                      </list-item>
                    </ordered-list>
                  </list-item>
                  <list-item>
                    <paragraph>
                      <emphasis>great</emphasis>
                    </paragraph>
                  </list-item>
                </unordered-list>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Markdown Code Blocks are Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            ```
            A code block starting a comment is not used for a brief but goes to the details.
            ```
            
            ```cpp
            A code block with info.
            ```
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Details">
                <code-block>A code block starting a comment is not used for a brief but goes to the details.
            </code-block>
                <code-block language="cpp">A code block with info.
            </code-block>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Markdown Headings are Preserved")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            # A heading starting a comment is not used for a brief but goes to the details.
            
            ## B
            
            ### C
            
            DDD
            ===
            
            EEE
            ---
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Details">
                <heading level="1">A heading starting a comment is not used for a brief but goes to the details.</heading>
                <heading level="2">B</heading>
                <heading level="3">C</heading>
                <heading level="1">DDD</heading>
                <heading level="2">EEE</heading>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Markdown Thematic Breaks are Supported")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            This line is used for the brief and does not show up in the details.
            
            A paragraph.
            
            ---
            
            A completely different paragraph.
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>This line is used for the brief and does not show up in the details.</paragraph>
              </section>
              <section name="Details">
                <paragraph>A paragraph.</paragraph>
                <thematic-break />
                <paragraph>A completely different paragraph.</paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Markdown Paragraphs are (Mostly) Preserved")
    {
        SECTION("Empty Lines Delimit Paragraphs")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                The first line ends in punctuation so it defines the brief and does not show up in the details.
                The first paragraph.
                Second line of the first paragraph.
                
                The second paragraph.
                
                The third paragraph.
                Second line of the third paragraph.
                )");

            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>The first line ends in punctuation so it defines the brief and does not show up in the details.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>The first paragraph.<soft-break />Second line of the first paragraph.</paragraph>
                    <paragraph>The second paragraph.</paragraph>
                    <paragraph>The third paragraph.<soft-break />Second line of the third paragraph.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }

        SECTION("Break Paragraphs Between Brief and Details")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                Brief
                sentence
                split
                into
                multiple!
                Not brief.
                )");

            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>Brief<soft-break />sentence<soft-break />split<soft-break />into<soft-break />multiple!</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>Not brief.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }

        SECTION("Preserve Complicated Parapgraphs in Details")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                Implicit brief.
                This is not part of the brief anymore because the previous line ends with a full stop.
                
                Still details.
                Even still details.
                
                > Also in quote.
                
                ```
                Or code.
                ```
                
                * Or
                * List
                )");

            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>Implicit brief.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>This is not part of the brief anymore because the previous line ends with a full stop.</paragraph>
                    <paragraph>Still details.<soft-break />Even still details.</paragraph>
                    <block-quote>
                      <paragraph>Also in quote.</paragraph>
                    </block-quote>
                    <code-block>Or code.
                </code-block>
                    <unordered-list>
                      <list-item>
                        <paragraph>Or</paragraph>
                      </list-item>
                      <list-item>
                        <paragraph>List</paragraph>
                      </list-item>
                    </unordered-list>
                  </section>
                </entity-documentation>
                )"));
        }
    }
}


TEST_CASE("Standardese Specific Markup Rules", "[comment_parser]")
{
    cpp_file header("void f();");

    SECTION("Explicit Sections Spanning Multiple Lines")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            \brief Explicit brief.
            Still explicit brief.
             
            \details Explicit details.
             
            Still details.
             
            \effects Explicit effects.
            Still effects.
             
            Details again.
             
            \requires
             
            \returns
            Explicit returns.
            \returns Different returns.
            A backslash at the end of the line causes a hard line break and ends this section.\
            Details again.
            \notes Explicit notes.
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>Explicit brief.<soft-break />Still explicit brief.</paragraph>
              </section>
              <section name="Details">
                <paragraph>Explicit details.</paragraph>
                <paragraph>Still details.</paragraph>
                <paragraph>Details again.</paragraph>
                <paragraph>Details again.</paragraph>
              </section>
              <section name="Effects">
                <paragraph>Explicit effects.<soft-break />Still effects.</paragraph>
              </section>
              <section name="Requires" />
              <section name="Return values">
                <paragraph>Explicit returns.</paragraph>
              </section>
              <section name="Return values">
                <paragraph>Different returns.<soft-break />A backslash at the end of the line causes a hard line break and ends this section.</paragraph>
              </section>
              <section name="Notes">
                <paragraph>Explicit notes.</paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Section Commands Must Start a Line")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            \details Ignore \effects not starting at beginning.
            Technically, this is because our cmark extension only recognizes such commands when they start a new block, and blocks can only start at the beginning of a line.
            \synopsis Ignore all lines starting with a command.
            But please include me.
            
            > \effects In block quote is ignored.
            
            * \effects In list is ignored.
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f" synopsis="Ignore all lines starting with a command.">
              <section name="Details">
                <paragraph>Ignore \effects not starting at beginning.<soft-break />Technically, this is because our cmark extension only recognizes such commands when they start a new block, and blocks can only start at the beginning of a line.</paragraph>
                <paragraph>But please include me.</paragraph>
                <block-quote>
                  <paragraph>\effects In block quote is ignored.</paragraph>
                </block-quote>
                <unordered-list>
                  <list-item>
                    <paragraph>\effects In list is ignored.</paragraph>
                  </list-item>
                </unordered-list>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Sections can be Terminated Explicitly")
    {
        const util::cpp_file header("void f(int foo);");

        const auto parsed = parsed_comments(header).add(header["f"], R"(
            \effects The first line of the effects section.
            The second line of the effects.
            
            This is part of effects and not details because there is an explicit end command.
            
            This is part of effects and not details because there is an explicit end command.
            
            \end
            
            This is the first line of the details.
            
            \returns This is the only line of the returns section.
            \end
            
            \notes This is the first line of the notes section.
            
            Another line of the notes section.
            \end
            This is the second line of the details.
            
            \requires This is the first line of the requires section.
            
            This is the second
            and this is the third line of the requires.\end
            This is the fourth line of the requires section because an \end must be at the start of the line.
            \end
            This is the third line of the details.
            
            \param foo A parameter.
            Going on.
            
            * Still
            * going
            * on
            
            \end
            
            This is the fourth line of the details.
            )");

        CHECK(xml_generator::render(parsed["f"]) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Effects">
                <paragraph>The first line of the effects section.<soft-break />The second line of the effects.</paragraph>
                <paragraph>This is part of effects and not details because there is an explicit end command.</paragraph>
                <paragraph>This is part of effects and not details because there is an explicit end command.</paragraph>
              </section>
              <section name="Details">
                <paragraph>This is the first line of the details.</paragraph>
                <paragraph>This is the second line of the details.</paragraph>
                <paragraph>This is the third line of the details.</paragraph>
                <paragraph>This is the fourth line of the details.</paragraph>
              </section>
              <section name="Return values">
                <paragraph>This is the only line of the returns section.</paragraph>
              </section>
              <section name="Notes">
                <paragraph>This is the first line of the notes section.</paragraph>
                <paragraph>Another line of the notes section.</paragraph>
              </section>
              <section name="Requires">
                <paragraph>This is the first line of the requires section.</paragraph>
                <paragraph>This is the second<soft-break />and this is the third line of the requires.\end<soft-break />This is the fourth line of the requires section because an \end must be at the start of the line.</paragraph>
              </section>
            </entity-documentation>
            )"));

        CHECK(xml_generator::render(parsed["f.foo"]) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="foo">
              <section name="Brief">
                <paragraph>A parameter.</paragraph>
              </section>
              <section name="Details">
                <paragraph>Going on.</paragraph>
                <unordered-list>
                  <list-item>
                    <paragraph>Still</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>going</paragraph>
                  </list-item>
                  <list-item>
                    <paragraph>on</paragraph>
                  </list-item>
                </unordered-list>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION("Key Value Sections (Are No Longer Supported)")
    {
        // Up to 0.6.4, we allowed to write sections of the form:
        // \returns 0 - describe 0
        // 1 - describe 1
        // This output did not work well in the Markdown output and also can be
        // sufficiently well emulated with Markdown.
        // Also this allowed to use the shorthand [link] instead of [link]() on
        // the left hand side of the `-`. This is not supported anymore.
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            \returns 0 - Value 0.
            1-Value 1.
            It requires extra long description.
            \returns Default returns.
            \notes This terminates.
            
            \effects -> really weird
            
            \see [foo] - Optional description.
            \see [bar]-
            
            This terminates.
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Return values">
                <paragraph>0 - Value 0.<soft-break />1-Value 1.<soft-break />It requires extra long description.</paragraph>
              </section>
              <section name="Return values">
                <paragraph>Default returns.</paragraph>
              </section>
              <section name="Notes">
                <paragraph>This terminates.</paragraph>
              </section>
              <section name="Effects">
                <paragraph>-&gt; really weird</paragraph>
              </section>
              <section name="See also">
                <paragraph>[foo] - Optional description.</paragraph>
              </section>
              <section name="See also">
                <paragraph>[bar]-</paragraph>
              </section>
              <section name="Details">
                <paragraph>This terminates.</paragraph>
              </section>
            </entity-documentation>
            )"));
    }
}

TEST_CASE("Standardese Commands", "[comment_parser]")
{
    cpp_file header("void f();");

    SECTION(R"(\verbatim Signals Inclusion Without any Markup)")
    {
        const auto parsed = parsed_comments(header).add(header["f"], R"(
            \verbatim This is *verbatim*.\end
            And \verbatim `this` as well.

            \effects But this is a section.

            With \verbatim VERBATIM<*>\end

            \end

            1. A list
               - A child list
                 \verbatim #105\end
            )");

        CHECK(xml_generator::render(parsed) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>This is *verbatim*.<soft-break />And `this` as well.</paragraph>
              </section>
              <section name="Effects">
                <paragraph>But this is a section.</paragraph>
                <paragraph>With VERBATIM&lt;*&gt;</paragraph>
              </section>
              <section name="Details">
                <ordered-list>
                  <list-item>
                    <paragraph>A list</paragraph>
                    <unordered-list>
                      <list-item>
                        <paragraph>A child list<soft-break />#105</paragraph>
                      </list-item>
                    </unordered-list>
                  </list-item>
                </ordered-list>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION(R"(\exclude Overrides Which Parts of a Node Should be Included)")
    {
        SECTION("The Default is not to Exclude Anything")
        {
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().exclude_mode == model::exclude_mode::include);
        }
        SECTION("The Command Works Without Arguments or With Special Keywords")
        {
            CHECK(parsed_comments(header).add(header["f"], R"(\exclude)").as_documentation().exclude_mode == model::exclude_mode::exclude);
            CHECK(parsed_comments(header).add(header["f"], R"(\exclude return)").as_documentation().exclude_mode == model::exclude_mode::exclude_return_type);
            CHECK(parsed_comments(header).add(header["f"], R"(\exclude target)").as_documentation().exclude_mode == model::exclude_mode::exclude_target);
            
        }
        SECTION("With other Keywords, the command is treated as text.")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(\exclude foo)");
            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\exclude foo</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }

        SECTION("Multiple Excludes are not Allowed")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \exclude
                \exclude
                )"), parse_error);
        }
    }

    SECTION(R"(\unique_name Command Overrides the Name of an Entity for Linking)")
    {
        SECTION("The Default is not to set the Unique Name")
        {
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().id == "");
        }
        SECTION("A Unique Name can be Set Explicitly")
        {
            CHECK(parsed_comments(header).add(header["f"], R"(\unique_name name)").as_documentation().id == "name");
        }
        SECTION("Malformed Unique Names are Treated as Text")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(\unique_name a b c)");
            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\unique_name a b c</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION("There can only be one Unique Name")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
              \unique_name a
              \unique_name b
              )"), parse_error);
        }
    }

    SECTION(R"(\output_name Changes the Base Name of an Output File)")
    {
        cpp_file header{"void f();"};

        SECTION("Output Name can be set on a File")
        {
            parsed_comments(header).add(header, R"(
                \file
                \output_name name
                )");
        }
        SECTION("On Non-Files it Affects the Output Name if Genereting a Document for this one Entity")
        {
            parsed_comments(header).add(header["f"], R"(
                \output_name f-function
                )");
        }
        SECTION("The Default is not to set the Output Name")
        {
            CHECK(parsed_comments(header).add(header, R"(\file)").as_documentation().output_name == "");
        }
        SECTION("The Output Name can be set Explicitly")
        {
            CHECK(parsed_comments(header).add(header, R"(
                \file
                \output_name name
                )").as_documentation().output_name == "name");
        }
        SECTION("A Malformed Output Name is Treated as Text")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(\output_name a b c)");
            CHECK(xml_generator::render(parsed) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\output_name a b c</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION("There can only be one Output Name")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header, R"(
                \file
                \output_name a
                \output_name b
                )"), parse_error);
        }
    }

    SECTION(R"(\synopsis Completely Overrides the Synopsis of an Entity)")
    {
        SECTION("The Default is not to set the Synopsis Override")
        {
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().synopsis == type_safe::nullopt);
        }
        SECTION("Synopsis can be set to any String")
        {
            CHECK(parsed_comments(header).add(header["f"], R"(\synopsis void f())").as_documentation().synopsis == "void f()");
            CHECK(parsed_comments(header).add(header["f"], R"(\synopsis)").as_documentation().synopsis == "");
        }
        SECTION("The Synopsis must be Unique")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \synopsis a
                \synopsis b
                )"), parse_error);
        }
    }

    SECTION(R"(\group and \output_section Groups Members Together)")
    {
        SECTION("The Default is not to set Group and Output Section")
        {
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().group == type_safe::nullopt);
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().output_section == type_safe::nullopt);
        }
        SECTION("Group can be set Explicitly")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \group name
            )").as_documentation();
            CHECK(parsed.group == "name");
            CHECK(parsed.output_section == "name");
        }
        SECTION("Group can be set Explicitly Without Defining an Output Section")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \group -name
            )").as_documentation();
            CHECK(parsed.group == "name");
            CHECK(!parsed.output_section.has_value());
        }
        SECTION("Group can be set Explicitly With a Specific Heading")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \group name Group Heading
            )").as_documentation();
            CHECK(parsed.group == "name");
            CHECK(parsed.output_section == "Group Heading");
        }
        SECTION("Output Section can be set Explicitly")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \output_section name
            )").as_documentation();
            CHECK(!parsed.group.has_value());
            CHECK(parsed.output_section == "name");
        }
        SECTION("Malformed Commands are Treated as Text")
        {
            CHECK(xml_generator::render(parsed_comments(header).add(header["f"], R"(
                \group
                \group
                )")) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\group<soft-break />\group</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION("Only one of Group and Output Section can be set Exactly Once")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \group a
                \group a
                )"), parse_error);
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \output_section
                \output_section
                )"), parse_error);
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \group a
                \output_section a
                )"), parse_error);
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \output_section a
                \output_section a
                )"), parse_error);
        }
        SECTION("Group Command Syntax can be Configured")
        {
            // This syntax allows us to have a group name including whitespace
            // so we drop the heading completely. (And there is also no way to
            // use a separate heading with this approach unfortunately.)
            parser::comment_parser_options options('\\', {"group|=== ((?:.(?!==))+)() =="});

            const auto parsed = parsed_comments(header).add(header["f"], R"(
                == Method Group ==
                This is the brief of this method group.
                )", options).as_documentation();

            CHECK(parsed.group == "Method Group");
            CHECK(parsed.output_section == "Method Group");
        }
    }

    SECTION(R"(\module Groups Entities Across File Boundaries)")
    {
        SECTION("The Default is not to Assign any Module")
        {
            CHECK(parsed_comments(header).add(header["f"], "a comment").as_documentation().module == type_safe::nullopt);
        }
        SECTION("The Module can be set Explicitly")
        {
            CHECK(parsed_comments(header).add(header["f"], R"(\module name)").as_documentation().module == "name");
        }
        SECTION("A Malformed Command is Treated as Text")
        {
            CHECK(xml_generator::render(parsed_comments(header).add(header["f"], R"(
                \module
                )")) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\module</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed_comments(header).add(header["f"], R"(
                \module a b c
                )")) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\module a b c</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION("The Module must be Unique")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \module a
                \module b
                )"), parse_error);
        }
        SECTION(R"(A \module Command at the File Level Provides Documentation for the Module Itself)")
        {
            const cpp_file header;
            const auto parsed = parsed_comments(header).add(header, R"(
                \module M
                Description of the module M
                )").as_module();
            CHECK(parsed.name == "M");
        }
    }

    SECTION(R"(The \file Command Provides Description for an Entire Header File)")
    {
        SECTION(R"(A \file Command can Appear at the Top-Level, i.e., not Associated to an Entity)")
        {
            const cpp_file header;

            CHECK_NOTHROW(parsed_comments(header).add(header, R"(\file description")"));
        }
        SECTION(R"(A \file Command cannot Appear next to an Entity such as a Method)")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(\file description)"), parse_error);
        }
    }

    SECTION(R"(The \entity Command Provides Documentation for Another Entity)")
    {
        util::cpp_file header(R"(
            void f();
            )");

        SECTION("The Entity can be set Explicitly")
        {
            const auto parsed = parsed_comments(header).add(header, R"(
                \entity f()
                This documentation is for the function f()
                )");

            CHECK_NOTHROW(parsed["f"]);
        }
        SECTION("The Implicit Entity Cannot be Replaced")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header["f"], R"(
                \entity f()
                This documentation is for the function f() and not for the entity which is next to.
                )"), parse_error);
        }
        SECTION("The Entity must be Unique and Non-Empty")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header, R"(
                \entity f
                \entity f
                )"), parse_error);
            CHECK_THROWS_AS(parsed_comments(header).add(header, R"(
                \entity a
                \file
                )"), parse_error);
        }
        SECTION("The Entity must Exist")
        {
            CHECK_THROWS_AS(parsed_comments(header).add(header, R"(
                \entity g
                )"), parse_error);
        }
    }

    SECTION(R"(Unknown Commands are Reproduced Verbatim)")
    {
        CHECK(xml_generator::render(parsed_comments(header).add(header["f"], R"(
            \invalid comment
            )")) == unindent(R"(
            <?xml version="1.0"?>
            <entity-documentation name="f">
              <section name="Brief">
                <paragraph>\invalid comment</paragraph>
              </section>
            </entity-documentation>
            )"));
    }

    SECTION(R"(The \param Command Describes a Parameter of a Function or Method)")
    {
        const util::cpp_file header("void f(int arg, int brg);");

        SECTION("A Malformed Command is Reproduced as Text")
        {
            CHECK(xml_generator::render(parsed_comments(header).add(header["f"], R"(
                \param
                )")) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>\param</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \param Follows the Usual Parsing Rules: the First Line is Brief, the Rest are Details)")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                This is the brief of the method we are documenting.
                These are the details of the method we are documenting.
                \param arg This is the brief of the parameter arg.
                These are the details of the parameter arg.
                )");

            CHECK(xml_generator::render(parsed["f"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Brief">
                    <paragraph>This is the brief of the method we are documenting.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the method we are documenting.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["f.arg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="arg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter arg.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the parameter arg.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \param ends Implicitly when Another Section Begins")")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \param arg This is the brief of the parameter arg.
                \returns This is the description of the return value.
                )");

            CHECK(xml_generator::render(parsed["f"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Return values">
                    <paragraph>This is the description of the return value.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["f.arg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="arg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter arg.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \param ends Implicitly when a Paragraph Ends")")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \param arg This is the brief of the parameter arg.
                These are the details of the parameter a.

                These are the details of the method.
                )");

            CHECK(xml_generator::render(parsed["f"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Details">
                    <paragraph>These are the details of the method.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["f.arg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="arg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter arg.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the parameter a.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \param can be Ended Explicitly with an \end)")
        {
            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \param arg This is the brief of the parameter arg.
                \end
                These are the details of the method.
                )");

            CHECK(xml_generator::render(parsed["f"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="f">
                  <section name="Details">
                    <paragraph>These are the details of the method.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["f.arg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="arg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter arg.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(The \param Command can be Changed Through Configuration)")
        {
            parser::comment_parser_options options('\\', {"param|=:param ([^:]+):"});

            const auto parsed = parsed_comments(header).add(header["f"], R"(
                \param arg This is the brief of the parameter arg.
                :param brg: This is the brief of the parameter brg.
                )", options);

            CHECK(xml_generator::render(parsed["f.arg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="arg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter arg.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["f.brg"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="brg">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter brg.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
    }

    SECTION(R"(The \tparam Command Describes a Template Parameter of a Class or Function)")
    {
        const util::cpp_file header("template <typename T> void swap(T& a, T& b);");

        SECTION("A Malformed Command is Reproduced as Text")
        {
            CHECK(xml_generator::render(parsed_comments(header).add(header["swap"], R"(
                \tparam
                )")) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="swap">
                  <section name="Brief">
                    <paragraph>\tparam</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \tparam Follows the Usual Parsing Rules: the First Line is Brief, the Rest are Details)")
        {
            const auto parsed = parsed_comments(header).add(header["swap"], R"(
                This is the brief of the method we are documenting.
                These are the details of the method we are documenting.
                \tparam T This is the brief of the parameter T.
                These are the details of the parameter T.
                )");

            CHECK(xml_generator::render(parsed["swap"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="swap">
                  <section name="Brief">
                    <paragraph>This is the brief of the method we are documenting.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the method we are documenting.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["swap.T"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="T">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter T.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the parameter T.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \tparam ends Implicitly when Another Section Begins")")
        {
            const auto parsed = parsed_comments(header).add(header["swap"], R"(
                \tparam T This is the brief of the parameter T.
                \returns This is the description of the return value.
                )");

            CHECK(xml_generator::render(parsed["swap"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="swap">
                  <section name="Return values">
                    <paragraph>This is the description of the return value.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["swap.T"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="T">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter T.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \tparam ends Implicitly when a Paragraph Ends")")
        {
            const auto parsed = parsed_comments(header).add(header["swap"], R"(
                \tparam T This is the brief of the parameter T.
                These are the details of the parameter T.

                These are the details of the method.
                )");

            CHECK(xml_generator::render(parsed["swap"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="swap">
                  <section name="Details">
                    <paragraph>These are the details of the method.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["swap.T"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="T">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter T.</paragraph>
                  </section>
                  <section name="Details">
                    <paragraph>These are the details of the parameter T.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(A \tparam can be Ended Explicitly with an \end)")
        {
            const auto parsed = parsed_comments(header).add(header["swap"], R"(
                \tparam T This is the brief of the parameter T.
                \end
                These are the details of the method.
                )");

            CHECK(xml_generator::render(parsed["swap"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="swap">
                  <section name="Details">
                    <paragraph>These are the details of the method.</paragraph>
                  </section>
                </entity-documentation>
                )"));
            CHECK(xml_generator::render(parsed["swap.T"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="T">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter T.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
        SECTION(R"(The \tparam Command can be Changed Through Configuration)")
        {
            parser::comment_parser_options options('\\', {"tparam|=:tparam ([^:]+):"});

            const auto parsed = parsed_comments(header).add(header["swap"], R"(
                :tparam T: This is the brief of the parameter T.
                )", options);

            CHECK(xml_generator::render(parsed["swap.T"]) == unindent(R"(
                <?xml version="1.0"?>
                <entity-documentation name="T">
                  <section name="Brief">
                    <paragraph>This is the brief of the parameter T.</paragraph>
                  </section>
                </entity-documentation>
                )"));
        }
    }

    SECTION(R"(The \base Command Describes a Base Class)")
    {
        // TODO: Implement me.
    }

    SECTION("Section Commands")
    {
        // TODO: We should test all the section commands here.
    }
}

}
