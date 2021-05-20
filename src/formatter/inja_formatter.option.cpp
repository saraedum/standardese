// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fmt/format.h>

#include "inja_formatter.impl.hpp"
#include "../../standardese/logger.hpp"

namespace standardese::formatter {

std::string inja_formatter::option_callback(const nlohmann::json& data) const {
  return std::visit([&](auto&& name) {
    using T = std::decay_t<decltype(name)>;
    if constexpr (std::is_same_v<T, const nlohmann::json::string_t*>) {
      return option(*name);
    }

    logger::error(fmt::format("Template callback `option` not valid here. Cannot get option for {}.", nlohmann::to_string(data)));
    return std::string{};
  }, self->from_json(data));
}

std::string inja_formatter::option(const std::string& key) const {
  const auto& options = self->options;

  const auto colon = key.find(':');
  if (colon != std::string::npos && key.substr(0, colon) == "default")
    return inja_formatter({}).option(key.substr(colon + 1));

  if (key == "function_format") return options.function_format;
  else if (key == "type_format") return options.type_format;
  else if (key == "return_type_format") return options.return_type_format;
  else if (key == "parameter_type_format") return options.parameter_type_format;
  else if (key == "template_argument_format") return options.template_argument_format;
  else if (key == "declaration_specifiers_format") return options.declaration_specifiers_format;
  else if (key == "function_declarator_format") return options.function_declarator_format;
  else if (key == "function_parameters_format") return options.function_parameters_format;
  else if (key == "function_parameter_format") return options.function_parameter_format;
  else if (key == "const_qualification_format") return options.const_qualification_format;
  else if (key == "volatile_qualification_format") return options.volatile_qualification_format;
  else if (key == "ref_qualification_format") return options.ref_qualification_format;
  else if (key == "noexcept_specification_format") return options.noexcept_specification_format;
  else {
    logger::error(fmt::format("Unknown option `{}` in template.", key));
    return nlohmann::json{};
  }
}

}

