// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_LINK_TARGET_INTERNAL_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_LINK_TARGET_INTERNAL_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"
#include "../inventory/cppast_inventory.hpp"
#include "../inventory/files.hpp"

namespace standardese::transformation
{

/// Resolves links in MarkDown that use standardese syntax to link to targets within the same project.
class link_target_internal_transformation : public transformation {
  public:
    link_target_internal_transformation(model::unordered_entities& documents, const parser::cpp_context& context);

  protected:
    void do_transform(model::entity&) override;

  private:
    inventory::cppast_inventory symbols;
    inventory::files files;
};

}

#endif

