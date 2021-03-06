// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_INVENTORY_DOXYGEN_TAGFILE_HPP_INCLUDED
#define STANDARDESE_INVENTORY_DOXYGEN_TAGFILE_HPP_INCLUDED

#include <iosfwd>
#include <string>
#include <vector>
#include <type_safe/optional.hpp>

#include "../inventory.hpp"
#include "anchor.hpp"

namespace standardese::inventory::doxygen {

/// The contents of a [doxygen tagfile](https://www.doxygen.nl/manual/external.html).
///
/// Tagfiles are generated by [doxygen](https://www.doxygen.nl). Such
/// inventories can be used to establish links between the documentations of
/// different projects, i.e., they resolve documented entities to URLs.
/// Standardese also generates such inventories itself, see
/// [tagfile_generator]().
///
/// For the precise structure of a tagfile see
/// https://github.com/doxygen/doxygen/blob/master/src/tagreader.cpp.
struct tagfile : public inventory {
  // TODO: Move members to separate files?
  // Corresponds to `TagEnumValueInfo` in doxygen.
  struct enum_value {
    std::string name;
    std::string file;
    std::string anchor;
    std::string clang_id;
  };

  // Corresponds to `TagIncludeInfo` in doxygen.
  struct include {
    std::string id;
    std::string name;
    std::string text;
    bool isLocal;
    bool isImported;
  };

  // Corresponds to `TagMemberInfo` in doxygen.
  struct member {
    std::string kind;

    std::string type;
    std::string name;
    std::string anchor_file;
    std::string anchor;
    std::string args;
    std::string clang_id;
    std::vector<struct anchor> anchors;
    std::string protection = "public";
    std::string virt = "normal";
    bool is_static = false;
    std::vector<enum_value> enum_values;
  };

  // Corresponds to `BaseInfo` in doxygen.
  struct base {
    std::string name;
    std::string virt = "normal";
    std::string protection = "public";
  };

  // Corresponds to `TagCompoundInfo` in doxygen.
  struct compound {
    std::string name;
    std::string filename;
    std::vector<struct anchor> anchors;
    std::vector<struct member> members;
  };

  // Corresponds to `TagClassInfo` in doxygen.
  struct type : compound {
    std::string kind;

    std::string clang_id;
    std::string anchor;
    std::vector<std::string> bases;
    std::vector<std::string> template_args;
    std::vector<std::string> classes;
    bool is_obj_c = false;
  };

  // Corresponds to `TagConceptInfo` in doxygen.
  struct concept : compound {
    std::string clang_id;
  };

  // Corresponds to `TagNamespaceInfo` in doxygen.
  struct namespase : compound {
    std::string clangId;
    std::vector<std::string> classes;
    std::vector<std::string> concepts;
    std::vector<std::string> namespaces;
  };

  // Corresponds to `TagPackageInfo` in doxygen.
  struct package : compound {
    std::vector<std::string> classes;
  };

  // Corresponds to `TagFileInfo` in doxygen.
  struct file : compound {
    std::string path;
    std::vector<std::string> classes;
    std::vector<std::string> concepts;
    std::vector<std::string> namespaces;
    std::vector<include> includes;
  };

  // Corresponds to `TagGroupInfo` in doxygen.
  struct group : compound {
    std::string title;
    std::vector<std::string> subgroups;
    std::vector<std::string> classes;
    std::vector<std::string> concepts;
    std::vector<std::string> namespaces;
    std::vector<std::string> files;
    std::vector<std::string> pages;
    std::vector<std::string> directories;
  };

  // Corresponds to `TagPageInfo` in doxygen.
  struct page : compound {
    std::string title;
  };

  // Corresponds to `TagDirInfo` in doxygen.
  struct directory : compound {
    std::string path;
    std::vector<std::string> subdirectories;
    std::vector<std::string> files;
  };

  static tagfile parse(const std::string& fname);

  friend std::istream& operator>>(std::istream&, tagfile&);
  friend std::ostream& operator<<(std::ostream&, const tagfile&);

  std::vector<type> types;
  std::vector<concept> concepts;
  std::vector<namespase> namespaces;
  std::vector<package> packages;
  std::vector<file> files;
  std::vector<group> groups;
  std::vector<page> pages;
  std::vector<directory> directories;
};

}

#endif
