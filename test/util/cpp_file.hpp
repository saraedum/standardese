// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TEST_UTIL_CPP_FILE_HPP_INCLUDED
#define STANDARDESE_TEST_UTIL_CPP_FILE_HPP_INCLUDED

#include <string>
#include <memory>
#include <type_safe/reference.hpp>
#include <cppast/cpp_file.hpp>
#include <boost/filesystem/path.hpp>

#include "../../standardese/parser/cpp_context.hpp"

namespace standardese::test::util {

/// A parsed C++ header file.
class cpp_file {
  public:
    explicit cpp_file(const std::string& code="", const std::string& name="header.hpp");

    /// Return the main `cppast::cpp_file` entity.
    operator type_safe::object_ref<const cppast::cpp_entity>() const;

    /// Return the main `cppast::cpp_file` entity.
    operator type_safe::object_ref<const cppast::cpp_file>() const;

    /// Return the main `cppast::cpp_file` entity.
    operator const cppast::cpp_file&() const;

    /// Return the main `cppast::cpp_file` entity.
    operator const cppast::cpp_file*() const;

    /// Return the compilation context which can be used to resolve type references.
    operator const parser::cpp_context&() const;

    /// Return the C++ entity `name` from the parsed header.
    const cppast::cpp_entity& operator[](const std::string& name) const;

    boost::filesystem::path path() const;

  private:
    std::string code;
    std::string name;

    using cache_key = std::pair<std::string, std::string>;

    cache_key key() const;

    /// Singleton cache of parse results to speed up testing.
    static std::map<cache_key, std::pair<type_safe::object_ref<const cppast::cpp_file>, parser::cpp_context>> files;
};

}

#endif
