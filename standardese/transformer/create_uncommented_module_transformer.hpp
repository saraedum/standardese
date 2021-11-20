// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_CREATE_UNCOMMENTED_MODULE_TRANSFORMER_HPP
#define STANDARDESE_TRANSFORMER_CREATE_UNCOMMENTED_MODULE_TRANSFORMER_HPP

#include "outer_transformer.hpp"

namespace standardese::transformer {

/// Adds modules to the parse result that are mentioned in other comments but
/// lack explicit documentation.
class create_uncommented_module_transformer : public outer_transformer {
 public:
  using outer_transformer::outer_transformer;

 protected:
  virtual std::vector<model::entity> do_transform(const model::entity& root) const override;

  virtual model::unordered_entities merge(std::vector<std::vector<model::entity>>&&) const override;
};

}

#endif
