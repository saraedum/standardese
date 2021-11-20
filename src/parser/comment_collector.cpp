// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/visitor.hpp>
#include <cppast/cpp_file.hpp>
#include <stdexcept>

#include "../../standardese/parser/comment_collector.hpp"

namespace standardese::parser {

comment_collector::comment::comment(const cppast::cpp_entity* location, std::optional<std::string> text) : text(std::move(text)), location(location) {
  if (location == nullptr)
    throw std::invalid_argument("comment location mut not be NULL");
}

comment_collector::comment_collector(comment_collector_options options) : options(options) {}

std::vector<comment_collector::comment> comment_collector::collect(const cppast::cpp_file* cpp_file) {
  std::vector<comment> comments;

  cppast::visit(*cpp_file, [&](const cppast::cpp_entity& entity, const cppast::visitor_info& info) {
      if (info.is_old_entity())
          // Continue visit but do not register this container twice.
          return true;

      if (cppast::is_friended(entity))
          // TODO(0.6.0-alpha): Why?
          return true;

      if (cppast::is_templated(entity))
          // Ignore templates themselves since we will only handle what's inside the template.
          return true;

      comments.emplace_back(&entity, entity.comment().has_value() ? std::optional{entity.comment().value()} : std::nullopt);

      return true;
  });

  for (const auto& free : static_cast<const cppast::cpp_file*>(cpp_file)->unmatched_comments())
    comments.emplace_back(static_cast<const cppast::cpp_entity*>(cpp_file), free.content);

  return comments;
}

}
