// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TEST_UTIL_TMP_FILE_HPP_INCLUDED
#define STANDARDESE_TEST_UTIL_TMP_FILE_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace standardese::test::util {
  /// A temporary file that deletes itself when it goes out of scope.
  struct tmp_file {
    tmp_file(const std::string& name, const std::string& content="");

    ~tmp_file();

    const boost::filesystem::path parent;
    const boost::filesystem::path path;
  };
}

#endif
