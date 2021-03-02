// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <sstream>

#include "../../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/tool/options.hpp"
#include "../util/logger.hpp"

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

      CAPTURE(logstream.str());
      CHECK(logstream.str().find("info") != std::string::npos);
    }

    SECTION("--verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CAPTURE(logstream.str());
      CHECK(logstream.str().find("debug") != std::string::npos);
    }

    SECTION("--verbose --verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CAPTURE(logstream.str());
      CHECK(logstream.str().find("trace") != std::string::npos);
    }

    SECTION("--verbose --verbose --verbose --verbose") {
      const char* argv[] = {"standardese", "-v", "-v", "-v", "-v"};
      options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      CAPTURE(logstream.str());
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

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_ext") {
    const char* argv[] = {"standardese", "--input.blacklist_ext", ".md", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -iname '*.md' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_file") {
    const char* argv[] = {"standardese", "--input.blacklist_file", "generated.h", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' '(' -name 'generated.h' -type f ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dir") {
    const char* argv[] = {"standardese", "--input.blacklist_dir", "external", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES '!' '(' -name 'external' -type d -prune -true ')' -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dot_files true") {
    const char* argv[] = {"standardese", "--input.blacklist_dotfiles", "true", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }

  SECTION("--input.blacklist_dot_files=0") {
    const char* argv[] = {"standardese", "--input.blacklist_dotfiles=0", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')'` FILES") != std::string::npos);
  }

  SECTION("--input.force_blacklist=on") {
    const char* argv[] = {"standardese", "--input.force_blacklist=on", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES_AND_FILES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'`") != std::string::npos);
  }

  SECTION("--input.force_blacklist=off") {
    const char* argv[] = {"standardese", "--input.force_blacklist=off", "directory"};
    options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(logstream.str().find("standardese OPTIONS `find DIRECTORIES -type f '(' -iname '*.h' -o -iname '*.hpp' -o -iname '*.h++' -o -iname '*.hxx' ')' '!' -name '.*'` FILES") != std::string::npos);
  }
  

  SECTION("--input.blacklist_namespace") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.blacklist_namespace", "standardese::detail", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.transformation_options.exclude_pattern_options.excluded.size() == 1);
  }

  SECTION("--input.require_comment") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.require_comment", "true", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.transformation_options.synopsis_options.exclude_uncommented);
  }

  SECTION("--input.extract_private") {
    auto logstream = std::stringstream();
    auto logger = util::logger::capturing_logger(logstream);

    const char* argv[] = {"standardese", "--input.extract_private", "1", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(!options.transformation_options.exclude_access_options.exclude_private);
  }
}

TEST_CASE("Parsing of Legacy --output.* Options", "[tool]") {
  auto logstream = std::stringstream();
  auto logger = util::logger::capturing_logger(logstream);

  SECTION("--output.prefix") {
    const char* argv[] = {"standardese", "--output.prefix", "outdir", "header.hpp"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CAPTURE(logstream.str());
    CHECK(options.output_generator_options.output_directory == "outdir");
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

    CHECK(std::find(begin(flags), end(flags), "-std=c++1z") != end(flags));
  }

  SECTION("--free-file-comments") {
    const char* argv[] = {"standardese", "--free-file-comments", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(options.parser_options.comment_parser_options.free_file_comments);
  }
}

TEST_CASE("Parsing of External Linking Options") {
  auto logger = util::logger::throwing_logger();

  SECTION("External Sphinx Documentation") {
    SECTION("Register Python 3 Documentation") { 
      const char* argv[] = {"standardese", "--external", "sphinx:py:objects.inv=https://docs.python.org/3", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformation_options.external_link_options.size() == 1);
      REQUIRE(options.transformation_options.external_link_options[0].kind == standardese::tool::transformations::options::external_link_options::kind::SPHINX);
      REQUIRE(options.transformation_options.external_link_options[0].schema == "py");
    }

    SECTION("Register Python 2 and Python 3 Documentation") { 
      const char* argv[] = {"standardese", "--external", "sphinx:py:objects.inv=https://docs.python.org/3", "--external", "sphinx:py2:objects2.inv=httsp://docs.python.org/2", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformation_options.external_link_options.size() == 2);
      REQUIRE(options.transformation_options.external_link_options[0].schema == "py");
      REQUIRE(options.transformation_options.external_link_options[1].schema == "py2");
    }
  }

  SECTION("External Doxygen Documentation") {
    SECTION("Register cppreference.com Documentation") {
      const char* argv[] = {"standardese", "--external", "doxygen:std:cppreference.xml=https://en.cppreference.com/w", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

      REQUIRE(options.transformation_options.external_link_options.size() == 1);
      REQUIRE(options.transformation_options.external_link_options[0].kind == standardese::tool::transformations::options::external_link_options::kind::DOXYGEN);
      REQUIRE(options.transformation_options.external_link_options[0].schema == "std");
    }
  }
}

TEST_CASE("Parsing of Composition Options", "[tool]") {
  auto logger = util::logger::throwing_logger();

  SECTION("--exclude") {
    const char* argv[] = {"standardese", "--exclude", "^std::", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    REQUIRE(options.transformation_options.exclude_pattern_options.excluded.size() == 1);
    CHECK(std::regex_search("std::hash", options.transformation_options.exclude_pattern_options.excluded[0]));
  }

  SECTION("--exclude-uncommented,-X") {
    SECTION("-XXXX") {
      const char* argv[] = {"standardese", "-XXXX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});
  
      CHECK(options.transformation_options.exclude_uncommented_options.exclude_file == transformation::exclude_uncommented_transformation::options::mode::exclude);
    }
    SECTION("-XXX") {
      const char* argv[] = {"standardese", "-XXX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});
  
      CHECK(options.transformation_options.exclude_uncommented_options.exclude_class == transformation::exclude_uncommented_transformation::options::mode::exclude);
    }
    SECTION("-XX") {
      const char* argv[] = {"standardese", "-XX", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});
  
      CHECK(options.transformation_options.exclude_uncommented_options.exclude_class == transformation::exclude_uncommented_transformation::options::mode::exclude_if_empty);
      CHECK(options.transformation_options.synopsis_options.exclude_uncommented);
    }
    SECTION("-X") {
      const char* argv[] = {"standardese", "-X", "header.h"};
      auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});
  
      CHECK(options.transformation_options.exclude_uncommented_options.exclude_class == transformation::exclude_uncommented_transformation::options::mode::exclude_if_empty);
      CHECK(!options.transformation_options.synopsis_options.exclude_uncommented);
    }
  }

  SECTION("--private") {
    const char* argv[] = {"standardese", "--private", "header.h"};
    auto options = options::parse(sizeof(argv)/sizeof(*argv), argv, {});

    CHECK(!options.transformation_options.exclude_access_options.exclude_private);
  }
}

}
