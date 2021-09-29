// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/cpp_type.hpp>
#include <cppast/cpp_template.hpp>
#include <cstdint>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <stdexcept>

#include "../../standardese/formatter/inja_formatter.hpp"
#include "../../standardese/model/visitor/visit.hpp"
#include "../../standardese/logger.hpp"

#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

namespace {

/// Return the pointer `ptr` as a string to embed it in JSON.
/// The inja formatter uses JSON internally to keep track of its state.
/// However, JSON cannot store references to arbitrary objects in the
/// standardese library. To store such references we must therefore serialize
/// them first. Here we serialize such a reference, simply by writing out its
/// address as a hexadecimal string.
/// Naturally, this is a hack and can lead to segfaults if the object at that
/// address disappears for some reason. This could probably be made a little
/// less hacky by serializing a cppast::cpp_entity_ref or a
/// cppast::cpp_entity_id.
template <typename T>
std::string to_string(const T* ptr) {
  if (ptr == nullptr)
    throw std::invalid_argument("Cannot serialize nullptr to JSON.");
  std::ostringstream adr;
  adr << static_cast<const void*>(ptr);
  return adr.str();
}

/// Return a pointer to the object referenced by `value`.
/// This is the inverse of `to_string()`.
template <typename T>
const T* from_string(const nlohmann::json::string_t* value) {
  std::istringstream adr(*value);
  std::uintptr_t parsed;

  adr >> std::hex >> parsed;

  if (!adr.eof())
    throw std::invalid_argument(fmt::format(R"(Cannot deserialize pointer "{}". Not a hexadecimal address.)", *value));
  if (parsed == 0)
    throw std::invalid_argument(fmt::format(R"(Cannot deserialize pontir "{}". Deserialized to null pointer.)", *value));

  return reinterpret_cast<const T*>(parsed);
}

}

nlohmann::json inja_formatter::to_json(const model::entity& entity) const {
  nlohmann::json json;

  // TODO: This is quite hacky. In particular how we only render md sometimes. Can we render md when a document contains anything that is not supported in plain MarkDown?

  model::visitor::visit([&](auto&& entity) {
    using T = std::decay_t<decltype(entity)>;

    if constexpr (std::is_base_of_v<model::mixin::documentation, T>) {
      if constexpr (std::is_same_v<T, model::module>) {
        json["standardese"] = {
          {"kind", "module"},
          {"name", entity.name },
        };
      } else if constexpr (std::is_same_v<T, model::group_documentation>) {
        json["standardese"] = {
          {"kind", "group"},
          {"entities", nlohmann::json::array()},
        };
        for (auto& member : entity.entities)
          json["standardese"]["entities"].push_back(to_json(member.entity()));
      } else if constexpr (std::is_same_v<T, model::cpp_entity_documentation>) {
        // TODO: Are we losing \synopsis here?
        json.merge_patch(to_json(entity.entity()));
      }

      json["standardese"]["group"] = entity.group.has_value() ? entity.group.value() : "";
      json["standardese"]["output_section"] = entity.output_section.has_value() ? entity.output_section.value() : "";
      json["standardese"]["synopsis"] = entity.synopsis.has_value() ? entity.synopsis.value() : "";
    } else {
      if constexpr (std::is_same_v<T, model::document>) {
        json["standardese"] = {
          {"kind", "document"},
          {"name", entity.name},
          {"path", entity.path},
        };
      }

      json["md"] = md(entity);
    }
  }, entity);

  return json;
}

nlohmann::json inja_formatter::to_json(const cppast::cpp_entity& entity) const {
  nlohmann::json json;

  json["standardese"] = {
    {"kind", "cpp_entity"},
    {"value", to_string(&entity) },
  };

  return json;
}

nlohmann::json inja_formatter::to_json(const cppast::cpp_type& type) const {
  nlohmann::json json = {
    {"standardese", {
      {"kind", "cpp_type"},
      {"value", to_string(&type) }
    }
  }};

  return json;
}

inja_formatter::impl::variant inja_formatter::impl::from_json(const nlohmann::json& value) {
  if (value.is_null())
    return nullptr;
  if (value.is_object()) {
    const auto standardese = value.find("standardese");
    if (standardese != value.end() && standardese->is_object()) {
      const auto kind = standardese->find("kind");
      if (kind != standardese->end() && kind->is_string()) {
        // When standardese.kind is set, this is an internal standardese object encoded as JSON.
        const auto& kind_ref = kind->get_ref<const nlohmann::json::string_t&>();
        if (kind_ref == "cpp_entity") {
          const auto v = standardese->find("value");
          if (v != standardese->end() && v->is_string())
            return from_string<cppast::cpp_entity>(v->get_ptr<const nlohmann::json::string_t*>());
          else
            logger::warn(fmt::format("Unsupported value in {}", nlohmann::to_string(value)));
        } else if (kind_ref == "module") {
          const auto name = standardese->find("name");
          if (name != standardese->end() && name->is_string())
            return model::module(name->get<std::string>());
          else
            logger::warn(fmt::format("Unsupported name in {}", nlohmann::to_string(value)));
        } else if (kind_ref == "cpp_type") {
          const auto v = standardese->find("value");
          if (v != standardese->end() && v->is_string())
            return from_string<cppast::cpp_type>(v->get_ptr<const nlohmann::json::string_t*>());
          else
            logger::warn(fmt::format("Unsupported value in {}", nlohmann::to_string(value)));
        } else {
          logger::warn(fmt::format("Unsupported kind in {}", nlohmann::to_string(value)));
        }
      }
    }
    return value.get_ptr<const nlohmann::json::object_t*>();
  } else if (value.is_string()) {
    return value.get_ptr<const nlohmann::json::string_t*>();
  } else if (value.is_array()) {
    return value.get_ptr<const nlohmann::json::array_t*>();
  }

  // TODO
  throw std::logic_error(fmt::format("not implemented: from_json({})", nlohmann::to_string(value)));
}

}
