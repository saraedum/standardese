// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_TOOL_GENERATOR_HPP_INCLUDED
#define STANDARDESE_TOOL_GENERATOR_HPP_INCLUDED

/*
#include <vector>

#include <cppast/cpp_entity_index.hpp>
#include <cppast/cpp_file.hpp>
#include <cppast/libclang_parser.hpp>

#include "../include/standardese/comment.hpp"
#include "../include/standardese/doc_entity.hpp"
#include "../include/standardese/linker.hpp"
#include "../include/standardese/model/document_entity.hpp"

#include "filesystem.hpp"

namespace standardese_tool
{
struct input_file
{
    fs::path path;
    fs::path relative;
};

struct parsed_file
{
    std::unique_ptr<cppast::cpp_file> file;
    std::string                       output_name;
};

type_safe::optional<std::vector<parsed_file>> parse(
    const cppast::libclang_compile_config&                            config,
    const type_safe::optional<cppast::libclang_compilation_database>& database,
    const std::vector<input_file>& files, const cppast::cpp_entity_index& index,
    unsigned no_threads);

standardese::comment_registry parse_comments(const standardese::parser::config& config,
                                             const std::vector<parsed_file>&     files,
                                             unsigned                            no_threads);

std::vector<std::unique_ptr<standardese::doc_cpp_file>> build_files(
    const standardese::comment_registry& registry, const cppast::cpp_entity_index& index,
    std::vector<parsed_file>&& files, const standardese::entity_blacklist& blacklist,
    bool hide_uncommented, unsigned no_threads);

using documents = std::vector<std::unique_ptr<standardese::model::document_entity>>;

documents generate(const standardese::generation_config& gen_config,
                   const standardese::synopsis_config&   syn_config,
                   const standardese::comment_registry&  comments,
                   const cppast::cpp_entity_index& index, const standardese::linker& linker,
                   const std::vector<std::unique_ptr<standardese::doc_cpp_file>>& files,
                   unsigned                                                       no_threads);

} // namespace standardese_tool
*/

#endif // STANDARDESE_TOOL_GENERATOR_HPP_INCLUDED
