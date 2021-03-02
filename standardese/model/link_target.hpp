// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_LINK_TARGET_HPP_INCLUDED
#define STANDARDESE_MODEL_LINK_TARGET_HPP_INCLUDED

#include <variant>
#include <cppast/forward.hpp>
#include <type_safe/reference.hpp>
#include <type_safe/optional_ref.hpp>

#include "../inventory/sphinx/documentation_set.hpp"

namespace standardese::model
{

/// The target of a MarkDown link.
/// This is usually parsed from a string in a comment, i.e., `[click
/// here](target)` creates the `link_target("target")`. Then, later in the
/// processing, `target` might get resolved to some entity within the
/// documentation. Finally, such a link target is rewritten as an actual
/// relative or absolute URL.
class link_target {
  public:
    struct standardese_target {
      standardese_target(std::string target);

      std::string target;
    };

    struct module_target {
      explicit module_target(std::string module);

      std::string module;
    };

    struct cppast_target {
      cppast_target(const cppast::cpp_entity&);

      type_safe::object_ref<const cppast::cpp_entity> target;
    };

    struct sphinx_target {
      sphinx_target(const inventory::sphinx::documentation_set&, inventory::sphinx::entry entry);

      std::string project;
      std::string version;
      inventory::sphinx::entry entry;
    };

    struct doxygen_target {
    };

    struct uri_target {
      explicit uri_target(std::string uri);

      std::string uri;
    };

    /// Create a link from input in MarkDown with standardese syntax.
    link_target(std::string target);

    /// Create a link to the module `module`.
    link_target(module_target module);

    /// Create a link to the C++ entity `entity`.
    link_target(const cppast::cpp_entity& entity);

    /// Create a link to external documentation generated with sphinx.
    link_target(sphinx_target target);

    /// Create a link to external documentation generated with doxygen.
    link_target(doxygen_target target);

    /// Create a link to an URL, either a http(s) URL or some standardese specific URL scheme.
    link_target(uri_target uri);

    /// The final target of this link, i.e., an absolute or relative URL.
    type_safe::optional<std::string> href() const;

    template <typename Visitor>
    auto accept(Visitor&& visitor) {
      return std::visit(std::forward<Visitor>(visitor), target);
    }

    template <typename Visitor>
    auto accept(Visitor&& visitor) const {
      return std::visit(std::forward<Visitor>(visitor), target);
    }

  private:
    std::variant<
      standardese_target,
      module_target,
      cppast_target,
      sphinx_target,
      doxygen_target,
      uri_target> target;
};

}

#endif
