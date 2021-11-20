// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_UNCOMMENTED_CHILD_TRANSFORMER_HPP
#define STANDARDESE_TRANSFORMER_CREATE_UNCOMMENTED_CHILD_TRANSFORMER_HPP

#include "outer_transformer.hpp"
#include "../parser/cpp_context.hpp"

namespace standardese::transformer {

/// Adds child entities such as function parameters, template parameters, base
/// class relations to the parse result if they have no explicit comment.
class create_uncommented_child_transformer : public outer_transformer {
 public:
  /// Create a transformer that will produce
  /// [model::cpp_entity_documentation]() entities for all the parameters and
  /// such of all the [entities]().
  create_uncommented_child_transformer(const model::unordered_entities* entities, const parser::cpp_context&);

 protected:
  virtual std::vector<model::entity> do_transform(const model::entity& root) const override;

  parser::cpp_context context;
};

}

#endif
