// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORMATTER_INJA_FORMATTER_IMPL_HPP_INCLUDED
#define STANDARDESE_FORMATTER_INJA_FORMATTER_IMPL_HPP_INCLUDED

#include <inja/exceptions.hpp>
#include <inja/inja.hpp>

#include <type_safe/optional.hpp>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/forward.hpp"
#include "../../standardese/model/module.hpp"

namespace standardese::formatter {

struct inja_formatter::impl {
  struct savepoint {
    savepoint(impl& self) : self(self), backup(self.data) {}
    ~savepoint() {
      self.data = std::move(backup);
    }

    impl& self;
    nlohmann::json backup;
  };

  explicit impl(inja_formatter_options options);

  static std::string href_schema;

  using json = nlohmann::json;
  using variant = std::variant<std::nullptr_t, const cppast::cpp_entity*, const cppast::cpp_type*, model::link_target, model::module, const json::array_t*, json::boolean_t, json::number_float_t, const json::object_t*, const json::string_t*>;

  static variant from_json(const nlohmann::json&);

  inja_formatter_options options;
  inja::Environment env;
  json data;
  type_safe::optional_ref<const model::mixin::documentation> context;
};

}

#endif

