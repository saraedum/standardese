// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
//                    2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

// TODO: Should we use std::filesystem everywhere?
// TODO: Make sure that all the default values are automatically synchronized with all the variable default values in the option structs.

#include <cppast/forward.hpp>
#include <iostream>
#include <cstdlib>
#include <stdexcept>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <spdlog/logger.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#endif

#include "../../standardese/tool/options.hpp"
#include "../../standardese/logger.hpp"

#ifndef STANDARDESE_VERSION_MAJOR
#define STANDARDESE_VERSION_MAJOR "?"
#endif

#ifndef STANDARDESE_VERSION_MINOR
#define STANDARDESE_VERSION_MINOR "?"
#endif

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace cppast  {
// TODO: Upstream into cpppast
std::istream& operator>>(std::istream& in, cpp_standard& std) {
  std::string str;
  in >> str;

  if (str == "c++98")
    std = cpp_standard::cpp_98;
  else if (str == "c++03")
    std = cpp_standard::cpp_03;
  else if (str == "c++11")
    std = cpp_standard::cpp_11;
  else if (str == "c++14")
    std = cpp_standard::cpp_14;
  else if (str == "c++1z" || str == "c++17")
    std = cpp_standard::cpp_1z;
  else
    throw std::invalid_argument("Unsupported C++ standard " + str);

  return in;
}

// TODO: Upstream into cpppast
std::ostream& operator<<(std::ostream& os, const cpp_standard& std) {
  switch(std) {
    case cpp_standard::cpp_98:
      return os << "c++98";
    case cpp_standard::cpp_03:
      return os << "c++03";
    case cpp_standard::cpp_11:
      return os << "c++11";
    case cpp_standard::cpp_14:
      return os << "c++14";
    case cpp_standard::cpp_1z:
      return os << "c++17";
  }
}

}

namespace standardese::tool {

namespace {

/// Parses command-line options with `boost::program_options`.
struct options_parser {
    options_parser(struct options::options_options options);
    
    /// Parse command line arguments into our `options`.
    void parse(int argc, const char* argv[]);

    /// The parsed options.
    struct options options;

  private:
    /// Determine the terminal width.
    int terminal_columns() const;

    /// Print help screen.
    void usage() const;

    /// Print version, copyright & license.
    void version() const;

    /// Parse options from this config file into the variables mapping.
    void parse_config_file(const fs::path&, po::variables_map&);

    /// Return generic options that any command line program should probably have.
    po::options_description generic_options() const; 

    /// Put the values of the generic options into the options structs.
    void process_generic_options(po::variables_map&);

    /// Return the `--input.*` options that standardese <0.6.0 supported.  Most
    /// of these options are not supported anymore as we believe that people
    /// should use tools such as `find` to build the list of header files to
    /// process.
    po::options_description legacy_input_options() const;

    /// Process legacy `--input.*` options.
    /// For most of these options, standardese will produce an error and
    /// explain what should be used instead. For others there is just a
    /// deprecation warning pointing out the newer version of that flag.
    void process_legacy_input_options(po::variables_map&);

    /// Return the deprecated `--compilation.*` options that standardese
    /// <0.6.0 supported.
    po::options_description legacy_compilation_options() const;

    /// Rewrite the deprecated `--compilation.*` options for the current
    /// version of standardese.
    void process_legacy_compilation_options(po::variables_map&);

    /// Return the deprecated `--comment.*` options that standardese
    /// <0.6.0 supported.
    po::options_description legacy_comment_options() const;

    /// Rewrite the deprecated `--comment.*` options for the current
    /// version of standardese.
    void process_legacy_comment_options(po::variables_map&);

    /// Return the deprecated `--output.*` options that standardese
    /// <0.6.0 supported.
    po::options_description legacy_output_options() const;

    /// Rewrite the deprecated `--output.*` options for the current
    /// version of standardese.
    void process_legacy_output_options(po::variables_map&);

    /// Return the options for the C/C++ parser.
    po::options_description cpp_parser_options() const;

    /// Process the options for the C/C++ parser.
    void process_cpp_parser_options(po::variables_map&);

    /// Return the options for the MarkDown comment parsing.
    po::options_description markdown_parser_options() const;

    /// Process the options for the MarkDown comment parser.
    void process_markdown_parser_options(po::variables_map&);

