// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//               2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_DOCUMENT_BUILDERS_HPP_INCLUDED
#define STANDARDESE_TOOL_DOCUMENT_BUILDERS_HPP_INCLUDED

#include <string>

#include "../model/unordered_entities.hpp"

namespace standardese::tool {

/// Creates the basic structure of the output documents that the standardese executable produces.
/// This wraps the different documents builder types into single interface.
class document_builders {
 public:
  struct options {
    // TODO: Implement me
    /// The name of the output file (without the suffix).
    /// The default is to take a cleaned-up name of what we are documenting.
    /// If that name is not unique a suffix will be added automatically.
    std::string name = "doc_{{ as_filename(name) }}";

    // TODO: Implement me
    /// The name of the final rendered (HTML) document.
    /// Links to this document will assume that this is the eventual (absolute)
    /// path of this document.
    /// The default is to just take the [name]() from above and assume that
    /// everything lives under the same document root without extensions such as
    /// `.html`.
    std::string path = "{{ document_name }}";
  };

  document_builders(struct options);

  /// Create the documents from the parsed source code.
  model::unordered_entities create(model::unordered_entities& parsed);

 private:
  struct options options;
};

}

#endif

