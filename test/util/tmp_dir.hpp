// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TEST_UTIL_TMP_DIR_HPP_INCLUDED
#define STANDARDESE_TEST_UTIL_TMP_DIR_HPP_INCLUDED

#include <string>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>

namespace standardese::test::util {
  /// A temporary directory that deletes itself when it goes out of scope.
  struct tmp_dir {
    tmp_dir();

    ~tmp_dir();

    /// Return the names of the files in this directory.
    std::vector<std::string> filenames() const;

    const boost::filesystem::path path;
  };
}

#endif

