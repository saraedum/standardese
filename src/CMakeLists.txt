# Copyright (C) 2016-2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(comment_header
    ../include/standardese/comment/commands.hpp
    ../include/standardese/comment/config.hpp
    ../include/standardese/comment/doc_comment.hpp
    ../include/standardese/comment/matching_entity.hpp
    ../include/standardese/comment/metadata.hpp
    ../include/standardese/comment/parser.hpp)
set(output_header
    ../include/standardese/output/markup/block_entity.hpp
    ../include/standardese/output/markup/block_id.hpp
    ../include/standardese/output/markup/block_quote.hpp
    ../include/standardese/output/markup/block_reference.hpp
    ../include/standardese/output/markup/brief_section.hpp
    ../include/standardese/output/markup/code_block.hpp
    ../include/standardese/output/markup/code.hpp
    ../include/standardese/output/markup/container_entity.hpp
    ../include/standardese/output/markup/description.hpp
    ../include/standardese/output/markup/detail/vector_ptr_iterator.hpp
    ../include/standardese/output/markup/detail/vector_ptr_range.hpp
    ../include/standardese/output/markup/details_section.hpp
    ../include/standardese/output/markup/documentation_entity.hpp
    ../include/standardese/output/markup/documentation_header.hpp
    ../include/standardese/output/markup/documentation_link.hpp
    ../include/standardese/output/markup/document_entity.hpp
    ../include/standardese/output/markup/doc_section.hpp
    ../include/standardese/output/markup/emphasis.hpp
    ../include/standardese/output/markup/entity_documentation.hpp
    ../include/standardese/output/markup/entity.hpp
    ../include/standardese/output/markup/entity_index.hpp
    ../include/standardese/output/markup/entity_index_item.hpp
    ../include/standardese/output/markup/entity_kind.hpp
    ../include/standardese/output/markup/external_link.hpp
    ../include/standardese/output/markup/file_documentation.hpp
    ../include/standardese/output/markup/file_index.hpp
    ../include/standardese/output/markup/hard_break.hpp
    ../include/standardese/output/markup/heading.hpp
    ../include/standardese/output/markup/index_entity.hpp
    ../include/standardese/output/markup/inline_section.hpp
    ../include/standardese/output/markup/link_base.hpp
    ../include/standardese/output/markup/list_item_base.hpp
    ../include/standardese/output/markup/list_item.hpp
    ../include/standardese/output/markup/list_section.hpp
    ../include/standardese/output/markup/main_document.hpp
    ../include/standardese/output/markup/module_documentation.hpp
    ../include/standardese/output/markup/module_index.hpp
    ../include/standardese/output/markup/namespace_documentation.hpp
    ../include/standardese/output/markup/ordered_list.hpp
    ../include/standardese/output/markup/output_name.hpp
    ../include/standardese/output/markup/paragraph.hpp
    ../include/standardese/output/markup/phrasing_entity.hpp
    ../include/standardese/output/markup/soft_break.hpp
    ../include/standardese/output/markup/strong_emphasis.hpp
    ../include/standardese/output/markup/subdocument.hpp
    ../include/standardese/output/markup/subheading.hpp
    ../include/standardese/output/markup/term_description_item.hpp
    ../include/standardese/output/markup/term.hpp
    ../include/standardese/output/markup/text.hpp
    ../include/standardese/output/markup/thematic_break.hpp
    ../include/standardese/output/markup/unordered_list.hpp
    ../include/standardese/output/markup/url.hpp
    ../include/standardese/output/markup/verbatim.hpp)
set(header
    ../include/standardese/comment.hpp
    ../include/standardese/doc_entity.hpp
    ../include/standardese/index.hpp
    ../include/standardese/linker.hpp
    ../include/standardese/logger.hpp)

set(comment_src
    comment/command-extension/command_extension.hpp
    comment/command-extension/command_extension.cpp
    comment/verbatim-extension/verbatim_extension.hpp
    comment/verbatim-extension/verbatim_extension.cpp
    comment/ignore-html-extension/ignore_html_extension.hpp
    comment/ignore-html-extension/ignore_html_extension.cpp
    comment/command-extension/user_data.hpp
    comment/command-extension/user_data.cpp
    comment/cmark-extension/cmark_extension.hpp
    comment/cmark-extension/cmark_extension.cpp
    comment/config.cpp
    comment/doc_comment.cpp
    comment/parser.cpp)
set(output_src
    output/markup/block.cpp
    output/markup/brief_section.cpp
    output/markup/code_block.cpp
    output/markup/details_section.cpp
    output/markup/document_entity.cpp
    output/markup/main_document.cpp
    output/markup/subdocument.cpp
    output/markup/documentation_header.cpp
    output/markup/documentation_entity.cpp
    output/markup/entity_documentation.cpp
    output/markup/file_documentation.cpp
    output/markup/entity_kind.cpp
    output/markup/heading.cpp
    output/markup/subheading.cpp
    output/markup/html.cpp
    output/markup/entity_index_item.cpp
    output/markup/file_index.cpp
    output/markup/namespace_documentation.cpp
    output/markup/entity_index.cpp
    output/markup/module_documentation.cpp
    output/markup/module_index.cpp
    output/markup/inline_section.cpp
    output/markup/external_link.cpp
    output/markup/documentation_link.cpp
    output/markup/link_base.cpp
    output/markup/list_item.cpp
    output/markup/term.cpp
    output/markup/description.cpp
    output/markup/unordered_list.cpp
    output/markup/ordered_list.cpp
    output/markup/list_section.cpp
    output/markup/markdown.cpp
    output/markup/paragraph.cpp
    output/markup/text.cpp
    output/markup/emphasis.cpp
    output/markup/strong_emphasis.cpp
    output/markup/code.cpp
    output/markup/verbatim.cpp
    output/markup/hard_break.cpp
    output/markup/soft_break.cpp
    output/markup/block_quote.cpp
    output/markup/thematic_break.cpp
    output/markup/xml.cpp)
set(src
    entity_visitor.hpp
    get_special_entity.hpp
    comment.cpp
    doc_entity.cpp
    index.cpp
    linker.cpp
    util/enum_values.hpp)

add_library(standardese ${detail_header} ${comment_header} ${output_header} ${header} ${comment_src} ${output_src} ${src})
set_target_properties(standardese PROPERTIES CXX_STANDARD 17)
target_include_directories(standardese PUBLIC
                                $<BUILD_INTERFACE:${STANDARDESE_SOURCE_DIR}/include>
                                $<INSTALL_INTERFACE:${include_dest}>
                                $<INSTALL_INTERFACE:${include_dest}/comp>)
target_compile_definitions(standardese PUBLIC
                                STANDARDESE=1
                                STANDARDESE_VERSION_MAJOR=${STANDARDESE_VERSION_MAJOR}
                                STANDARDESE_VERSION_MINOR=${STANDARDESE_VERSION_MINOR})

# add threading support
find_package(Threads REQUIRED)
target_link_libraries(standardese PUBLIC Threads::Threads)

# link cppast
target_link_libraries(standardese PUBLIC cppast)

# link cmark
target_link_libraries(standardese PUBLIC libcmark-gfm_static)

# install library: TODO
#install(TARGETS standardese EXPORT standardese DESTINATION "${lib_dest}")
#install(FILES ${header} DESTINATION "${include_dest}/standardese")
#install(FILES ${detail_header} DESTINATION "${include_dest}/standardese/detail")
