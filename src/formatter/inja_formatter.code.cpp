// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_template_parameter.hpp>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/model/markup/paragraph.hpp"
#include "../../standardese/model/visitor/visit.hpp"

namespace standardese::formatter {

nlohmann::json inja_formatter::code_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return to_json(code(data));
    }

    logger::error(fmt::format("Template callback `code` not valid here. Cannot produce code for {}.", self->to_string(data)));
    return to_json(model::document::container());
  }, self->from_json(data));
}

nlohmann::json inja_formatter::code_callback(const nlohmann::json& format, const nlohmann::json& entity) const {
  return std::visit([&](auto&& format_string, auto&& cpp_entity) {
    using T = std::decay_t<decltype(format_string)>;
    using S = std::decay_t<decltype(cpp_entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      if constexpr (std::is_same_v<S, const cppast::cpp_entity&>) {
        return to_json(code(*format_string, *cpp_entity));
      } else if constexpr (std::is_same_v<S, const cppast::cpp_type&>) {
        return to_json(code(*format_string, *cpp_entity));
      } else if constexpr (std::is_same_v<S, const cppast::cpp_template_argument&>) {
        return to_json(code(*format_string, *cpp_entity));
      } else {
        logger::error(fmt::format("Template callback `code` not valid here. Cannot produce code for {}.", self->to_string(entity)));
      }
    } else {
      logger::error(fmt::format("Template callback `code` not valid here. Argument {} is not a format string.", self->to_string(format)));
    }
    return to_json(model::document::container());
  }, self->from_json(format), self->from_json(entity));
}

model::markup::paragraph inja_formatter::code(const std::string& text) const {
  return code(parse(text).paragraph());
}

model::markup::paragraph inja_formatter::code(const model::entity& entity) const {
  return model::visitor::visit([&](auto&& e) {
    using T = std::decay_t<decltype(e)>;
    if constexpr (std::is_same_v<T, model::markup::code>)
      return model::markup::paragraph{std::move(e)};
    else if constexpr (std::is_same_v<T, model::markup::text>)
      return model::markup::paragraph{model::markup::code{e}};
    else if constexpr (std::is_same_v<T, model::markup::paragraph>) {
      auto code = model::markup::paragraph{};
      for (auto&& child : e.children)
        for (auto&& converted : this->code(child).children)
          code.children.push_back(std::move(converted));
      return code;
    } else if constexpr (std::is_same_v<model::markup::link, T>) {
      auto link = e;
      link.children.clear();
      for (auto&& child : e.children)
        for (auto&& converted : this->code(child).children)
          link.children.push_back(std::move(converted));
      return model::markup::paragraph{link};
    } else {
      logger::error(fmt::format("Callback code() is not supported for this kind of entity: {}", md(e)));
      return model::markup::paragraph{};
    }
  }, entity);
}

}
