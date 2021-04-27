// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_FORWARD_HPP_INCLUDED
#define STANDARDESE_FORWARD_HPP_INCLUDED

namespace standardese
{

class synopsis_config;
class generation_config;
class doc_entity;
class doc_excluded_entity;
class doc_cpp_entity;
class doc_metadata_entity;
class doc_member_group_entity;
class doc_cpp_namespace;
class doc_cpp_file;
class entity_blacklist;
class comment_registry;
class file_comment_parser;
class comment_registry;
class linker;

}

namespace standardese::model::markup
{
class block_quote;
class code;
class code_block;
class emphasis;
class hard_break;
class heading;
class link;
class list_item;
class list;
class paragraph;
class soft_break;
class strong_emphasis;
class text;
class thematic_break;
class image;
}

namespace standardese::model
{

class document;
class entity;
class cpp_entity_documentation;
enum class exclude_mode;
class module;
class unordered_entities;
class section;
class link_target;

}

namespace standardese::model::visitor
{

template <bool>
class visitor;
template <bool>
class recursive_visitor;

}

namespace standardese::comment
{

class doc_comment;
class member_group;
class metadata;
enum class exclude_mode;
struct current_file;
struct inline_base;
struct inline_param;
struct module;
struct parse_result;
struct remote_entity;
struct unmatched_doc_comment;

}

namespace standardese::parser
{
class markdown_parser;
class comment_parser;
class parse_error;
class cpp_context;
}

namespace standardese::inventory
{
class files;
class cppast_inventory;
class symbols;
}

namespace standardese::inventory::sphinx {
class entry;
class documentation_set;
}

namespace standardese::parser::commands
{
enum class special_command;
enum class inline_command;
enum class section_command;
}

namespace standardese::document_builder
{

class index_document_builder;
class entity_document_builder;

}

namespace standardese::transformation
{

class link_target_internal_transformation;
class link_target_external_transformation;
class link_href_internal_transformation;
class link_sphinx_transformation;
class link_external_legacy_transformation;
class anchor_transformation;
class exclude_pattern_transformation;
class exclude_access_transformation;
class synopsis_transformation;

}

namespace standardese::formatter {

class inja_formatter;

}

namespace standardese::threading
{

class pool;
class unthreaded_pool;

}

namespace standardese::tool
{

class options;
class parsers;
class document_builders;
class transformations;
class output_generators;

}

namespace standardese::output_generator {

template <typename G>
class stream_generator;

}

namespace standardese::output_generator::html {

class html_generator;

}
namespace standardese::output_generator::markdown {

class markdown_generator;

}
namespace standardese::output_generator::xml {

class xml_generator;

}

#endif
