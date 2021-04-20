# Copyright (C) 2016-2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
#                    2021 Julian Rüth <julian.rueth@fsfe.org>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(tests
    parser/comment_parser.cpp
    inventory/cppast_inventory.cpp
    inventory/sphinx/documentation_set.cpp
    tool/options.cpp
    tool/parsers.cpp
    model/document/entity_document.cpp
    model/document/index_document.cpp
    model/markup/code_block.cpp
    model/visitor/visit.cpp
    model/documentation.cpp
    model/markup/heading.cpp
    model/markup/link.cpp
    model/markup/list.cpp
    model/markup/paragraph.cpp
    model/markup/phrasing.cpp
    model/markup/block_quote.cpp
    model/markup/thematic_break.cpp
    transformation/link_target_internal_transformation.cpp
    transformation/link_target_external_transformation.cpp
    transformation/link_external_legacy_transformation.cpp
    comment.cpp
    doc_entity.cpp
    documentation.cpp
    index.cpp
    synopsis.cpp
    util/unindent.cpp
    util/cpp_file.cpp
    util/logger.cpp
    util/parsed_comments.cpp)

add_executable(standardese_test test.cpp util/logger.hpp ${tests})
target_include_directories(standardese_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(standardese_test PUBLIC standardese)
set_target_properties(standardese_test PROPERTIES CXX_STANDARD 17)

enable_testing()
add_test(NAME test COMMAND standardese_test)
