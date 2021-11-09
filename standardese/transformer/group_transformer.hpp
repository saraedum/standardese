// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TRANSFORMER_GROUP_TRANSFORMER_HPP_INCLUDED
#define STANDARDESE_TRANSFORMER_GROUP_TRANSFORMER_HPP_INCLUDED

#include "transformer.hpp"

namespace standardese::transformer {

/// Merges entities that are in the same group, e.g., related overloads that
/// are marked with the same `\group` command.
/// Such merged entities will then typically receive a heading listing all the
/// group members
/// ```
/// (1) first_overload()
/// (2) second_overload()
/// …
/// ```
/// see [entity_heading_transformer]().
class group_transformer : public transformer {
  public:
    struct group_options {
    };

    explicit group_transformer(model::unordered_entities& documents, struct group_options options);

  protected:
    void do_transform(model::entity& root) override;

  private:
    void merge(model::group_documentation&, model::cpp_entity_documentation&&) const;

    group_options options;
};

}

#endif

