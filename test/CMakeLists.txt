# Copyright (C) 2016-2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(tests
    comment/parser.cpp
    output/markup/code_block.cpp
    output/markup/document.cpp
    output/markup/documentation.cpp
    output/markup/heading.cpp
    output/markup/index.cpp
    output/markup/link.cpp
    output/markup/list.cpp
    output/markup/paragraph.cpp
    output/markup/phrasing.cpp
    output/markup/block_quote.cpp
    output/markup/thematic_break.cpp
    comment.cpp
    doc_entity.cpp
    documentation.cpp
    index.cpp
    linker.cpp
    synopsis.cpp
    util/indent.cpp
    util/assertions/sections.cpp)

add_executable(standardese_test test.cpp test_logger.hpp test_parser.hpp ${tests})
target_include_directories(standardese_test PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(standardese_test PUBLIC standardese)
set_target_properties(standardese_test PROPERTIES CXX_STANDARD 17)

enable_testing()
add_test(NAME test COMMAND standardese_test)
