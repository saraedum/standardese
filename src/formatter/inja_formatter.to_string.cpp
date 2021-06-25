// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>
#include <cppast/cpp_entity_kind.hpp>

#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

template<class> inline constexpr bool always_false_v = false;

// TODO: Use everywhere for better error messages.
std::string inja_formatter::impl::to_string(const nlohmann::json& data) const {
  return std::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;
    if constexpr (std::is_same_v<T, const cppast::cpp_entity*>) {
      return fmt::format("{} {}, i.e., {}", inja_formatter{{}}.kind(*entity), inja_formatter{{}}.name(*entity), nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, const cppast::cpp_type*>) {
      return fmt::format("type {}, i.e., {}", inja_formatter{{}}.name(*entity), nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, std::nullptr_t>) {
      return std::string{"null"};
    } else if constexpr (std::is_same_v<T, model::link_target>) {
      return fmt::format("link target {}", nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, model::module>) {
      return fmt::format("module {}", entity.name);
    } else if constexpr (std::is_same_v<T, const json::array_t*>) {
      return fmt::format("array {}", nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, json::boolean_t>) {
      return fmt::format("bool {}", nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, json::number_float_t>) {
      return fmt::format("number {}", nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, const json::object_t*>) {
      return fmt::format("object {}", nlohmann::to_string(data));
    } else if constexpr (std::is_same_v<T, const json::string_t*>) {
      return fmt::format("string {}", nlohmann::to_string(data));
    } else {
      static_assert(always_false_v<T>, "unsupported variant");
      return std::string{};
    }
  }, from_json(data));
}

}
