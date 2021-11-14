// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <boost/filesystem.hpp>
#include <vector>
#include <string>

#include "tmp_dir.hpp"

namespace standardese::test::util {

tmp_dir::tmp_dir() :
  path(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path())
  {
    if (!boost::filesystem::create_directories(path))
      throw std::logic_error("Temporary directory with random name already exists. We will not write to it as we can not safely remove it.");
  }

tmp_dir::~tmp_dir() {
  boost::filesystem::remove_all(this->path);
}

std::vector<std::string> tmp_dir::filenames() const {
  std::vector<std::string> filenames;

  for (auto child = boost::filesystem::directory_iterator{path}; child != boost::filesystem::directory_iterator{}; child++)
    filenames.push_back(child->path().filename().native());

  return filenames;
}
}

