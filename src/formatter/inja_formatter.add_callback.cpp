// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "inja_formatter.impl.hpp"

namespace standardese::formatter {

void inja_formatter::add_callback(const std::string& name, std::function<nlohmann::json()> callback) {
  self->env.add_callback(name, 0, [callback](inja::Arguments& args) {
    return callback();
  });
}

void inja_formatter::add_callback(const std::string& name, std::function<nlohmann::json(std::vector<const nlohmann::json*>)> callback) {
  self->env.add_callback(name, [callback](inja::Arguments& args) {
    std::vector<const nlohmann::json*> jargs;
    for (const auto& arg : args)
      jargs.push_back(arg);
    return callback(jargs);
  });
}

void inja_formatter::add_void_callback(const std::string& name, std::function<void(std::vector<const nlohmann::json*>)> callback) {
  self->env.add_void_callback(name, [callback](inja::Arguments& args) {
    std::vector<const nlohmann::json*> jargs;
    for (const auto& arg : args)
      jargs.push_back(arg);
    return callback(jargs);
  });
}

}
