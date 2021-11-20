// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <cppast/cpp_entity.hpp>
#include <cppast/visitor.hpp>
#include <type_safe/optional_ref.hpp>
#include <numeric>
#include <optional>
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

parsers::parsers(parser_options options) : options(options) {}

std::pair<model::unordered_entities, parser::cpp_context> parsers::parse() {
  // Configure Worker Pool
  auto workers = threading::threaded_pool::factory(options.parallelism);

  // TODO(0.6.0-beta): Split sources when parsing into C - C++ - Markdown?

  // Parse C/C++ source code.
  auto cpp_parser = parser::cppast_parser(options.cppast_options);
  auto parsed = threading::transform(workers, options.sources.begin(), options.sources.end(), [&](const auto& header) -> const cppast::cpp_file* {
    if (boost::filesystem::extension(header) == ".md")
      return nullptr;
    return &cpp_parser.parse(header);
  });

  // Drop files that failed to parse.
  decltype(parsed) successfully_parsed;
  for (auto& cpp_file : parsed)
    if (cpp_file != nullptr)
      successfully_parsed.emplace_back(cpp_file);

  // Collect source code comments.
  auto comment_collector = parser::comment_collector(options.comment_collector_options);
  auto comments = flatten(threading::transform(workers, successfully_parsed.begin(), successfully_parsed.end(), [&](const auto& cpp_file) {
      return comment_collector.collect(cpp_file);
  }));

  // Parse comments as MarkDown...
  auto comment_parser = parser::comment_parser(options.comment_parser_options, cpp_parser.context());

  // ...first process comments that are next to entities and cannot contain an
  // `\entity` command.
  auto entities = flatten(threading::transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_entity) -> std::vector<model::entity> {
    if (comment_with_entity.location->kind() == cppast::cpp_file::kind())
      // Ignore comments that are not next to an entity.
      return {};
    if (!comment_with_entity.text.has_value())
      // Ignore empty comments initially.
      return {};

    const auto resolve_entity = [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
      throw std::logic_error(R"(not supported: entity comment should not invoke an entity lookup since \entity commands are illegal in such comments.)");
    };

    return comment_parser.parse(*comment_with_entity.text, *comment_with_entity.location, resolve_entity);
  }));

  // ...now we have seen all the relevant `\unique_name` commands and can
  // safely resolve `\entity` commands and merge with what we have so far.
  entities = flatten(std::vector{
    flatten(threading::transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_file) -> std::vector<model::entity> {
      if (comment_with_file.location->kind() != cppast::cpp_file::kind())
        // Ignore this comment because it is next to an entity and was already handled before.
        return {};
      if (!comment_with_file.text.has_value())
        // Ignore empty comments initially.
        return {};

      const auto resolve_entity = [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
        throw std::logic_error(R"(not implemented: resolve_entity in tool::parsers.)");
      };

      return comment_parser.parse(*comment_with_file.text, *comment_with_file.location, resolve_entity);
    })),
    std::move(entities),
  });

  // ... finally, add placeholders for all the entities that are not explicitly commented on.
  entities = flatten(std::vector{
    flatten(threading::transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_entity) -> std::vector<model::entity> {
      if (comment_with_entity.text.has_value())
        // Ignore non-empty comments, they have been handled before.
        return {};

      const auto resolve_entity = [](const std::string&) -> type_safe::optional_ref<const cppast::cpp_entity> {
        throw std::logic_error(R"(not supported: empty comment should not invoke an entity lookup since it cannot contain \entity commands.)");
      };

      auto parsed = comment_parser.parse(std::string{}, *comment_with_entity.location, resolve_entity);

      for (auto& entity : parsed)
        entity.template as<model::mixin::documentation>().exclude_mode = model::exclude_mode::uncommented;

      return parsed;
    })),
    std::move(entities),
  });

  // Parse MarkDown files.
  parser::markdown_parser markdown_parser;

  auto mds = threading::transform(workers, options.sources.begin(), options.sources.end(), [&](const auto& md) -> std::optional<model::entity> {
      if (boost::filesystem::extension(md) != ".md")
        return std::nullopt;
      std::ifstream in(md.native());
      std::string raw(std::istreambuf_iterator<char>(in), {});
      auto doc = markdown_parser.parse(raw);

      // TODO(0.6.0-alpha): This is a hack.
      doc.name = md.native();
      if (doc.name.find_last_of('/') != std::string::npos)
        doc.name = doc.name.substr(doc.name.find_last_of('/') + 1);
      if (doc.name.find_first_of('.') != std::string::npos)
        doc.name = doc.name.substr(0, doc.name.find_first_of('.'));

      return doc;
  });

  for (auto& md : mds)
    if (md)
      entities.emplace_back(std::move(md.value()));

  // Merge and return entities.
  return {model::unordered_entities{entities}, cpp_parser.context()};
}

}
