// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_file.hpp>

#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/parser/cppast_parser.hpp"

#include "cpp_file.hpp"
#include "unindent.hpp"
#include "logger.hpp"

namespace standardese::test::util {

cpp_file::cpp_file(const std::string& code, const std::string& name) : code(unindent(code)), name(name) {
  auto logger = util::logger::throwing_logger();

  if (files.find(key()) == files.end()) {
    parser::cppast_parser parser{{}};

    struct tmp_file {
      tmp_file(const std::string& name) :
        parent(boost::filesystem::temp_directory_path() / boost::filesystem::unique_path()),
        path([&]() {
          if (!boost::filesystem::create_directories(parent))
            throw std::logic_error("Temporary directory with random name already exists. We will not write to it as we can not safely remove it.");
          return parent / name;
        }()),
        stream(boost::filesystem::ofstream(path)) {}

      ~tmp_file() {
        boost::filesystem::remove_all(this->parent);
      }

      boost::filesystem::path parent;
      boost::filesystem::path path;
      boost::filesystem::ofstream stream;
    };

    auto tmp = tmp_file(name);

    tmp.stream << this->code;
    tmp.stream.close();

    files.insert({key(), {type_safe::ref(parser.parse(tmp.path)), parser.context()}});
  }
}

cpp_file::operator type_safe::object_ref<const cppast::cpp_entity>() const {
  return files.at(key()).first;
}

cpp_file::operator type_safe::object_ref<const cppast::cpp_file>() const {
  return files.at(key()).first;
}

cpp_file::operator const cppast::cpp_file&() const {
  return *files.at(key()).first;
}

cpp_file::operator const cppast::cpp_file*() const {
  return &*files.at(key()).first;
}

cpp_file::operator const parser::cpp_context&() const {
  return files.at(key()).second;
}

boost::filesystem::path cpp_file::path() const {
  return name;
}

cpp_file::cache_key cpp_file::key() const {
  return {code, name};
}

const cppast::cpp_entity& cpp_file::operator[](const std::string& name) const {
    auto [file, context] = files.at(key());
    auto result = inventory::cppast_inventory::find(name, *file, context);

    if (!result)
        throw std::logic_error("No symbol `" + name + "` found in \n```\n" + code + "\n```");

    return result.value();
}

std::map<cpp_file::cache_key, std::pair<type_safe::object_ref<const cppast::cpp_file>, parser::cpp_context>> cpp_file::files;

}
