// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_type.hpp>
#include <cppast/cppast_fwd.hpp>
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

code_formatter::code_formatter(code_formatter_options options, const parser::cpp_context& context) : options(std::move(options)), cpp_context(std::move(context)) {}

model::markup::paragraph code_formatter::build(const cppast::cpp_entity& entity) const {
  inja_formatter inja{options, cpp_context};
  inja.data() = inja.to_json(&entity);
  return inja.code(inja.format(options.cpp_format));
}

model::markup::paragraph code_formatter::build(const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const {
  inja_formatter inja{options, cpp_context, &context};
  inja.data() = inja.to_json(&entity);
  return inja.code(inja.format(options.cpp_format));
}

model::markup::paragraph code_formatter::build(const cppast::cpp_type& type) const {
  return build(options.type_format, type);
}

model::markup::paragraph code_formatter::build(const cppast::cpp_type& type, const cppast::cpp_entity& context) const {
  return build(options.type_format, type, context);
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_entity& entity) const {
  inja_formatter inja{options, cpp_context};
  inja.data() = inja.to_json(&entity);
  return inja.code(inja.format(format));
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_entity& entity, const cppast::cpp_entity& context) const {
  inja_formatter inja{options, cpp_context, &context};
  inja.data() = inja.to_json(&entity);
  return inja.code(inja.format(format));
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_type& type) const {
  inja_formatter inja{options, cpp_context};
  inja.data() = inja.to_json(&type);
  return inja.code(inja.format(format));
}

model::markup::paragraph code_formatter::build(const std::string& format, const cppast::cpp_type& type, const cppast::cpp_entity& context) const {
  inja_formatter inja{options, cpp_context, &context};
  inja.data() = inja.to_json(&type);
  return inja.code(inja.format(format));
}

}
