// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../standardese/tool/options.hpp"
#include "../standardese/tool/parsers.hpp"
#include "../standardese/tool/document_builders.hpp"
#include "../standardese/tool/transformations.hpp"
#include "../standardese/tool/output_generators.hpp"
#include "../standardese/model/unordered_entities.hpp"
#include "../standardese/logger.hpp"

int main(int argc, const char* argv[])
{
    // Parse command line options.
    auto options = standardese::tool::options::parse(argc, argv, {});

    if (options.parser_options.sources.empty()) {
      standardese::logger::warn("No input header files.");
    }
    
    // Parse source code.
    auto [parsed, context] = standardese::tool::parsers(options.parser_options).parse();
    
    // Create output document outlines.
    auto documents = standardese::tool::document_builders(options.document_builder_options).create(parsed);

    // Apply transformations to output documents.
    standardese::tool::transformations(options.transformation_options).transform(documents, context);

    // Emit output documents.
    standardese::tool::output_generators(options.output_generator_options).emit(documents);

    if (standardese::logger::errors())
      return 1;

    return 0;
}

/*
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>
#include <type_safe/optional.hpp>
#include <cppast/libclang_parser.hpp>

#include "filesystem.hpp"
#include "generator.hpp"
#include "thread_pool.hpp"

#include "../include/standardese/parser/config.hpp"

namespace po = boost::program_options;
namespace fs = boost::filesystem;

constexpr auto terminal_width = 100u; // assume 100 columns for terminal help text

inline bool default_msvc_comp() noexcept
{
#ifdef _MSC_VER
    return true;
#else
    return false;
#endif
}

void print_version(const char* exe_name)
{
    std::clog << exe_name << " version " << STANDARDESE_VERSION_MAJOR << '.'
              << STANDARDESE_VERSION_MINOR << '\n';
    std::clog << "Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>\n";
    std::clog << '\n';
}

void print_usage(const char* exe_name, const po::options_description& generic,
                 const po::options_description& configuration)
{
    std::clog << "Usage: " << exe_name << " [options] inputs\n";
    std::clog << '\n';
    std::clog << generic << '\n';
    std::clog << '\n';
    std::clog << configuration << '\n';
}

po::variables_map get_options(int argc, char* argv[], const po::options_description& generic,
                              const po::options_description& configuration)
{
    po::variables_map map;

    po::options_description input("");
    po::positional_options_description input_pos;
    input_pos.add("input-files", -1);

    po::options_description cmd;
    cmd.add(generic).add(configuration).add(input);

    auto cmd_result = po::command_line_parser(argc, argv)
                          .options(cmd)
                          .positional(input_pos)
                          .allow_unregistered()
                          .run();
    po::store(cmd_result, map);
    po::notify(map);

    auto               iter = map.find("config");
    po::parsed_options file_result(nullptr);
    if (iter != map.end())
    {
        auto          path = iter->second.as<fs::path>();
        std::ifstream config(path.string());
        if (!config.is_open())
            throw std::runtime_error("config file '" + path.generic_string() + "' not found");

        po::options_description conf;
        conf.add(configuration);

        file_result = po::parse_config_file(config, configuration, true);
        po::store(file_result, map);
        po::notify(map);
    }

    return map;
}

bool has_option(const po::variables_map& map, const char* name)
{
    return map.count(name) != 0u;
}

template <typename T>
type_safe::optional<T> get_option(const po::variables_map& map, const char* name)
{
    auto iter = map.find(name);
    if (iter != map.end())
        return iter->second.as<T>();
    else
        return type_safe::nullopt;
}

inline cppast::cpp_standard parse_standard(const std::string& str)
{
    using cppast::cpp_standard;

    if (str == "c++98")
        return cpp_standard::cpp_98;
    else if (str == "c++03")
        return cpp_standard::cpp_03;
    else if (str == "c++11")
        return cpp_standard::cpp_11;
    else if (str == "c++14")
        return cpp_standard::cpp_14;
    else if (str == "c++1z" || str == "c++17")
        return cpp_standard::cpp_1z;
    else
        throw std::invalid_argument("invalid C++ standard '" + str + "'");
}

cppast::libclang_compile_config get_compile_config(const po::variables_map& options)
{
    cppast::libclang_compile_config config;

    cppast::compile_flags flags;
    if (auto gnu_ext = get_option<bool>(options, "compilation.gnu_extensions"))
        flags.set(cppast::compile_flag::gnu_extensions, gnu_ext.value());
    if (auto ms_ext = get_option<bool>(options, "compilation.ms_extensions"))
        flags.set(cppast::compile_flag::ms_extensions, ms_ext.value());
    if (auto ms_comp = get_option<bool>(options, "compilation.ms_compatibility"))
        flags.set(cppast::compile_flag::ms_compatibility, ms_comp.value());

    auto standard = parse_standard(options.at("compilation.standard").as<std::string>());
    config.set_flags(standard, flags);

    if (auto includes = get_option<std::vector<std::string>>(options, "compilation.include_dir"))
        for (auto& include : includes.value())
            config.add_include_dir(include);

    if (auto macros = get_option<std::vector<std::string>>(options, "compilation.macro_definition"))
        for (auto& macro : macros.value())
        {
            auto pos = macro.find('=');
            if (pos == std::string::npos)
                config.define_macro(macro, "");
            else
                config.define_macro(macro.substr(0, pos), macro.substr(pos + 1));
        }

    if (auto macros
        = get_option<std::vector<std::string>>(options, "compilation.macro_undefinition"))
        for (auto& macro : macros.value())
            config.undefine_macro(macro);

    if (auto features = get_option<std::vector<std::string>>(options, "compilation.feature"))
        for (auto& feature : features.value())
            config.enable_feature(feature);

    return config;
}

type_safe::optional<cppast::libclang_compilation_database> get_compilation_database(
    const po::variables_map& options)
{
    if (auto dir = get_option<std::string>(options, "compilation.commands_dir"))
        return cppast::libclang_compilation_database(dir.value());
    else
        return type_safe::nullopt;
}

std::vector<standardese_tool::input_file> get_input(const po::variables_map& options)
{
    auto source_ext = get_option<std::vector<std::string>>(options, "input.source_ext").value();
    auto blacklist_ext
        = get_option<std::vector<std::string>>(options, "input.blacklist_ext").value();
    auto blacklist_files
        = get_option<std::vector<std::string>>(options, "input.blacklist_file").value();
    auto blacklist_dirs
        = get_option<std::vector<std::string>>(options, "input.blacklist_dir").value();
    auto blacklist_dotfiles = get_option<bool>(options, "input.blacklist_dotfiles").value();
    auto force_blacklist    = get_option<bool>(options, "input.force_blacklist").value();

    auto input_files = get_option<std::vector<fs::path>>(options, "input-files");
    if (!input_files)
        throw std::invalid_argument("no input files specified");

    std::vector<standardese_tool::input_file> files;
    for (auto& file : input_files.value())
        standardese_tool::handle_path(file, source_ext, blacklist_ext, blacklist_files,
                                      blacklist_dirs, blacklist_dotfiles, force_blacklist,
                                      [&](bool, const fs::path& path, const fs::path& relative) {
                                          files.push_back({path, relative});
                                      });

    return files;
}

standardese::parser::config get_comment_config(const po::variables_map& variables)
{
    standardese::parser::config::options options;
    options.command_character = get_option<char>(variables, "comment.command_character").value();
    options.free_file_comments = get_option<bool>(variables, "comment.free_file_comments").value();
    options.group_uncommented = get_option<bool>(variables, "comment.group_uncommented").value();
    options.command_patterns = get_option<std::vector<std::string>>(variables, "comment.command_pattern").value();

    return standardese::parser::config(options);
}

standardese::synopsis_config get_synopsis_config(const po::variables_map& options)
{
    standardese::synopsis_config config;

    if (auto tab = get_option<unsigned>(options, "output.tab_width"))
        config.set_tab_width(tab.value());

    config.set_flag(standardese::synopsis_config::show_complex_noexcept,
                    get_option<bool>(options, "output.show_complex_noexcept").value());
    config.set_flag(standardese::synopsis_config::show_macro_replacement,
                    get_option<bool>(options, "output.show_macro_replacement").value());
    config.set_flag(standardese::synopsis_config::show_group_output_section,
                    get_option<bool>(options, "output.show_group_output_section").value());

    return config;
}

standardese::generation_config get_generation_config(const po::variables_map& options)
{
    standardese::generation_config config;

    config.set_flag(standardese::generation_config::document_uncommented,
                    !get_option<bool>(options, "input.require_comment").value());
    config.set_flag(standardese::generation_config::hide_uncommented,
                    !get_option<bool>(options, "input.hide_uncommented").value());
    config.set_flag(standardese::generation_config::inline_doc,
                    get_option<bool>(options, "output.inline_doc").value());

    auto order = get_option<std::string>(options, "output.entity_index_order").value();
    if (order == "namespace_inline_sorted")
        config.set_order(standardese::entity_index::namespace_inline_sorted);
    else if (order == "namespace_external")
        config.set_order(standardese::entity_index::namespace_external);
    else
        throw std::invalid_argument("unknown entity_index_order '" + order + "'");

    return config;
}

std::vector<std::pair<standardese::markup::generator, const char*>> get_formats(
    const po::variables_map& options)
{
    std::vector<std::pair<standardese::markup::generator, const char*>> formats;

    auto link_prefix    = get_option<std::string>(options, "output.link_prefix").value_or("");
    auto link_extension = get_option<std::string>(options, "output.link_extension");

    auto option = get_option<std::vector<std::string>>(options, "output.format").value();
    for (auto& format : option)
        if (format == "html")
            formats.emplace_back(standardese::markup::html_generator(link_prefix,
                                                                     link_extension.value_or(
                                                                         "html")),
                                 "html");
        else if (format == "xml")
            formats.emplace_back(standardese::markup::xml_generator(), "xml");
        else if (format == "commonmark")
            formats.emplace_back(standardese::markup::markdown_generator(false, link_prefix,
                                                                         link_extension.value_or(
                                                                             "md")),
                                 "md");
        else if (format == "commonmark_html")
            formats.emplace_back(standardese::markup::markdown_generator(true, link_prefix,
                                                                         link_extension.value_or(
                                                                             "md")),
                                 "md");
        else if (format == "text")
            formats.emplace_back(standardese::markup::text_generator(), "txt");
        else
            throw std::invalid_argument("unknown format '" + format + "'");

    return formats;
}

standardese::entity_blacklist get_blacklist(const po::variables_map& options)
{
    standardese::entity_blacklist blacklist(
        get_option<bool>(options, "input.extract_private").value());

    auto blacklist_ns
        = get_option<std::vector<std::string>>(options, "input.blacklist_namespace").value();
    for (auto& ns : blacklist_ns)
        blacklist.blacklist_namespace(ns);

    return blacklist;
}

//TODO: bring these parameters back  // clang-format off
//TODO: bring these parameters back  po::options_description generic("Generic options", terminal_width), configuration("Configuration", terminal_width);
//TODO: bring these parameters back  generic.add_options()
//TODO: bring these parameters back      ("jobs,j", po::value<unsigned>()->default_value(standardese_tool::default_no_threads()),
//TODO: bring these parameters back       "sets the number of threads to use");
//TODO: bring these parameters back
//TODO: bring these parameters back  configuration.add_options()
//TODO: bring these parameters back      ("compilation.commands_dir", po::value<std::string>(),
//TODO: bring these parameters back       "the directory where a compile_commands.json is located, its options have lower priority than the other ones")
//TODO: bring these parameters back      ("compilation.standard", po::value<std::string>()->default_value("c++14"),
//TODO: bring these parameters back       "the C++ standard to use for parsing, valid values are c++98/03/11/14/1z/17")
//TODO: bring these parameters back      ("compilation.include_dir,I", po::value<std::vector<std::string>>(),
//TODO: bring these parameters back       "adds an additional include directory to use for parsing")
//TODO: bring these parameters back      ("compilation.macro_definition,D", po::value<std::vector<std::string>>(),
//TODO: bring these parameters back       "adds an implicit #define before parsing")
//TODO: bring these parameters back      ("compilation.macro_undefinition,U", po::value<std::vector<std::string>>(),
//TODO: bring these parameters back       "adds an implicit #undef before parsing")
//TODO: bring these parameters back      ("compilation.feature,f", po::value<std::vector<std::string>>(),
//TODO: bring these parameters back       "enable a custom feature (-fXX flag)")
//TODO: bring these parameters back      ("compilation.gnu_extensions",
//TODO: bring these parameters back       po::value<bool>()->implicit_value(true)->default_value(false),
//TODO: bring these parameters back       "enable/disable GNU extension support (-std=gnu++XX vs -std=c++XX)")
//TODO: bring these parameters back      ("compilation.ms_extensions",
//TODO: bring these parameters back       po::value<bool>()->implicit_value(true)->default_value(default_msvc_comp()),
//TODO: bring these parameters back       "enable/disable MSVC extension support (-fms-extensions)")
//TODO: bring these parameters back      ("compilation.ms_compatibility",
//TODO: bring these parameters back       po::value<bool>()->implicit_value(true)->default_value(default_msvc_comp()),
//TODO: bring these parameters back       "enable/disable MSVC compatibility (-fms-compatibility)")
//TODO: bring these parameters back
//TODO: bring these parameters back      ("comment.command_character", po::value<char>()->default_value(standardese::parser::config::options().command_character),
//TODO: bring these parameters back       "character used to introduce special commands")
//TODO: bring these parameters back      ("comment.command_pattern", po::value<std::vector<std::string>>()->default_value({}, ""),
//TODO: bring these parameters back       "set the regular expression to detect a command, e.g., `--comment.command_pattern 'returns=RETURNS:'` or `'returns|=RETURNS:'` to also keep the original pattern.")
//TODO: bring these parameters back      ("comment.free_file_comments", po::value<bool>()->implicit_value(true)->default_value(standardese::parser::config::options().free_file_comments),
//TODO: bring these parameters back       "associate free comments to their entire file")
//TODO: bring these parameters back      ("comment.group_uncommented", po::value<bool>()->implicit_value(true)->default_value(standardese::parser::config::options().group_uncommented),
//TODO: bring these parameters back       "group undocumented members with preceding commented members")
//TODO: bring these parameters back      ("output.entity_index_order", po::value<std::string>()->default_value("namespace_inline_sorted"),
//TODO: bring these parameters back       "how the namespaces are handled in the entity index: namespace_inline_sorted (sorted inline with all others), "
//TODO: bring these parameters back       "namespace_external (namespaces in top-level list only, sorted by the end position in the source file)")
//TODO: bring these parameters back      ("output.tab_width", po::value<unsigned>()->default_value(standardese::synopsis_config::default_tab_width()),
//TODO: bring these parameters back       "the tab width (i.e. number of spaces, won't emit tab) of the code in the synthesis")
//TODO: bring these parameters back      ("output.inline_doc", po::value<bool>()->default_value(true)->implicit_value(true),
//      This is the effect. The following inline entities are normally
//      included but can be explicitly disabled.
//        else if (inline_doc
//                 && (entity().kind() == cppast::cpp_function_parameter::kind()
//                     || entity().kind() == cppast::cpp_macro_parameter::kind()))
//            inlines.value().params.add_item(
//                get_inline_doc(get_documentation_id(), entity(), comment()));
//        else if (inline_doc && cppast::is_parameter(entity().kind()))
//            // not a function parameter at this point
//            inlines.value().tparams.add_item(
//                get_inline_doc(get_documentation_id(), entity(), comment()));
//        else if (inline_doc && entity().kind() == cppast::cpp_base_class::kind())
//            inlines.value().bases.add_item(get_inline_doc(get_documentation_id(), entity(), comment()));
//        else if (inline_doc && entity().kind() == cppast::cpp_enum_value::kind())
//            inlines.value().enumerators.add_item(
//                get_inline_doc(get_documentation_id(), entity(), comment()));
//        else if (inline_doc
//                 && (entity().kind() == cppast::cpp_member_variable::kind()
//                     || entity().kind() == cppast::cpp_bitfield::kind()))
//            inlines.value().members.add_item(
//                get_inline_doc(get_documentation_id(), entity(), comment()));
//TODO: bring these parameters back       "whether or not some entity documentation (parameters etc.) will be shown inline")
//TODO: bring these parameters back      ("output.show_complex_noexcept", po::value<bool>()->default_value(true)->implicit_value(true),
//TODO: bring these parameters back       "whether or not complex noexcept expressions will be shown in the synopsis or replaced by \"see below\"")
//TODO: bring these parameters back      ("output.show_macro_replacement", po::value<bool>()->default_value(false)->implicit_value(true),
//TODO: bring these parameters back       "whether or not the replacement of macros will be shown")
//TODO: bring these parameters back      ("output.show_group_output_section", po::value<bool>()->default_value(true)->implicit_value(true),
//TODO: bring these parameters back       "whether or not member groups have an implicit output section");
    // clang-format on

    try
    {
        auto options = get_options(argc, argv, generic, configuration);

        if (has_option(options, "version"))
            print_version(argv[0]);
        else if (has_option(options, "help"))
            print_usage(argv[0], generic, configuration);
        else
        {
            auto no_threads = get_option<unsigned>(options, "jobs").value();

            auto compile_config = get_compile_config(options);
            auto database       = get_compilation_database(options);
            auto input          = get_input(options);

            auto comment_config    = get_comment_config(options);
            auto synopsis_config   = get_synopsis_config(options);
            auto generation_config = get_generation_config(options);

            auto blacklist = get_blacklist(options);

            // auto formats = get_formats(options);
            auto prefix  = get_option<std::string>(options, "output.prefix").value();

            standardese::linker linker;

            try
            {
                cppast::cpp_entity_index index;

                std::clog << "parsing C++ files...\n";
                auto parsed
                    = standardese_tool::parse(compile_config, database, input, index, no_threads);
                if (!parsed)
                    return 1;

                std::clog << "parsing documentation comments...\n";
                auto comments
                    = standardese_tool::parse_comments(comment_config, parsed.value(), no_threads);
                auto files
                    = standardese_tool::build_files(comments, index, std::move(parsed.value()),
                                                    blacklist, generation_config.is_flag_set(standardese::generation_config::hide_uncommented), no_threads);

                std::clog << "generating documentation...\n";
                auto docs = standardese_tool::generate(generation_config, synopsis_config, comments,
                                                       index, linker, files, no_threads);

                for (auto& format : formats)
                {
                    std::clog << "writing files in format '" << format.second << "'...\n";

                    auto format_prefix
                        = formats.size() > 1u ? std::string(format.second) + '/' + prefix : prefix;
                    if (!format_prefix.empty())
                        fs::create_directories(fs::path(format_prefix).parent_path());

                    standardese_tool::write_files(docs, format.first, std::move(format_prefix),
                                                  format.second, no_threads);
                }
            }
            catch (std::exception& ex)
            {
                std::cerr << "error: " << ex.what() << '\n';
            }
        }
    }
    catch (std::exception& ex)
    {
        std::cerr << "error: " << ex.what() << '\n';
        std::cerr << '\n';
        print_usage(argv[0], generic, configuration);
        return 1;
    }
}
*/
