// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_file.hpp>
#include <boost/filesystem/path.hpp>
#include <regex>
#include <fmt/format.h>

// TODO: Drop external/ from everything but test/. Instead explain how to grab
//       dependencies and set compiler flags.
#include <inja/exceptions.hpp>
#include <inja/inja.hpp>

#include "../../standardese/formatter/inja_formatter.hpp"

#include "../../standardese/parser/markdown_parser.hpp"
#include "../../standardese/model/entity.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/model/markup/text.hpp"
#include "../../standardese/model/module.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

namespace {

enum class name_modifiers {
  // TODO: Allow arguments such as without-namespace, with-namespace, relative-namespace, without-return-type, ...
  shoort = 0,
};

std::string render_name(const cppast::cpp_entity& entity, name_modifiers modifiers) {
  if (entity.kind() == cppast::cpp_file::kind()) {
    return boost::filesystem::path(entity.name()).filename().native();
  } else {
    return entity.name();
  }
}

}

inja_formatter::options::options() {}

struct inja_formatter::environment::impl {
  inja::Environment env;
  nlohmann::json data;
};

inja_formatter::environment::environment() : self(std::make_unique<impl>()) {
  self->env.add_callback("filename", 1, [](inja::Arguments& args) {
    return boost::filesystem::path(args.at(0)->get<std::string>()).filename().string();
  });
  self->env.add_callback("md_escape", 1, [](inja::Arguments& args) {
    return parser::markdown_parser::escape(args.at(0)->get<std::string>());
  });
  self->env.add_callback("sanitize_basename", 1, [](inja::Arguments& args) {
    static std::regex forbidden{R"((\W|_)+)"};
    return std::regex_replace(args.at(0)->get<std::string>(), forbidden, "_");
  });
}

inja_formatter::environment::environment(const model::cpp_entity_documentation& context) : environment() {
  const auto& entity = context.entity();

  self->data["name"] = render_name(entity, name_modifiers{});

  self->env.add_callback("name", [&entity](inja::Arguments& args) {
    name_modifiers modifiers;
    for (const auto& arg : args) {
      const std::string& modifier = arg->get<std::string>();
      if (modifier == "short") {
        modifiers = name_modifiers::shoort;
      } else {
        logger::warn(fmt::format("Ignoring unknown flag to `name` template `{}`.", modifier));
      }
    }

    return render_name(entity, modifiers);
  });

  self->env.add_callback("path", 0, [&entity](inja::Arguments&) {
    const auto* parent = &entity;
    while(parent->parent().has_value())
      parent = &parent->parent().value();
    return static_cast<const cppast::cpp_file&>(*parent).name();;
  });
}

inja_formatter::environment::environment(const model::link_target& target) : environment() {
}

inja_formatter::environment::environment(const model::module& context) : environment() {
  self->data["name"] = context.name;

  // TODO: Add a callback version of name that complains about any arguments given.
}

inja_formatter::environment::~environment() {}

inja_formatter::inja_formatter(struct options options) : options(options) {}

std::string inja_formatter::format(const std::string &format, const environment &context) const {
  try {
    return context.self->env.render(format, context.self->data);
  } catch(inja::ParserError& e) {
    logger::error(fmt::format("Faild to parse inja template `{}` error at [{}:{}]: {}", format, e.location.line, e.location.column, e.message));
    return "";
  } catch(inja::RenderError& e) {
    logger::error(fmt::format("Faild to render inja template `{}` error at [{}:{}]: {}", format, e.location.line, e.location.column, e.message));
    return "";
  }
}

model::document inja_formatter::build(const std::string &format, const environment &context) const {
  const std::string markdown = this->format(format, context);

  return parser::markdown_parser{}.parse(markdown);
}

}
