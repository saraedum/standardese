// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../external/catch/single_include/catch2/catch.hpp"

#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/transformer/create_uncommented_module_transformer.hpp"
#include "../../standardese/model/unordered_entities.hpp"

#include "../util/cpp_file.hpp"
#include "../util/logger.hpp"

namespace standardese::test::transformer {

using standardese::transformer::create_uncommented_module_transformer;

TEST_CASE("Modules are Created if Necessary", "[create_uncommented_module_transformer]") {
  auto logger = util::logger::throwing_logger();

  util::cpp_file header(R"(
    void f();
    void g();
    void h();
  )");

  auto f = model::cpp_entity_documentation(&header["f()"], header);
  f.module = "fg";

  auto g = model::cpp_entity_documentation(&header["g()"], header);
  g.module = "fg";

  auto h = model::cpp_entity_documentation(&header["h()"], header);
  h.module = "h";

  SECTION("Modules are Created if they are Missing") {
    auto entities = model::unordered_entities{f, g, h};

    const auto modules = create_uncommented_module_transformer{&entities}.transform();
    REQUIRE(modules.size() == 2);
  }

  SECTION("No Modules are Created if None are Missing") {
    auto entities = model::unordered_entities{f, g, h, model::module{"fg"}, model::module{"h"}};

    const auto modules = create_uncommented_module_transformer{&entities}.transform();

    REQUIRE(modules.size() == 0);
  }
}

}
