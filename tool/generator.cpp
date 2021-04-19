// Copyright (C) 2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

/*
#include "generator.hpp"

#include <fstream>

#include "../include/standardese/index.hpp"
#include "../include/standardese/linker.hpp"
#include "../include/standardese/model/index_entity.hpp"
#include "../include/standardese/model/file_index.hpp"
#include "../include/standardese/model/module_index.hpp"

#include "thread_pool.hpp"

using namespace standardese_tool;

type_safe::optional<std::vector<parsed_file>> standardese_tool::parse(
    const cppast::libclang_compile_config&                            config,
    const type_safe::optional<cppast::libclang_compilation_database>& database,
    const std::vector<input_file>& files, const cppast::cpp_entity_index& index,
    unsigned no_threads)
{
    std::vector<parsed_file> result;
    bool                     error(false);
    cppast::libclang_parser  parser(cppast::default_logger());

    {
        std::mutex  mutex;
        thread_pool pool(no_threads);
        for (auto& file : files)
        {
            add_job(pool, [&, file] {
                auto db_config = database.map([&](const cppast::libclang_compilation_database& db) {
                    return cppast::find_config_for(db, file.path.generic_string());
                });

                auto actual_config = db_config.value_or(config);
                auto parsed
                    = parser.parse(index, fs::canonical(file.path).generic_string(), actual_config);

                std::lock_guard<std::mutex> lock(mutex);
                if (parsed)
                    result.push_back({std::move(parsed), file.relative.generic_string()});
                else
                    error = true;
            });
        }
    }

    if (error)
        return type_safe::nullopt;
    else
        return std::move(result);
}

standardese::comment_registry standardese_tool::parse_comments(
    const standardese::parser::config& config, const std::vector<parsed_file>& files,
    unsigned no_threads)
{
    standardese::file_comment_parser parser(cppast::default_logger(), config);
    {
        thread_pool pool(no_threads);
        for (auto& file : files)
            add_job(pool, [&file, &parser] { parser.parse(type_safe::ref(*file.file)); });
    }
    return parser.finish();
}

std::vector<std::unique_ptr<standardese::doc_cpp_file>> standardese_tool::build_files(
    const standardese::comment_registry& registry, const cppast::cpp_entity_index& index,
    std::vector<parsed_file>&& files, const standardese::entity_blacklist& blacklist,
    bool hide_uncommented, unsigned no_threads)
{
    {
        thread_pool pool(no_threads);
        for (auto& file : files)
            add_job(pool,
                    [&] { standardese::exclude_entities(registry, index, blacklist, hide_uncommented, *file.file); });
    }

    std::vector<std::unique_ptr<standardese::doc_cpp_file>> result;

    {
        std::mutex  mutex;
        thread_pool pool(no_threads);
        for (auto& file : files)
            add_job(pool, [&] {
                auto entity = standardese::build_doc_entities(type_safe::ref(registry), index,
                                                              std::move(file.file),
                                                              std::move(file.output_name));

                std::lock_guard<std::mutex> lock(mutex);
                result.push_back(std::move(entity));
            });
    }

    return result;
}

namespace
{
standardese::model::document_entity get_index_document(
    standardese::model::index_entity index, const char* title, const char* name)
{
    throw std::logic_error("not implemented: add_child");
    standardese::model::subdocument document(title, name);
    document.add_child(std::move(index));
    return document;
}
} // namespace

documents standardese_tool::generate(
    const standardese::generation_config& gen_config,
    const standardese::synopsis_config& syn_config, const standardese::comment_registry& comments,
    const cppast::cpp_entity_index& index, const standardese::linker& linker,
    const std::vector<std::unique_ptr<standardese::doc_cpp_file>>& files, unsigned no_threads)
{
    std::mutex                                                         result_mutex;
    std::vector<std::unique_ptr<standardese::model::document_entity>> result;

    throw std::logic_error("not implemented: indexes");
    standardese::entity_index eindex;
    standardese::file_index   findex;
    standardese::module_index mindex;

    {
        thread_pool pool(no_threads);

        std::vector<std::future<void>> futures;
        for (auto& file : files)
            futures.push_back(add_job(pool, [&] {
                standardese::model::subdocument::builder document(file->output_name(),
                                                                   "doc_"
                                                                       + get_output_file_name(
                                                                             file->output_name()));
                document.add_child(
                    standardese::generate_documentation(gen_config, syn_config, index, *file));
                auto finished_doc = document.finish();

                standardese::register_documentations(*cppast::default_logger(), linker,
                                                     *finished_doc);
                standardese::register_index_entities(eindex, file->file());
                standardese::register_module_entities(mindex, comments, file->file());
                findex.register_file(file->link_name(), file->output_name(),
                                     file->comment() ? file->comment().value().brief_section()
                                                     : nullptr);

                std::lock_guard<std::mutex> lock(result_mutex);
                result.push_back(std::move(finished_doc));
            }));

        for (auto& future : futures)
            future.get(); // to retrieve exceptions
    }

    auto eindex_doc = get_index_document(eindex.generate(gen_config.order()), "Entities",
                                         "standardese_entities");
    standardese::register_documentations(*cppast::default_logger(), linker, *eindex_doc);
    result.push_back(std::move(eindex_doc));

    auto findex_doc = get_index_document(findex.generate(), "Files", "standardese_files");
    standardese::register_documentations(*cppast::default_logger(), linker, *findex_doc);
    result.push_back(std::move(findex_doc));

    auto mindex_doc = get_index_document(mindex.generate(), "Modules", "standardese_modules");
    standardese::register_documentations(*cppast::default_logger(), linker, *mindex_doc);
    result.push_back(std::move(mindex_doc));

    for (auto& doc : result)
        standardese::resolve_links(*cppast::default_logger(), linker, *doc);

    return result;
}

*/
