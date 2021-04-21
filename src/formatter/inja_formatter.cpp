// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <boost/filesystem/path.hpp>

// TODO: Drop external/ from everything but test/. Instead explain how to grab
//       dependencies and set compiler flags.
#include <inja/inja.hpp>

#include "../../standardese/formatter/inja_formatter.hpp"

#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/markup/text.hpp"
#include "../../standardese/model/module.hpp"

// TODO: Handle inja exceptions.

namespace standardese::formatter {

inja_formatter::options::options() {}

inja_formatter::options::options(const std::string& format)
  : file_format(format),
    function_format(format),
    type_alias_format(format),
    class_format(format),
    member_variable_format(format) {}

inja_formatter::inja_formatter(struct options options) : options(options) {}

model::entity inja_formatter::format(const cppast::cpp_entity& entity) const {
  inja::Environment env;
  nlohmann::json data;

  const auto file = [&]() -> const cppast::cpp_file& {
   const auto* parent = &entity;
   while(parent->parent().has_value())
     parent = &parent->parent().value();
   return static_cast<const cppast::cpp_file&>(*parent);
  };

  data["name"] = entity.name();

  env.add_callback("path", 0, [&](inja::Arguments&) {
    return boost::filesystem::path(file().name()).string();
  });
  env.add_callback("filename", 1, [](inja::Arguments& args) {
    return boost::filesystem::path(args.at(0)->get<std::string>()).filename().string();
  });

  // TODO: Parses inja output.
  return model::markup::text(env.render(options.file_format, data));
}

model::entity inja_formatter::format(const model::module& module) const {
  // TODO: Actually use inja.
  return model::markup::text(module.name);
}

}
