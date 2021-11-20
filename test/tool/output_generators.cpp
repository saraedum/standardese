// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem/directory.hpp>
#include <vector>
#include <string>

#include "../external/catch/single_include/catch2/catch.hpp"
#include "../../standardese/tool/output_generators.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/tool/output_generators.hpp"
#include "../util/tmp_dir.hpp"

namespace standardese::test::tool {

using standardese::tool::output_generators;

TEST_CASE("Generation of MarkDown Output", "[tool]") {
  SECTION("MarkDown Output is Emitted for each Document") {
    model::entity_set documents{
      model::document{"example", "webroot/example"},
    };

    util::tmp_dir output;

    output_generators::output_generators_options options;
    options.primary_format = output_generators::output_generators_options::output_format::markdown;
    options.output_directory = output.path;
    options.intersphinx_inventory = std::string{};
    options.doxygen_tagfile = std::string{};

    output_generators generators{options};

    generators.emit(documents);

    REQUIRE_THAT(output.filenames(), Catch::Matchers::Equals(std::vector<std::string>{"example.md"}));
  }
}

TEST_CASE("Generation of HTML Output", "[tool]") {
  //TODO(0.6.0-alpha)
}

TEST_CASE("Generation of Text Output", "[tool]") {
  //TODO(0.6.0-alpha)
}

TEST_CASE("Generation of XML Output", "[tool]") {
  SECTION("XML Output is Emitted for each Document") {
    model::entity_set documents{
      model::document{"example", "webroot/example"},
    };

    util::tmp_dir output;

    output_generators::output_generators_options options;
    options.primary_format = output_generators::output_generators_options::output_format::xml;
    options.output_directory = output.path;
    options.intersphinx_inventory = std::string{};
    options.doxygen_tagfile = std::string{};

    output_generators generators{options};

    generators.emit(documents);

    REQUIRE_THAT(output.filenames(), Catch::Matchers::Equals(std::vector<std::string>{"example.xml"}));
  }
}

TEST_CASE("Generation of an Intersphinx Inventory", "[tool]") {
  SECTION("An Intersphinx Inventory is Emitted") {
    model::entity_set documents{};

    util::tmp_dir output;

    output_generators::output_generators_options options;
    options.output_directory = output.path;
    options.intersphinx_inventory = "sphinx.inv";
    options.doxygen_tagfile = std::string{};

    output_generators generators{options};

    generators.emit(documents);

    REQUIRE_THAT(output.filenames(), Catch::Matchers::Equals(std::vector<std::string>{"sphinx.inv"}));
  }
}

TEST_CASE("Generation of a Doxygen Tagfile", "[tool]") {
  SECTION("A Doxygen Tagfile is Emitted") {
    model::entity_set documents{};

    util::tmp_dir output;

    output_generators::output_generators_options options;
    options.output_directory = output.path;
    options.intersphinx_inventory = std::string{};
    options.doxygen_tagfile = "doxygen.xml";

    output_generators generators{options};

    generators.emit(documents);

    REQUIRE_THAT(output.filenames(), Catch::Matchers::Equals(std::vector<std::string>{"doxygen.xml"}));
  }
}

}
