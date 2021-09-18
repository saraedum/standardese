// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED
#define STANDARDESE_TRANSFORMATION_ENTITY_HEADING_TRANSFORMATION_HPP_INCLUDED

#include "transformation.hpp"
#include "../formatter/inja_formatter.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::transformation {

// TODO: This is crucial and currently completely untested.

/// Inserts headings into a document, e.g., for all
/// [model::cpp_entity_documentation]() entities.
class entity_heading_transformation : public transformation {
  public:
    struct entity_heading_transformation_options {
      entity_heading_transformation_options(formatter::inja_formatter::inja_formatter_options = {});

      std::string format;
      std::string group_format;

      formatter::inja_formatter::inja_formatter_options inja_formatter_options;
    };

    entity_heading_transformation(model::unordered_entities& entities, parser::cpp_context, entity_heading_transformation_options = {});

  protected:
    void do_transform(model::entity& root) override;

  private:
    struct entity_heading_transformation_options options;
    parser::cpp_context cpp_context;
};

}

#endif
