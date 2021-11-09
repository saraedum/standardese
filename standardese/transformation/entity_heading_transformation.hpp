// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"

namespace standardese::transformation {

class entity_heading_transformation : public transformation {
  public:
    struct options {
      options();

      std::string file_heading = "{name} — {brief}";
      std::string function_heading = "{synopsis}";
      std::string type_alias_heading = "{kind} `{name}`";
      std::string class_heading = "{kind} `{name}`";
      std::string member_variable_heading = "{kind} `{name}`";
    };

    entity_heading_transformation(model::unordered_entities& entities, options = {});

  protected:
    void do_transform(model::entity& root) override;

  private:
    struct options options;
};

}

#endif
