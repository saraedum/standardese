// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_type.hpp>
#include <cppast/forward.hpp>
#include <fmt/format.h>

#include <cppast/code_generator.hpp>
#include <cppast/cpp_type_alias.hpp>
#include <cppast/cpp_function.hpp>
#include <cppast/cpp_namespace.hpp>
#include <cppast/cpp_array_type.hpp>
#include <cppast/cpp_decltype_type.hpp>
#include <cppast/cpp_function_type.hpp>
#include <cppast/cpp_template.hpp>
#include <nlohmann/json.hpp>

#include "../../standardese/formatter/code_formatter.hpp"
#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/mixin/documentation.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/list.hpp"
#include "../../standardese/model/link_target.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

code_formatter::code_formatter_options::code_formatter_options() {}

code_formatter::code_formatter(code_formatter_options options) : options(options) {}

model::document code_formatter::build(const cppast::cpp_entity& entity, const model::mixin::documentation& context) const {
  return inja_formatter{options, context}.code(entity);
}

model::document code_formatter::build(const cppast::cpp_type& type, const model::mixin::documentation& context) const {
  return build(options.type_format, type, context);
}

model::document code_formatter::build(const cppast::cpp_template_argument& argument, const model::mixin::documentation& context) const {
  return build(options.template_argument_format, argument, context);
}

model::document code_formatter::build(const std::string& format, const cppast::cpp_entity& entity, const model::mixin::documentation& context) const {
  return inja_formatter{options, context}.code(format, entity);
}

model::document code_formatter::build(const std::string& format, const cppast::cpp_type& type, const model::mixin::documentation& context) const {
  inja_formatter inja{options, context};

  // TODO:
  throw std::logic_error("not implemented: code_formatter::build(format, type, context");
}

model::document code_formatter::build(const std::string& format, const cppast::cpp_template_argument& argument, const model::mixin::documentation& context) const {
  inja_formatter inja{options, context};

  // TODO
  throw std::logic_error("not implemented: code_formatter::build(format, template parameter, context");
}

}
