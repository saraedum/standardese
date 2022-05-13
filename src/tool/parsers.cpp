// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include <fstream>
#include <cppast/cpp_entity.hpp>
#include <cppast/visitor.hpp>
#include <cppast/cpp_entity_kind.hpp>
#include <type_safe/optional_ref.hpp>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <utility>

#include "../../standardese/tool/parsers.hpp"
#include "../../standardese/model/entity_set.hpp"
#include "../../standardese/model/document.hpp"
#include "../../standardese/threading/threaded_pool.hpp"
#include "../../standardese/threading/flat_transform.hpp"
#include "../../standardese/threading/transform.hpp"
#include "../../standardese/parser/comment_collector.hpp"
#include "../../standardese/model/cpp_entity_documentation.hpp"
#include "../../standardese/inventory/cppast_inventory.hpp"
#include "../../standardese/inventory/unique_name_inventory.hpp"
#include "../../standardese/inventory/symbols.hpp"

namespace standardese::tool {

parsers::parsers(parser_options options) : options(options) {}

std::pair<model::entity_set, parser::cpp_context> parsers::parse() {
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
  auto comments = threading::flat_transform(workers, successfully_parsed.begin(), successfully_parsed.end(), [&](const auto& cpp_file) {
      return comment_collector.collect(cpp_file);
  });

  // Parse comments as MarkDown...
  auto comment_parser = parser::comment_parser(options.comment_parser_options, cpp_parser.context());

  // ... first process comments that are next to entities and cannot contain an
  // `\entity` command.
  model::entity_set entities;
  entity_set_extend(entities, threading::flat_transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_entity) -> std::vector<model::entity> {
    if (comment_with_entity.location->kind() == cppast::cpp_file::kind())
      // Ignore comments that are not next to an entity.
      return {};
    if (!comment_with_entity.text.has_value())
      // Ignore empty comments initially.
      return {};

    const auto resolve_entity = [](const std::string&) -> const cppast::cpp_entity* {
      throw std::logic_error(R"(not supported: entity comment should not invoke an entity lookup since \entity commands are illegal in such comments.)");
    };

    return comment_parser.parse(*comment_with_entity.text, *comment_with_entity.location, resolve_entity);
  }));

  // ... now we have seen all the relevant `\unique_name` commands and can
  // safely resolve `\entity` commands and merge with what we have so far.

  const inventory::cppast_inventory cppast_inventory{std::vector<const cppast::cpp_entity*>(begin(successfully_parsed), end(successfully_parsed)), cpp_parser.context()};
  const inventory::unique_name_inventory unique_name_inventory{&entities, &cppast_inventory};
  const inventory::symbols unique_name_symbols{&unique_name_inventory};
  const inventory::symbols cppast_symbols{&cppast_inventory};


  entity_set_extend(entities, threading::flat_transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_file) -> std::vector<model::entity> {
    if (comment_with_file.location->kind() != cppast::cpp_file::kind())
      // Ignore this comment because it is next to an entity and was already handled before.
      return {};
    if (!comment_with_file.text.has_value())
      // Ignore empty comments initially.
      return {};

    const auto resolve_entity = [&](const std::string& name) -> const cppast::cpp_entity* {
      const auto* resolved = inventory::cppast_inventory::find(name, inventory::symbols{&unique_name_inventory}, *comment_with_file.location);
      if (resolved != nullptr)
        return resolved;
      return inventory::cppast_inventory::find(name, unique_name_symbols, *comment_with_file.location);
    };

    return comment_parser.parse(*comment_with_file.text, *comment_with_file.location, resolve_entity);
  }));

  // ... finally, add placeholders for all the entities that are not explicitly commented on.
  entity_set_extend(entities, threading::flat_transform(workers, comments.begin(), comments.end(), [&](const auto& comment_with_entity) -> std::vector<model::entity> {
    if (comment_with_entity.text.has_value())
      // Ignore non-empty comments, they have been handled before.
      return {};

    const auto resolve_entity = [](const std::string&) -> const cppast::cpp_entity* {
      throw std::logic_error(R"(not supported: empty comment should not invoke an entity lookup since it cannot contain \entity commands.)");
    };

    // Create documentation for every entity as if it had an empty comment attached to it.
    std::vector<model::entity> parsed;
    
    // We might not be allowed to parse empty comments at the file level if
    // free_file_comments is not enabled, so we need to shortcut that case. It
    // is easy because a header file has no template arguments, arguments,
    // bases and such dependent entities for which need to create documentation
    // entities.
    if (comment_with_entity.location->kind() == cppast::cpp_entity_kind::file_t)
      parsed = {model::cpp_entity_documentation{comment_with_entity.location, cpp_parser.context()}};
    else
      parsed = comment_parser.parse(std::string{}, *comment_with_entity.location, resolve_entity);

    for (auto& entity : parsed)
      entity.template as<model::mixin::documentation>().exclude_mode = model::exclude_mode::uncommented;

    return parsed;
  }));

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
      entity_set_insert(entities, std::move(md.value()));

  // Merge and return entities.
  return {entities, cpp_parser.context()};
}

}
