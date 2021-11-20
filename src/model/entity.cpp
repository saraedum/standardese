// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../standardese/model/entity.hpp"
#include "../../standardese/output_generator/xml/xml_generator.hpp"

namespace standardese::model {

entity::entity(const entity& rhs) : value(rhs.value->clone()) {}
entity::entity(entity&& rhs) : value(std::move(rhs.value)) {}

entity& entity::operator=(const entity& rhs) {
  value.reset(rhs.value->clone());
  return *this;
}

entity& entity::operator=(entity&& rhs) {
  value = std::move(rhs.value);
  return *this;
}

void entity::accept(visitor::visitor<false>& visitor) {
    value->accept(visitor);
}

void entity::accept(visitor::visitor<true>& visitor) const {
    value->accept(visitor);
}

const mixin::ivisitable* entity::get() const {
    return value.get();
}

std::ostream& operator<<(std::ostream& os, const entity& entity) {
  // TODO(0.6.0-rc): Rendering this as XML is not terribly helpful in many cases.
  auto xml = output_generator::xml::xml_generator{&os, pugi::format_raw};
  entity.accept(xml);
  return os;
}

}
