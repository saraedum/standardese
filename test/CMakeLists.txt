# Copyright (C) 2016-2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
#                    2021 Julian Rüth <julian.rueth@fsfe.org>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(tests
    comment.cpp
    doc_entity.cpp
    documentation.cpp
    formatter/code_formatter.cpp
    formatter/inja_formatter.cpp
    index.cpp
    inventory/cppast_inventory.cpp
    inventory/sphinx/documentation_set.cpp
    model/documentation.cpp
    model/markup/block_quote.cpp
    model/markup/code_block.cpp
    model/markup/heading.cpp
    model/markup/link.cpp
    model/markup/list.cpp
    model/markup/paragraph.cpp
    model/markup/phrasing.cpp
    model/markup/thematic_break.cpp
    model/visitor/visit.cpp
    parser/command-extension/command_extension.cpp
    parser/comment_parser.cpp
    synopsis.cpp
    tool/options.cpp
    tool/output_generators.cpp
    tool/parsers.cpp
    transformer/create_entity_document_transformer.cpp
    transformer/create_entity_heading_transformer.cpp
    transformer/create_link_text_transformer.cpp
    transformer/create_uncommented_module_transformer.cpp
    transformer/merge_group_transformer.cpp
    transformer/set_href_external_legacy_transformer.cpp
    transformer/set_target_external_transformer.cpp
    transformer/set_target_internal_transformer.cpp
    util/cpp_file.cpp
    util/logger.cpp
    util/parsed_comments.cpp
    util/tmp_dir.cpp
    util/tmp_file.cpp
    util/unindent.cpp
)

add_executable(standardese_test test.cpp util/logger.hpp ${tests})
target_include_directories(standardese_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(standardese_test PUBLIC standardese)
set_target_properties(standardese_test PROPERTIES CXX_STANDARD 17)

enable_testing()
add_test(NAME test COMMAND standardese_test)
