// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>

#include "tmp_file.hpp"

namespace standardese::test::util {

tmp_file::tmp_file(const std::string& name, const std::string& content) :
  parent(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()),
  path([&]() {
    if (!boost::filesystem::create_directories(parent))
      throw std::logic_error("Temporary directory with random name already exists. We will not write to it as we can not safely remove it.");
    return parent / name;
  }()) {
    boost::filesystem::ofstream stream{path};
    stream << content;
    stream.close();
  }

tmp_file::~tmp_file() {
  boost::filesystem::remove_all(this->parent);
}

}
