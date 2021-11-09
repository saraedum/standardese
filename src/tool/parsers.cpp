// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/visitor.hpp>
#include <type_safe/optional_ref.hpp>
#include <numeric>
#include <stdexcept>
#include <utility>

#include "../../standardese/tool/parsers.hpp"
#include "../../standardese/model/unordered_entities.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/threading/threaded_pool.hpp"
#include "../../standardese/threading/transform.hpp"
#include "../../standardese/parser/comment_collector.hpp"

namespace standardese::tool {

namespace {

template <typename R, typename T = typename std::decay_t<R>::iterator::value_type::value_type>
std::vector<T> flatten(R&& ranges) {
  std::vector<T> flattened;

  for (auto& src : ranges)
    flattened.insert(flattened.end(), std::make_move_iterator(src.begin()), std::make_move_iterator(src.end()));

  return flattened;
}

}

parsers::parsers(struct options options) : options(options) {}

std::pair<model::unordered_entities, parser::cpp_context> parsers::parse() {
  // Configure Worker Pool
  auto workers = threading::threaded_pool::factory(options.parallelism);

  // TODO: Split sources when parsing into C - C++ - Markdown?

  // Parse C/C++ source code.
  auto cpp_parser = parser::cppast_parser(options.cppast_options);
  auto parsed = threading::transform(workers, options.sources.begin(), options.sources.end(), [&](const auto& header) -> type_safe::optional<type_safe::object_ref<const cppast::cpp_file>> {
    if (boost::filesystem::extension(header) == ".md")
      return {};
    return type_safe::ref(cpp_parser.parse(header));
  });

  // Drop files that failed to parse.
  decltype(parsed) successfully_parsed;
  for (auto& cpp_file : parsed)
    if (cpp_file.has_value())
      successfully_parsed.emplace_back(std::move(cpp_file));

  // Collect source code comments.
  auto comment_collector = parser::comment_collector(options.comment_collector_options);
  auto comments = flatten(threading::transform(workers, successfully_parsed.begin(), successfully_parsed.end(), [&](const auto& cpp_file) {
      return comment_collector.collect(*cpp_file.value());
  }));

  // Parse comments as MarkDown...
  auto comment_parser = parser::comment_parser(options.comment_parser_options, cpp_parser.context());

  // ...first process comments that are next to entities and cannot contain an
  // `\entity` command.
  auto entities = flatten(threading::transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_entity) -> std::vector<model::entity> {
    if (std::get<1>(comment_with_entity)->kind() == cppast::cpp_file::kind())
      return {};

    const auto resolve_entity = [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
      throw std::logic_error(R"(not implemented: entity comment should not invoke an entity lookup since \entity commands are illegal in such comments.)");
    };

    return comment_parser.parse(std::get<0>(comment_with_entity), *std::get<1>(comment_with_entity), resolve_entity);
  }));

  // ...now we have seen all the relevant `\unique_name` commands and can
  // safely resolve `\entity` commands and merge with what we have so far.
  entities = flatten(std::vector{
    flatten(threading::transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_file) -> std::vector<model::entity> {
        if (std::get<1>(comment_with_file)->kind() != cppast::cpp_file::kind())
          return {};

        const auto resolve_entity = [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
          throw std::logic_error(R"(not implemented: resole_entity in tool::parsers.)");
        };

        return comment_parser.parse(std::get<0>(comment_with_file), *std::get<1>(comment_with_file), resolve_entity);
    })),
    std::move(entities),
  });

  // Parse MarkDown files.
  auto mds = threading::transform(workers, options.sources.begin(), options.sources.end(), [&](const auto& md) {
      if (boost::filesystem::extension(md) != ".md")
        return type_safe::optional<model::entity>();
      std::ifstream in(md.native());
      std::string raw(std::istreambuf_iterator<char>(in), {});
      auto doc = comment_parser.parse(raw);
      // TODO: This is a hack.
      doc.name = md.native();
      if (doc.name.find_last_of('/') != std::string::npos)
        doc.name = doc.name.substr(doc.name.find_last_of('/') + 1);
      if (doc.name.find_first_of('.') != std::string::npos)
        doc.name = doc.name.substr(0, doc.name.find_first_of('.'));
      return type_safe::optional<model::entity>(doc);
  });

  for (auto& md : mds)
    if (md)
      entities.emplace_back(std::move(md.value()));

  // Merge entities.
  auto ret = model::unordered_entities(entities);

  // TODO: Is this really what we should do? And should we do this here?
  for (auto& cpp_file : successfully_parsed)
    comment_parser.add_uncommented_entities(ret, *cpp_file.value());
  // TODO: Is this really what we should do? And should we do this here?
  comment_parser.add_missing_sections(ret);

  return {std::move(ret), cpp_parser.context()};
}

}
