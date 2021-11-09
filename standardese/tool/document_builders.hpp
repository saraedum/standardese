// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_DOCUMENT_BUILDERS_HPP_INCLUDED
#define STANDARDESE_TOOL_DOCUMENT_BUILDERS_HPP_INCLUDED

#include "../model/unordered_entities.hpp"

namespace standardese::tool {

/// Creates the basic structure of the output documents that the standardese executable produces.
/// This wraps the different documents builder types into single interface.
class document_builders {
 public:
  struct options {
  };

  document_builders(struct options);

  /// Create the documents from the parsed source code.
  model::unordered_entities create(model::unordered_entities& parsed);
};

}

#endif

