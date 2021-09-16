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

code_formatter::code_formatter(code_formatter_options options, parser::cpp_context context) : options(std::move(options)), cpp_context(std::move(context)) {}

model::markup::paragraph code_formatter::build(const cppast::cpp_entity& entity) const {
  return inja_formatter{options, cpp_context}.code(entity);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const {
  return inja_formatter{options, cpp_context, context}.code(entity);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_type& type) const {
  return build(options.type_format, type);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_type& type, const cppast::cpp_entity& context) const {
  return build(options.type_format, type, context);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_template_argument& argument) const {
  return build(options.template_argument_format, argument);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_template_argument& argument, const cppast::cpp_entity& context) const {
  return build(options.template_argument_format, argument, context);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_entity& entity) const {
  return inja_formatter{options, cpp_context}.code(format, entity);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const {
  return inja_formatter{options, cpp_context, context}.code(format, entity);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_type& type) const {
  return inja_formatter{options, cpp_context}.code(format, type);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_type& type, const cppast::cpp_entity& context) const {
  return inja_formatter{options, cpp_context, context}.code(format, type);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_template_argument& argument) const {
  return inja_formatter{options, cpp_context}.code(format, argument);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_template_argument& argument, const cppast::cpp_entity& context) const {
  return inja_formatter{options, cpp_context, context}.code(format, argument);
}

}
