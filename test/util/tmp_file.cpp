// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>

#include "tmp_file.hpp"

namespace standardese::test::util {

tmp_file::tmp_file(const std::string& name, const std::string& content) :
  parent(),
  path(parent.path / name) {
    boost::filesystem::ofstream stream{path};
    stream << content;
    stream.close();
  }

tmp_file::~tmp_file() {
  // No need to clean up anything. The tmp_dir destructor will take care of that for us.
}

}