    /// Return the options that control linking to other pieces of documentation.
    po::options_description external_options() const;

    /// Process the options for external linking.
    void process_external_options(po::variables_map&);

    /// Return the options that control the composition of the output
    /// document, i.e., which documents are created, and what is included in
    /// these documents.
    po::options_description composition_options() const;

    /// Process the options that control composition of output documents.
    void process_composition_options(po::variables_map&);

    /// Return the options that control the general formatting and rendering of
    /// the output documents.
    po::options_description output_options() const;

    /// Process the generic formatting options.
    void process_output_options(po::variables_map&);

    /// Return the options that control the rendering of outputs as XML.
    po::options_description xml_options() const;

    /// Process options controlling the XML rendering.
    void process_xml_options(po::variables_map&);

    /// Return the options that control the rendering of outputs as [Doxygen
    /// Tagfiles](https://www.doxygen.nl/manual/config.html#cfg_tagfiles).
    po::options_description doxygen_options() const;

    /// Process options controlling the rendering of Doxygen tags.
    void process_doxygen_options(po::variables_map&);

    /// Return the options that control the rendering of outputs in MarkDown.
    po::options_description markdown_options() const;

    /// Process options controlling the MarkDown rendering.
    void process_markdown_options(po::variables_map&);

    /// Return the options that control the rendering of outputs in YAML.
    po::options_description yaml_options() const;

    /// Process options controlling the YAML rendering.
    void process_yaml_options(po::variables_map&);

    /// Return the options that control the rendering of outputs for
    /// [intersphinx](https://www.sphinx-doc.org/en/master/usage/extensions/intersphinx.html).
    po::options_description intersphinx_options() const;

    /// Process options controlling the intersphinx rendering.
    void process_intersphinx_options(po::variables_map&);

    /// Return the options that control how positional options are processed,
    //i.e., the header files to process.
    po::options_description positional_options() const;

