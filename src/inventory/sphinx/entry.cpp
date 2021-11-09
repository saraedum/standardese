// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <string>

#include "../../../standardese/inventory/sphinx/entry.hpp"

namespace standardese::inventory::sphinx {

entry::entry(std::string name, std::string domain, std::string type, long priority, std::string uri, std::string display_name) :
  name(std::move(name)),
  domain(std::move(domain)),
  type(std::move(type)),
  priority(priority),
  uri(std::move(uri)),
  display_name(std::move(display_name)) {}

}
