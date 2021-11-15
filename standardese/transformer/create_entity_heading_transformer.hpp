// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_ENTITY_HEADING_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_CREATE_ENTITY_HEADING_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"
#include "../formatter/inja_formatter.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::transformer {

/// Inserts headings into a document.
/// For example, this can be used to insert a heading for every
/// [model::cpp_entity_documentation](), i.e., for every documented C/C++
/// entity.
/// This is also used to create the headings for all sections that are
/// created in the source code with commands such as `\returns`.
class create_entity_heading_transformer : public transformer {
  public:
    struct create_entity_heading_transformer_options {
      create_entity_heading_transformer_options(formatter::inja_formatter::inja_formatter_options = {});

      std::string format;
      std::string group_format;

      formatter::inja_formatter::inja_formatter_options inja_formatter_options;
    };

    create_entity_heading_transformer(model::unordered_entities& entities, parser::cpp_context, create_entity_heading_transformer_options = {});

  protected:
    void do_transform(model::entity& root) override;

  private:
    create_entity_heading_transformer_options options;
    parser::cpp_context cpp_context;
};

}

#endif