    /// Process the positional options, i.e., the header files to process.
    void process_positional_options(po::variables_map&);
};

/// Enables counting flags in boost, see https://stackoverflow.com/a/31697869/812379
struct counter { int count = 0; };

/// Count the number of occurrences of a counter program options flag.
void validate(boost::any& value, std::vector<std::string> const&, counter*, long)
{
  if (value.empty())
    // This is the first occurrence.
    value = counter{1};
  else
    // Another occurrence.
    ++boost::any_cast<counter&>(value).count;
}

}

options::options() {}

options options::parse(int argc, const char* argv[], struct options_options options) {
  auto parser = options_parser(options);
  parser.parse(argc, argv);
  return parser.options;
}

namespace {

options_parser::options_parser(struct options::options_options options) {
  if (options.columns < 0)
      options.columns = terminal_columns();

  this->options.options_options = options; 
}

void options_parser::parse(int argc, const char* argv[]) {
  po::options_description descriptions; 
  descriptions.add(generic_options());
  descriptions.add(legacy_input_options());
  descriptions.add(legacy_compilation_options());
  descriptions.add(legacy_comment_options());
  descriptions.add(legacy_output_options());
  descriptions.add(cpp_parser_options());
  descriptions.add(markdown_parser_options());
  descriptions.add(external_options());
  descriptions.add(composition_options());
  descriptions.add(output_options());
  descriptions.add(xml_options());
  descriptions.add(doxygen_options());
  descriptions.add(markdown_options());
  descriptions.add(yaml_options());
  descriptions.add(intersphinx_options());
  descriptions.add(positional_options());

  try {
    po::positional_options_description headers;
    headers.add("input-files", -1);

    auto result = po::command_line_parser(argc, argv).options(descriptions).positional(headers).run();

    po::variables_map parsed;
    po::store(result, parsed);
    po::notify(parsed);

    process_generic_options(parsed);
    process_legacy_input_options(parsed);
    process_legacy_compilation_options(parsed);
    process_legacy_comment_options(parsed);
    process_legacy_output_options(parsed);
    process_cpp_parser_options(parsed);
    process_markdown_parser_options(parsed);
    process_external_options(parsed);
    process_composition_options(parsed);
    process_output_options(parsed);
    process_xml_options(parsed);
    process_doxygen_options(parsed);
    process_markdown_options(parsed);
    process_yaml_options(parsed);
    process_intersphinx_options(parsed);
    process_positional_options(parsed);
  } catch(std::exception& e) {
    if (options.options_options.throw_on_error)
      throw;

    std::cerr << options.options_options.executable << ": " << e.what() << std::endl;
    std::cerr << "Try '" << options.options_options.executable << " --help' for more information." << std::endl;
    exit(2);
  }
}

void options_parser::version() const {
    std::cerr << options.options_options.executable << " " << STANDARDESE_VERSION_MAJOR << '.' << STANDARDESE_VERSION_MINOR << std::endl;
    std::cerr << "Copyright (C) 2016-2021 Jonathan Müller <jonathanmueller.dev@gmail.com>" << std::endl;
    std::cerr << "License MIT" << std::endl;
    std::cerr << "This is free software: you are free to change and redistribute it." << std::endl;
    std::cerr << "There is NO WARRANTY, to the extent permitted by law." << std::endl;
}

void options_parser::usage() const {
    std::cerr << "Usage: " << options.options_options.executable << " [OPTION]... [HEADER]..." << std::endl;
    std::cerr << "Parse C/C++ source HEADERs and generate documentation for them." << std::endl;
    std::cerr << std::endl << generic_options();
    std::cerr << std::endl << cpp_parser_options();
    std::cerr << std::endl << markdown_parser_options();
    std::cerr << std::endl << external_options();
    std::cerr << std::endl << composition_options();
    std::cerr << std::endl << output_options();
    std::cerr << std::endl << xml_options();
    std::cerr << std::endl << doxygen_options();
    std::cerr << std::endl << markdown_options();
    std::cerr << std::endl << yaml_options();
    std::cerr << std::endl << intersphinx_options();
}

int options_parser::terminal_columns() const {
  const char* columns = getenv("COLUMNS");
  if (columns != nullptr) {
    int width = atoi(columns);
    if (width > 0) return width;
  }

  // autodetection as in https://stackoverflow.com/q/23369503/812379

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
  CONSOLE_SCREEN_BUFFER_INFO csbi;

  GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
  return csbi.srWindow.Right - csbi.srWindow.Left + 1;
#else
  struct winsize w;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
  return w.ws_col;
#endif
}

po::options_description options_parser::generic_options() const {
  auto generic = po::options_description("Generic Options", options.options_options.columns);

  generic.add_options()
        ("config,c", po::value<fs::path>()->value_name("FILE"), "Read additional options from config file.")
        ("warn-as-error,W", po::bool_switch(), "Treat warnings as errors.")
        ("verbose,v", po::value<counter>()->zero_tokens(), "Print verbose messages.");

  if (options.options_options.include_cli_options) {
    generic.add_options()
        ("help,h", "Print this message and exit.")
        ("version,V", "Print version information and exit.");
  }

  return generic;
}

void options_parser::process_generic_options(po::variables_map& parsed) {
  if (parsed.count("config"))
    parse_config_file(parsed.find("config")->second.as<fs::path>(), parsed);

  if (options.options_options.include_cli_options) {
    if (parsed.count("help")) {
      usage();
      exit(0);
    }

    if (parsed.count("version")) {
      version();
      exit(0);
    }
  }

  if (parsed.count("verbose")) {
    switch(parsed.at("verbose").as<counter>().count) {
      case 1:
        logger::get().set_level(spdlog::level::info);
        logger::info("info logging enabled");
        break;
      case 2:
        logger::get().set_level(spdlog::level::debug);
        logger::info("debug logging enabled");
        break;
      default:
        logger::get().set_level(spdlog::level::trace);
        logger::info("trace logging enabled");
        break;
    }
  } else {
    logger::get().set_level(spdlog::level::warn);
  }

  if (parsed.find("warn-as-error")->second.as<bool>()) {
    logger::warn_as_error();
  }
}

po::options_description options_parser::legacy_input_options() const {
  auto legacy = po::options_description("Legacy Input Options", options.options_options.columns);

  legacy.add_options()
      ("input.source_ext", po::value<std::vector<std::string>>(), "File extensions that are treated as header files and will be parsed; this flag is not supported anymore.")
      ("input.blacklist_ext", po::value<std::vector<std::string>>(), "File extensions that are not parsed automatically; this flag is not supported anymore.")
      ("input.blacklist_file", po::value<std::vector<std::string>>(), "File that is not parsed automatically; this flag is not supported anymore.")
      ("input.blacklist_dir", po::value<std::vector<std::string>>(), "Directory which is not parsed automatically; this flag is not supported anymore.")
      ("input.blacklist_dotfiles", po::value<bool>()->default_value(true)->implicit_value(true), "Do not parse dotfiles in the specified directories; this flag is not supported anymore.")
      ("input.force_blacklist", po::value<bool>()->default_value(false)->implicit_value(true), "Apply blacklist rules to files explicitly specified on the command line; this flag is not supported anymore.")
      ("input.blacklist_namespace", po::value<std::vector<std::string>>(), "Namespace that is excluded from the documentation; this flag is deprecated, use --exclude instead.")
      ("input.require_comment", po::value<bool>()->default_value(false)->implicit_value(true), "Omit uncommented members from the synopsis; this flag is deprecated, use --exclude-uncommented --exclude-uncommented instead.")
      ("input.extract_private", po::value<bool>()->default_value(false)->implicit_value(true), "Document private entities; this flag is deprecated, use `--private` instead.");

  return legacy;
}

void options_parser::process_legacy_input_options(po::variables_map& parsed) {
  // Report (and ignore) unsupported --input.* flags.
  {
      std::vector<std::string> source_ext = {".h", ".hpp", ".h++", ".hxx"};
      std::vector<std::string> blacklist_ext = {};
      std::vector<std::string> blacklist_file = {};
      std::vector<std::string> blacklist_dir = {};
      bool blacklist_dotfiles = true;
      bool force_blacklist = false;

      std::vector<std::string> unsupported_flags = {};

      if (parsed.count("input.source_ext")) {
        unsupported_flags.push_back("--input.source_ext");
        source_ext = parsed.find("input.source_ext")->second.as<std::vector<std::string>>();
      }
      if (parsed.count("input.blacklist_ext")) {
        unsupported_flags.push_back("--input.blacklist_ext");
        blacklist_ext = parsed.find("input.blacklist_ext")->second.as<std::vector<std::string>>();
      }
      if (parsed.count("input.blacklist_file")) {
        unsupported_flags.push_back("--input.blacklist_file");
        blacklist_file = parsed.find("input.blacklist_file")->second.as<std::vector<std::string>>();
      }
      if (parsed.count("input.blacklist_dir")) {
        unsupported_flags.push_back("--input.blacklist_dir");
        blacklist_dir = parsed.find("input.blacklist_dir")->second.as<std::vector<std::string>>();
      }
      if (!parsed.find("input.blacklist_dotfiles")->second.defaulted()) {
        unsupported_flags.push_back("--input.blacklist_dotfiles");
        blacklist_dotfiles = parsed.find("input.blacklist_dotfiles")->second.as<bool>();
      }
      if (!parsed.find("input.force_blacklist")->second.defaulted()) {
        unsupported_flags.push_back("--input.force_blacklist");
        force_blacklist = parsed.find("input.force_blacklist")->second.as<bool>();
      }

      if (unsupported_flags.size()) {
        // Print replacement `find ...` commands for the unsupported --input.* flags found on command line.
        std::string find = "find";
        find = find + " " + (force_blacklist ? "DIRECTORIES_AND_FILES" : "DIRECTORIES");

        std::vector<std::string> tests = {find};
        if (blacklist_dir.size()) {
          for (const auto& dname : blacklist_dir)
            tests.push_back(fmt::format("'!' '(' -name '{}' -type d -prune -true ')'", dname));
        }

        tests.push_back("-type f");

        if (source_ext.size()) {
          std::vector<std::string> inames;
          for (const auto& ext : source_ext)
            inames.push_back(fmt::format("-iname '*{}'", ext));
          tests.push_back(fmt::format("'(' {} ')'", fmt::join(inames, " -o ")));
        }

        if (blacklist_ext.size()) {
          for (const auto& ext : blacklist_ext)
            tests.push_back(fmt::format("'!' -iname '*{}'", ext));
        }

        if (blacklist_file.size()) {
          for (const auto& fname : blacklist_file)
            tests.push_back(fmt::format("'!' '(' -name '{}' -type f ')'", fname));
        }

        if (blacklist_dotfiles)
          tests.push_back("'!' -name '.*'");

        find = fmt::format("`{}`", fmt::join(tests, " "));
        if (!force_blacklist)
          find = find + " FILES";

        logger::error(fmt::format("Command line flags detected that are not supported by this version of {} anymore: {}", options.options_options.executable, fmt::join(unsupported_flags, " ")));
        logger::error(fmt::format("Please provide the input files explicitly or use a tool such as `find` to collect the files as in the following (overly complicated) command: {} OPTIONS {}", options.options_options.executable, find));
      }
  }

  // Rewrite deprecated --input.blacklist_namespace as --exclude
  if (parsed.count("input.blacklist_namespace")) {
    if (!parsed.count("exclude"))
      parsed.insert({"exclude", {boost::any(std::vector<std::string>()), false}});

    auto& exclude = parsed.find("exclude")->second.as<std::vector<std::string>>();
    for (auto& name : parsed.find("input.blacklist_namespace")->second.as<std::vector<std::string>>()) {
      logger::warn(fmt::format("--input.blacklist_namespace is deprecated, use --exclude='^{}$|(::)' instead.", name));
      exclude.push_back("^" + name + "$|(::)");
    }
  }

  // Rewrite deprecated --input.require_comment as --exclude-uncommented --exclude-uncommented
  if (!parsed.find("input.require_comment")->second.defaulted()) {
    if (!parsed.count("exclude-uncommented"))
      parsed.insert({"exclude-uncommented", {boost::any(counter()), false}});

    auto& count = parsed.find("exclude-uncommented")->second.as<counter>().count;
    if (count < 2) count = 2;
  }

  // Rewrite deprecated --input.extract_private as --private
  if (!parsed.find("input.extract_private")->second.defaulted()) {
    auto value = parsed.find("input.extract_private")->second.as<bool>();

    if (!parsed.count("private"))
      parsed.insert({"private", {boost::any(value), false}});
    auto& privat = parsed.find("private")->second;

    if (privat.defaulted())
      privat.as<bool>() = value;
    if (privat.as<bool>() != value)
      logger::error("Command line flags for --input.extract_private and --private are incompatible. Ignoring --input.extract_private.");
  }
}

po::options_description options_parser::legacy_compilation_options() const {
  auto legacy = po::options_description("Legacy Compilation Options", options.options_options.columns);

  // TODO: Implement me

  return legacy;
}

void options_parser::process_legacy_compilation_options(po::variables_map&) {}

po::options_description options_parser::legacy_comment_options() const {
  auto legacy = po::options_description("Legacy Compilation Options", options.options_options.columns);

  legacy.add_options()
    ("comment.external_doc", po::value<std::vector<std::string>>()->value_name("namespace=url"), "Link entities in this namespace to a fixed URL prefix.");

  return legacy;
}

void options_parser::process_legacy_comment_options(po::variables_map& parsed) {
  if (parsed.count("comment.external_doc")) {
    for (const auto& external : parsed.find("comment.external_doc")->second.as<std::vector<std::string>>()) {
      static std::regex syntax{R"(([^=]*)=(.*\$\$.*))"};

      std::smatch match;
      if (!std::regex_match(external, match, syntax)) {
        logger::error(fmt::format("Ignoring malformed argument for --external. Must be `NAMESPACE=URL` and URL must contain $$ but found `{}`.", external));
      } else {
        transformations::options::external_legacy_options external_options;
        external_options.options.namspace = match[1];
        external_options.options.url = match[2];
        options.transformation_options.external_link_options.push_back(external_options);
      }
    }
    logger::warn(fmt::format("--comment.external_doc is deprecated. Use --external instead."));
  }
}

po::options_description options_parser::legacy_output_options() const {
  auto legacy = po::options_description("Legacy Compilation Options", options.options_options.columns);

  legacy.add_options()
    ("output.prefix", po::value<boost::filesystem::path>(), "Output directory for generated files.");

  return legacy;
}

void options_parser::process_legacy_output_options(po::variables_map& parsed) {
  // Rewrite deprecated --output.directory as --outdir
  if (parsed.count("output.prefix")) {
    logger::warn("--output.prefix is deprecated, use --outdir instead.");
    parsed.find("outdir")->second.as<boost::filesystem::path>() = parsed.find("output.prefix")->second.as<boost::filesystem::path>();
  }
}

po::options_description options_parser::cpp_parser_options() const {
  auto compiler = po::options_description("C/C++ Parser Options", options.options_options.columns);

  compiler.add_options()
    (",I", po::value<std::vector<std::string>>()->value_name("dir"), "Add directory to be searched for header files.")
    ("std", po::value<cppast::cpp_standard>()->default_value(cppast::cpp_standard::cpp_14), "The C++ standard to use for parsing.")
    ("free-file-comments", po::value<bool>()->default_value(false)->implicit_value(true)->zero_tokens(), "Associate free comments to their header file.");

  return compiler;
}

void options_parser::process_cpp_parser_options(po::variables_map& parsed) {
  if (parsed.count("-I")) {
    for (const auto& dir: parsed.find("-I")->second.as<std::vector<std::string>>())
      options.parser_options.cppast_options.clang_config.add_include_dir(dir);
  }

  if (parsed.count("std")) {
    options.parser_options.cppast_options.clang_config.set_flags(parsed.find("std")->second.as<cppast::cpp_standard>());
  }

  if (parsed.count("free-file-comments"))
    options.parser_options.comment_parser_options.free_file_comments = parsed.find("free-file-comments")->second.as<bool>();
}

po::options_description options_parser::markdown_parser_options() const {
  auto markdown = po::options_description("Comment Parser Options", options.options_options.columns);

  // TODO: Implement me

  return markdown;
}

void options_parser::process_markdown_parser_options(po::variables_map&) {}

po::options_description options_parser::composition_options() const {
  auto composition = po::options_description("Compsition Options", options.options_options.columns);

  // TODO: Implement me

  return composition;
}

void options_parser::process_external_options(po::variables_map& parsed) {
  if (parsed.count("external")) {
    for (auto external: parsed.find("external")->second.as<std::vector<std::string>>()) {
      std::regex syntax{"([^:]*):([^:]*):([^=]*)=(.*)"};
      std::smatch match;
      if (!std::regex_match(external, match, syntax)) {
        logger::error(fmt::format("Igroning malformed command line flag for --external. Must be of the form `KIND:SCHEMA:INVENTORY=URL` but found `{}`.", external));
        continue;
      }

      if (match[1] == "sphinx") {
        transformations::options::external_sphinx_options entry;
        entry.options.schema = match[2];
        entry.inventory = match[3];
        entry.options.url = match[4];
        options.transformation_options.external_link_options.emplace_back(entry);
      } else if (match[1] == "doxygen") {
        transformations::options::external_doxygen_options entry;
        entry.options.schema = match[2];
        entry.inventory = match[3];
        entry.options.url = match[4];
        options.transformation_options.external_link_options.emplace_back(entry);
      } else {
        logger::error(fmt::format("Ignoring malformed command line flag for --external. Must start with one of `sphinx` or `doxygen` but found `{}`.", match[1].str()));
        continue;
      }
    }
  }
}

po::options_description options_parser::external_options() const {
  auto external = po::options_description("External Link Options", options.options_options.columns);

  external.add_options()
    ("external", po::value<std::vector<std::string>>(), "External documentation to link to, can be one of\nsphinx:SCHEMA:INVENTORY=BASE_URL\ndoxygen:SCHEMA:TAGFILE=BASE_URL");

  return external;
}

void options_parser::process_composition_options(po::variables_map&) {}

po::options_description options_parser::output_options() const {
  auto format = po::options_description("Output Options", options.options_options.columns);

  format.add_options()
    ("exclude", po::value<std::vector<std::string>>()->value_name("regex"), "Exclude C/C++ entities whose full name matches this regular expression.")
    ("exclude-uncommented,X", po::value<counter>()->zero_tokens(), "No output for uncommented C/C++ entities, can be specified multiple times.\n-XXXX no output at all.\n-XXX do not apply this to files.\n-XX also do not apply to parents with commented members.\n-X also show uncommented members in their parent's synopsis.")
    ("private", po::value<bool>()->default_value(false)->implicit_value(true)->zero_tokens(), "Include private members and base classes.")
    ("outdir,O", po::value<boost::filesystem::path>()->value_name("dir")->default_value((struct output_generators::options){}.output_directory), "Output directory for generated files.");

  return format;
}

void options_parser::process_output_options(po::variables_map& parsed) {
  if (parsed.count("exclude")) {
    for (auto& pattern : parsed.find("exclude")->second.as<std::vector<std::string>>())
      options.transformation_options.exclude_pattern_options.excluded.emplace_back(pattern);
  }

  if (parsed.count("exclude-uncommented")) {
    using mode = transformation::exclude_uncommented_transformation::options::mode;

    switch(parsed.at("exclude-uncommented").as<counter>().count) {
      case 2:
        options.transformation_options.synopsis_options.exclude_uncommented = true;
        [[fallthrough]];
      case 1:
        options.transformation_options.exclude_uncommented_options.exclude_file = mode::include;
        options.transformation_options.exclude_uncommented_options.exclude_namespace = mode::exclude_if_empty;
        options.transformation_options.exclude_uncommented_options.exclude_class = mode::exclude_if_empty;
        options.transformation_options.exclude_uncommented_options.exclude_alias = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_function = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_variable = mode::exclude;
        break;
      case 3:
        options.transformation_options.exclude_uncommented_options.exclude_file = mode::include;
        options.transformation_options.exclude_uncommented_options.exclude_namespace = mode::exclude_if_empty;
        options.transformation_options.exclude_uncommented_options.exclude_class = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_alias = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_function = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_variable = mode::exclude;
        break;
      default:
        options.transformation_options.exclude_uncommented_options.exclude_file = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_namespace = mode::exclude_if_empty;
        options.transformation_options.exclude_uncommented_options.exclude_class = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_alias = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_function = mode::exclude;
        options.transformation_options.exclude_uncommented_options.exclude_variable = mode::exclude;
        break;
    }
  }

  if (parsed.count("private")) {
    options.transformation_options.exclude_access_options.exclude_private = !parsed.find("private")->second.as<bool>();
  }

  if (parsed.count("outdir")) {
    options.output_generator_options.output_directory = parsed.find("outdir")->second.as<boost::filesystem::path>();
  }
}

po::options_description options_parser::xml_options() const {
  auto xml = po::options_description("XML Rendering Options", options.options_options.columns);

  // TODO: Implement me

  return xml;
}

void options_parser::process_xml_options(po::variables_map&) {}

po::options_description options_parser::doxygen_options() const {
  auto doxygen = po::options_description("Doxygen Tag File Rendering Options", options.options_options.columns);

  // TODO: Implement me

  return doxygen;
}

void options_parser::process_doxygen_options(po::variables_map&) {}

po::options_description options_parser::markdown_options() const {
  auto markdown = po::options_description("MarkDown Rendering Options", options.options_options.columns);

  // TODO: Implement me

  return markdown;
}

void options_parser::process_markdown_options(po::variables_map&) {}

po::options_description options_parser::yaml_options() const {
  auto yaml = po::options_description("YAML Rendering Options", options.options_options.columns);

  // TODO: Implement me

  return yaml;
}

void options_parser::process_yaml_options(po::variables_map&) {}

po::options_description options_parser::intersphinx_options() const {
  auto intersphinx = po::options_description("Intersphinx Inventory Rendering Options", options.options_options.columns);

  // TODO: Implement me

  return intersphinx;
}

void options_parser::process_intersphinx_options(po::variables_map&) {}

po::options_description options_parser::positional_options() const {
  auto positional = po::options_description("Positional Options", options.options_options.columns);

  positional.add_options()("input-files", po::value<std::vector<fs::path>>(), "Header files to process.");

  return positional;
}

void options_parser::process_positional_options(po::variables_map& parsed) {
  if (parsed.count("input-files"))
    options.parser_options.sources = parsed.find("input-files")->second.as<std::vector<fs::path>>();
}

void options_parser::parse_config_file(const fs::path& path, po::variables_map& parsed) {
  std::ifstream config(path.string());
  if (!config.is_open())
    throw std::runtime_error("config file '" + path.generic_string() + "' not found.");

  po::options_description descriptions;
  descriptions.add(generic_options());
  descriptions.add(legacy_input_options());
  descriptions.add(legacy_compilation_options());
  descriptions.add(legacy_comment_options());
  descriptions.add(legacy_output_options());
  descriptions.add(cpp_parser_options());
  descriptions.add(markdown_parser_options());
  descriptions.add(external_options());
  descriptions.add(composition_options());
  descriptions.add(output_options());
  descriptions.add(xml_options());
  descriptions.add(doxygen_options());
  descriptions.add(markdown_options());
  descriptions.add(yaml_options());
  descriptions.add(intersphinx_options());
  descriptions.add(positional_options());
  auto result = po::parse_config_file(config, descriptions, false);
  po::store(result, parsed);
  po::notify(parsed);
}

}

}
