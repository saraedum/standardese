// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"

#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::format_callback(const nlohmann::json& format, const nlohmann::json& data) const {
  auto finally = impl::savepoint(const_cast<inja_formatter::impl&>(*this->self));
  const_cast<inja_formatter*>(this)->data() = data;

  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return this->format(*entity);
    }

    logger::error(fmt::format("Template callback `format` not valid here. Cannot format with template {}.", nlohmann::to_string(format)));
    return std::string{};
  }, self->from_json(format));

}

std::string inja_formatter::format_callback(const nlohmann::json& format) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return this->format(*entity);
    }

    logger::error(fmt::format("Template callback `format` not valid here. Cannot format with template {}.", nlohmann::to_string(format)));
    return std::string{};
  }, self->from_json(format));
}

std::string inja_formatter::format(const std::string &format) const {
  try {
    std::string rendered = self->env.render(format, self->data);

    logger::trace([&]() { return fmt::format("Rendered template `{}` with `{}` as `{}`.", format, nlohmann::to_string(data()), rendered); });

    return rendered;
  } catch(inja::ParserError& e) {
    logger::error(fmt::format("Faild to parse inja template `{}` error at [{}:{}]: {}", format, e.location.line, e.location.column, e.message));
    return std::string{};
  } catch(inja::RenderError& e) {
    logger::error(fmt::format("Faild to render inja template `{}` with data `{}` error at [{}:{}]: {}", format, self->data.dump(), e.location.line, e.location.column, e.message));
    return std::string{};
  }
}

}
