// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <sstream>
#include <boost/type_index.hpp>
#include <boost/algorithm/string.hpp>

#include "../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/tool/options.hpp"
#include "../util/logger.hpp"
#include "../util/tmp_file.hpp"

// TODO(0.6.0-beta): Test (probably not here). When parsing a Sphinx inventory from a missing file, a proper error is produced and not just "not in a supported format".

namespace standardese::test::tool {

using standardese::tool::options;

TEST_CASE("Parsing of Generic Command Line Options", "[tool]")
{
  auto logger = util::logger::throwing_logger();

  SECTION("--verbose Flag") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    SECTION("--verbose") {
      const char* argv[] = {"standardese", "--verbose"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(logstream.str().find("info") != std::string::npos);
    }

    SECTION("--verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(logstream.str().find("debug") != std::string::npos);
    }

    SECTION("--verbose --verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(logstream.str().find("trace") != std::string::npos);
    }

    SECTION("--verbose --verbose --verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(logstream.str().find("trace") != std::string::npos);
    }
  }
}

TEST_CASE("Parsing of Legacy --input.* Options", "[tool]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  SECTION("--input.source_ext") {
    const char* argv[] = {"standardese", "--input.source_ext", ".h", "--input.source_ext", ".hpp", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_ext") {
    const char* argv[] = {"standardese", "--input.blacklist_ext", ".md", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -iname '*.md' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_file") {
    const char* argv[] = {"standardese", "--input.blacklist_file", "generated.h", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' '(' -name 'generated.h' -type f ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dir") {
    const char* argv[] = {"standardese", "--input.blacklist_dir", "external", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES '!' '(' -name 'external' -type d -prune -true ')' -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dot_files true") {
    const char* argv[] = {"standardese", "--input.blacklist_dotfiles", "true", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dot_files=0") {
    const char* argv[] = {"standardese", "--input.blacklist_dotfiles=0", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')'` FILES") != std::string::npos);
  }

  SECTION("--input.force_blacklist=on") {
    const char* argv[] = {"standardese", "--input.force_blacklist=on", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES_AND_FILES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'`") != std::string::npos);
  }

  SECTION("--input.force_blacklist=off") {
    const char* argv[] = {"standardese", "--input.force_blacklist=off", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }


  SECTION("--input.blacklist_namespace") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.blacklist_namespace", "standardese::detail", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(options.transformer_options.exclude_pattern_options.excluded.size() == 1);
  }

  SECTION("--input.require_comment") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.require_comment", "true", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(options.transformer_options.synopsis_options.exclude_uncommented);
  }

  SECTION("--input.extract_private") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.extract_private", "1", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(!options.transformer_options.exclude_access_options.exclude_private);
  }

}

TEST_CASE("Parsing of Legacy --compilation.* Options", "[tool]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  SECTION("--compilation.macro_definition") {
    util::tmp_file header{"header.hpp", R"(
      #ifndef MACRO
      THIS_LINE_SHOULD_BE_IGNORED
      #endif
      )"};

    const char* argv[] = {"standardese", "--compilation.macro_definition", "MACRO", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    parser::cppast_parser parser{options.parser_options.cppast_options};
    parser.parse(header.path);

    CHECK(logstream.str() != "");
  }

  SECTION("--compilation.macro_undefinition") {
    util::tmp_file header{"header.hpp", R"(
      #ifdef MACRO
      THIS_LINE_SHOULD_BE_IGNORED
      #endif
      )"};

    const char* argv[] = {"standardese", "--compilation.macro_definition", "MACRO", "--compilation.macro_undefinition", "MACRO", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    parser::cppast_parser parser{options.parser_options.cppast_options};
    parser.parse(header.path);

    CHECK(logstream.str() != "");
  }
}

TEST_CASE("Parsing of Legacy --comment.* Options", "[tool]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  SECTION("--comment.external_doc") {
    const char* argv[] = {"standardese", "--comment.external_doc", R"(std=http://en.cppreference.com/mwiki/index.php?title=Special%3ASearch&search=$$)", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    REQUIRE(options.transformer_options.external_link_options.size() == 1);
    std::visit([&](const auto& option) {
      using T = std::decay_t<decltype(option)>;
      CAPTURE(boost::typeindex::type_id<T>().pretty_name());
      if constexpr (std::is_same_v<T, standardese::tool::transformers::transformer_options::external_legacy_options>) {
        REQUIRE(option.options.namspace == "std");
      } else {
        REQUIRE(false);
      }
    }, options.transformer_options.external_link_options[0]);
  }
}

TEST_CASE("Parsing of Legacy --output.* Options", "[tool]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  SECTION("--output.prefix") {
    const char* argv[] = {"standardese", "--output.prefix", "outdir", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CHECK(options.output_generator_options.output_directory == "outdir");
  }

  SECTION("--output.format") {
    SECTION("HTML Output") {
      const char* argv[] = {"standardese", "--output.format", "html", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::html);
    }

    SECTION("XML Output") {
      const char* argv[] = {"standardese", "--output.format", "xml", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::xml);
    }

    SECTION("MarkDown Output") {
      const char* argv[] = {"standardese", "--output.format", "commonmark", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::markdown);
      CHECK(options.output_generator_options.markdown_options.targets == output_generator::markdown::markdown_generator::markdown_generator_options::target_rendering::plain);
    }

    SECTION("MarkDown+HTML Output") {
      const char* argv[] = {"standardese", "--output.format", "commonmark_html", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::markdown);
      CHECK(options.output_generator_options.markdown_options.targets == output_generator::markdown::markdown_generator::markdown_generator_options::target_rendering::html);
    }

    SECTION("Text Output") {
      const char* argv[] = {"standardese", "--output.format", "text", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(logstream.str() != "");
      CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::text);
    }
  }

  SECTION("--output.link_extension") {
    const char* argv[] = {"standardese", "--output.link_extension", "html", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CAPTURE(options.transformer_options.entity_document_options.document_path);
    CHECK(boost::algorithm::ends_with(options.transformer_options.entity_document_options.document_path, ".html"));
  }

  SECTION("--output.link_prefix") {
    const char* argv[] = {"standardese", "--output.link_prefix", "/docs/", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(logstream.str() != "");
    CAPTURE(options.transformer_options.entity_document_options.document_path);
    CHECK(boost::algorithm::starts_with(options.transformer_options.entity_document_options.document_path, "/docs/"));
  }
}

TEST_CASE("Parsing of Parser Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("-I") {
    const char* argv[] = {"standardese", "-I", ".", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    const auto flags = cppast::detail::libclang_compile_config_access::flags(options.parser_options.cppast_options.clang_config);
    CAPTURE(flags);

    CHECK(std::find(begin(flags), end(flags), "-I.") != end(flags));
  }

  SECTION("--std") {
    const char* argv[] = {"standardese", "--std", "c++17", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    const auto flags = cppast::detail::libclang_compile_config_access::flags(options.parser_options.cppast_options.clang_config);
    CAPTURE(flags);

    CHECK(std::find(begin(flags), end(flags), "-std=c++17") != end(flags));
  }

  SECTION("--free-file-comments") {
    const char* argv[] = {"standardese", "--free-file-comments", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.parser_options.comment_parser_options.free_file_comments);
  }

  SECTION("-D") {
    SECTION("Without Value") {
      util::tmp_file header{"header.hpp", R"(
        #ifndef MACRO
        THIS_LINE_SHOULD_BE_IGNORED
        #endif
        )"};

      const char* argv[] = {"standardese", "-D", "MACRO", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      parser::cppast_parser parser{options.parser_options.cppast_options};
      parser.parse(header.path);
    }

    SECTION("With Value") {
      util::tmp_file header{"header.hpp", R"(
        #if MACRO==0
        #else
        THIS_LINE_SHOULD_BE_IGNORED
        #endif
        )"};

      const char* argv[] = {"standardese", "-D", "MACRO=0", "header.hpp"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      parser::cppast_parser parser{options.parser_options.cppast_options};
      parser.parse(header.path);
    }
  }

  SECTION("-U") {
    util::tmp_file header{"header.hpp", R"(
      #ifdef MACRO
      THIS_LINE_SHOULD_BE_IGNORED
      #endif
      )"};

    const char* argv[] = {"standardese", "-D", "MACRO", "-U", "MACRO", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    parser::cppast_parser parser{options.parser_options.cppast_options};
    parser.parse(header.path);
  }

  SECTION("Known Bug: Order of -D and -U is Ignored") {
    util::tmp_file header{"header.hpp", R"(
      #ifdef MACRO
      THIS_LINE_SHOULD_BE_IGNORED
      #endif
      )"};

    const char* argv[] = {"standardese", "-U", "MACRO", "-D", "MACRO", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    parser::cppast_parser parser{options.parser_options.cppast_options};
    parser.parse(header.path);
  }
}

TEST_CASE("Parsing of External Linking Options") {
  auto logger = util::logger::throwing_logger();

  SECTION("External Sphinx Documentation") {
    SECTION("Register Python 3 Documentation") {
      const char* argv[] = {"standardese", "--external", "sphinx:py:objects.inv=https://docs.python.org/3", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformer_options.external_link_options.size() == 1);
      std::visit([&](const auto& option) {
        using T = std::decay_t<decltype(option)>;
        CAPTURE(boost::typeindex::type_id<T>().pretty_name());
        if constexpr (std::is_same_v<T, standardese::tool::transformers::transformer_options::external_sphinx_options>) {
          REQUIRE(option.options.schema == "py");
        } else {
          REQUIRE(false);
        }
      }, options.transformer_options.external_link_options[0]);
    }

    SECTION("Register Python 2 and Python 3 Documentation") {
      const char* argv[] = {"standardese", "--external", "sphinx:py:objects.inv=https://docs.python.org/3", "--external", "sphinx:py2:objects2.inv=httsp://docs.python.org/2", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformer_options.external_link_options.size() == 2);
      std::visit([&](const auto& option) {
        using T = std::decay_t<decltype(option)>;
        CAPTURE(boost::typeindex::type_id<T>().pretty_name());
        if constexpr (std::is_same_v<T, standardese::tool::transformers::transformer_options::external_sphinx_options>) {
          REQUIRE(option.options.schema == "py");
        } else {
          REQUIRE(false);
        }
      }, options.transformer_options.external_link_options[0]);
      std::visit([&](const auto& option) {
        using T = std::decay_t<decltype(option)>;
        CAPTURE(boost::typeindex::type_id<T>().pretty_name());
        if constexpr (std::is_same_v<T, standardese::tool::transformers::transformer_options::external_sphinx_options>) {
          REQUIRE(option.options.schema == "py2");
        } else {
          REQUIRE(false);
        }
      }, options.transformer_options.external_link_options[1]);
    }
  }

  SECTION("External Doxygen Documentation") {
    SECTION("Register cppreference.com Documentation") {
      const char* argv[] = {"standardese", "--external", "doxygen:std:cppreference.xml=https://en.cppreference.com/w", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformer_options.external_link_options.size() == 1);
      std::visit([&](const auto& option) {
        using T = std::decay_t<decltype(option)>;
        CAPTURE(boost::typeindex::type_id<T>().pretty_name());
        if constexpr (std::is_same_v<T, standardese::tool::transformers::transformer_options::external_doxygen_options>) {
          REQUIRE(option.options.schema == "std");
        } else {
          REQUIRE(false);
        }
      }, options.transformer_options.external_link_options[0]);
    }
  }
}

TEST_CASE("Parsing of Composition Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("--exclude") {
    const char* argv[] = {"standardese", "--exclude", "^std::", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    REQUIRE(options.transformer_options.exclude_pattern_options.excluded.size() == 1);
    CHECK(std::regex_search("std::hash", options.transformer_options.exclude_pattern_options.excluded[0]));
  }

  SECTION("--exclude-uncommented,-X") {
    SECTION("-XXXX") {
      const char* argv[] = {"standardese", "-XXXX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(options.transformer_options.exclude_uncommented_options.exclude_file == transformer::exclude_uncommented_transformer::exclude_uncommented_transformer_options::mode::exclude);
    }
    SECTION("-XXX") {
      const char* argv[] = {"standardese", "-XXX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(options.transformer_options.exclude_uncommented_options.exclude_class == transformer::exclude_uncommented_transformer::exclude_uncommented_transformer_options::mode::exclude);
    }
    SECTION("-XX") {
      const char* argv[] = {"standardese", "-XX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(options.transformer_options.exclude_uncommented_options.exclude_class == transformer::exclude_uncommented_transformer::exclude_uncommented_transformer_options::mode::exclude_if_empty);
      CHECK(options.transformer_options.synopsis_options.exclude_uncommented);
    }
    SECTION("-X") {
      const char* argv[] = {"standardese", "-X", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CHECK(options.transformer_options.exclude_uncommented_options.exclude_class == transformer::exclude_uncommented_transformer::exclude_uncommented_transformer_options::mode::exclude_if_empty);
      CHECK(!options.transformer_options.synopsis_options.exclude_uncommented);
    }
  }

  SECTION("--private") {
    const char* argv[] = {"standardese", "--private", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(!options.transformer_options.exclude_access_options.exclude_private);
  }
}

TEST_CASE("Parsing of MarkDown Output Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("MarkDown Output is the Default") {
    const char* argv[] = {"standardese", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::markdown);
  }

  SECTION("--md") {
    const char* argv[] = {"standardese", "--md", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::markdown);
  }
}

TEST_CASE("Parsing of HTML Output Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("--html") {
    const char* argv[] = {"standardese", "--html", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::html);
  }
}

TEST_CASE("Parsing of XML Output Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("--xml") {
    const char* argv[] = {"standardese", "--xml", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::xml);
  }
}

TEST_CASE("Parsing of Plain Text Output Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("--text") {
    const char* argv[] = {"standardese", "--text", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.output_generator_options.primary_format == standardese::tool::output_generators::output_generators_options::output_format::text);
  }
}

}
