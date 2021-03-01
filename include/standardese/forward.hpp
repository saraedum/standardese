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
class entity_index;
class file_index;
class module_index;
class comment_registry;
class linker;

}

namespace standardese::output::markup
{

class block_entity;
class block_id;
class block_quote;
class block_reference;
class brief_section;
class code;
class code_block;
class description;
class details_section;
class doc_section;
class document_entity;
class documentation_entity;
class documentation_header;
class documentation_link;
class emphasis;
class entity;
class entity_documentation;
class entity_index;
class entity_index_item;
class external_link;
class file_documentation;
class file_index;
class hard_break;
class heading;
class index_entity;
class inline_section;
class link_base;
class list_item;
class list_item_base;
class list_section;
class main_document;
class module_documentation;
class module_index;
class namespace_documentation;
class ordered_list;
class output_name;
class paragraph;
class phrasing_entity;
class soft_break;
class strong_emphasis;
class subdocument;
class subheading;
class term;
class term_description_item;
class text;
class thematic_break;
class unordered_list;
class url;
class verbatim;
enum class entity_kind;
template <typename T> class container_entity;

}

namespace standardese::comment
{

class config;
class doc_comment;
class member_group;
class metadata;
class parse_error;
class parser;
enum class command_type;
enum class exclude_mode;
enum class inline_type;
enum class section_type;
struct current_file;
struct inline_base;
struct inline_param;
struct module;
struct parse_result;
struct remote_entity;
struct unmatched_doc_comment;

}

#endif
