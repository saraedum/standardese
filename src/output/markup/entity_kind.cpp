// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../include/standardese/output/markup/entity_kind.hpp"

namespace standardese::output::markup
{

bool is_phrasing(entity_kind kind) noexcept
{
    switch (kind)
    {
    case entity_kind::code_block_keyword:
    case entity_kind::code_block_identifier:
    case entity_kind::code_block_string_literal:
    case entity_kind::code_block_int_literal:
    case entity_kind::code_block_float_literal:
    case entity_kind::code_block_punctuation:
    case entity_kind::code_block_preprocessor:
    case entity_kind::text:
    case entity_kind::emphasis:
    case entity_kind::strong_emphasis:
    case entity_kind::code:
    case entity_kind::verbatim:
    case entity_kind::soft_break:
    case entity_kind::hard_break:
    case entity_kind::term:
    case entity_kind::description:
    case entity_kind::external_link:
    case entity_kind::documentation_link:
        return true;

    case entity_kind::main_document:
    case entity_kind::subdocument:
    case entity_kind::file_documentation:
    case entity_kind::entity_documentation:
    case entity_kind::namespace_documentation:
    case entity_kind::module_documentation:
    case entity_kind::entity_index_item:
    case entity_kind::file_index:
    case entity_kind::entity_index:
    case entity_kind::module_index:
    case entity_kind::heading:
    case entity_kind::subheading:
    case entity_kind::paragraph:
    case entity_kind::list_item:
    case entity_kind::term_description_item:
    case entity_kind::unordered_list:
    case entity_kind::ordered_list:
    case entity_kind::block_quote:
    case entity_kind::code_block:
    case entity_kind::thematic_break:
    case entity_kind::brief_section:
    case entity_kind::details_section:
    case entity_kind::inline_section:
    case entity_kind::list_section:
        break;
    }
    return false;
}

bool is_block(entity_kind kind) noexcept
{
    switch (kind)
    {
    case entity_kind::main_document:
    case entity_kind::subdocument:
    case entity_kind::file_documentation:
    case entity_kind::entity_documentation:
    case entity_kind::namespace_documentation:
    case entity_kind::module_documentation:
    case entity_kind::entity_index_item:
    case entity_kind::file_index:
    case entity_kind::entity_index:
    case entity_kind::module_index:
    case entity_kind::heading:
    case entity_kind::subheading:
    case entity_kind::paragraph:
    case entity_kind::list_item:
    case entity_kind::term_description_item:
    case entity_kind::unordered_list:
    case entity_kind::ordered_list:
    case entity_kind::block_quote:
    case entity_kind::code_block:
    case entity_kind::thematic_break:
        return true;

    case entity_kind::code_block_keyword:
    case entity_kind::code_block_identifier:
    case entity_kind::code_block_string_literal:
    case entity_kind::code_block_int_literal:
    case entity_kind::code_block_float_literal:
    case entity_kind::code_block_punctuation:
    case entity_kind::code_block_preprocessor:
    case entity_kind::text:
    case entity_kind::emphasis:
    case entity_kind::strong_emphasis:
    case entity_kind::code:
    case entity_kind::verbatim:
    case entity_kind::soft_break:
    case entity_kind::hard_break:
    case entity_kind::term:
    case entity_kind::description:
    case entity_kind::external_link:
    case entity_kind::documentation_link:
    case entity_kind::brief_section:
    case entity_kind::details_section:
    case entity_kind::inline_section:
    case entity_kind::list_section:
        break;
    }
    return false;
}

bool is_documentation(entity_kind kind) noexcept
{
    switch (kind)
    {
    case entity_kind::file_documentation:
    case entity_kind::entity_documentation:
    case entity_kind::namespace_documentation:
    case entity_kind::module_documentation:
        return true;

    case entity_kind::main_document:
    case entity_kind::subdocument:
    case entity_kind::entity_index_item:
    case entity_kind::file_index:
    case entity_kind::entity_index:
    case entity_kind::module_index:
    case entity_kind::heading:
    case entity_kind::subheading:
    case entity_kind::paragraph:
    case entity_kind::list_item:
    case entity_kind::term:
    case entity_kind::description:
    case entity_kind::term_description_item:
    case entity_kind::unordered_list:
    case entity_kind::ordered_list:
    case entity_kind::block_quote:
    case entity_kind::code_block:
    case entity_kind::code_block_keyword:
    case entity_kind::code_block_identifier:
    case entity_kind::code_block_string_literal:
    case entity_kind::code_block_int_literal:
    case entity_kind::code_block_float_literal:
    case entity_kind::code_block_punctuation:
    case entity_kind::code_block_preprocessor:
    case entity_kind::brief_section:
    case entity_kind::details_section:
    case entity_kind::inline_section:
    case entity_kind::list_section:
    case entity_kind::thematic_break:
    case entity_kind::text:
    case entity_kind::emphasis:
    case entity_kind::strong_emphasis:
    case entity_kind::code:
    case entity_kind::verbatim:
    case entity_kind::soft_break:
    case entity_kind::hard_break:
    case entity_kind::external_link:
    case entity_kind::documentation_link:
        break;
    }

    return false;
}

}
