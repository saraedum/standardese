// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_OUTPUT_SPHINX_INVENTORY_GENERATOR_HPP_INCLUDED
#define STANDARDESE_OUTPUT_SPHINX_INVENTORY_GENERATOR_HPP_INCLUDED

#include <cppast/forward.hpp>

#include "../stream_generator.hpp"
#include "../../inventory/sphinx/documentation_set.hpp"

namespace standardese::output_generator::sphinx
{

class inventory_generator : public stream_generator {
  public:
    inventory_generator(std::ostream& os);

    void visit(cpp_entity_documentation&) override;
    void visit(group_documentation&) override;
    void visit(document&) override;

    ~inventory_generator() override;

  private:
    std::pair<std::string, std::string> domain_type(const cppast::cpp_entity& entity) const;
    int priority(const cppast::cpp_entity& entity) const;
    std::string display_name(const cppast::cpp_entity& entity) const;
    std::string name(const cppast::cpp_entity& entity) const;

    inventory::sphinx::documentation_set inventory;

    std::string path;
};

}

#endif

