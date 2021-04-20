# Copyright (C) 2016-2017 Jonathan Müller <jonathanmueller.dev@gmail.com>
#                    2021 Julian Rüth <julian.rueth@fsfe.org>
# This file is subject to the license terms in the LICENSE file
# found in the top-level directory of this distribution.

set(src
    parser/command-extension/command_extension.hpp
    parser/command-extension/command_extension.cpp
    parser/verbatim-extension/verbatim_extension.hpp
    parser/verbatim-extension/verbatim_extension.cpp
    parser/ignore-html-extension/ignore_html_extension.hpp
    parser/ignore-html-extension/ignore_html_extension.cpp
    parser/command-extension/user_data.hpp
    parser/command-extension/user_data.cpp
    parser/cmark-extension/cmark_extension.hpp
    parser/cmark-extension/cmark_extension.cpp
    parser/comment_parser.cpp
    parser/comment_parser_options.cpp
    parser/cppast_parser.cpp
    parser/comment_collector.cpp
    parser/cpp_context.cpp
    inventory/files.cpp
    inventory/symbols.cpp
    inventory/inventory.cpp
    inventory/cppast_inventory.cpp
    inventory/sphinx/entry.cpp
    inventory/sphinx/documentation_set.cpp
    transformation/transformation.cpp
    transformation/link_target_internal_transformation.cpp
    transformation/link_target_external_transformation.cpp
    transformation/link_external_legacy_transformation.cpp
    transformation/link_target_unresolved_transformation.cpp
    transformation/link_href_internal_transformation.cpp
    transformation/link_sphinx_transformation.cpp
    transformation/entity_heading_transformation.cpp
    transformation/synopsis_transformation.cpp
    transformation/exclude_uncommented_transformation.cpp
    transformation/output_group_transformation.cpp
    transformation/anchor_transformation.cpp
    output_generator/html/html_generator.cpp
    output_generator/markdown/markdown_generator.cpp
    output_generator/sphinx/inventory_generator.cpp
    output_generator/xml/xml_generator.cpp
    document_builder/index_document_builder.cpp
    document_builder/entity_document_builder.cpp
    model/link_target.cpp
    model/unordered_entities.cpp
    model/visitor/recursive_visitor.cpp
    threading/pool.cpp
    threading/threaded_pool.cpp
    threading/unthreaded_pool.cpp
    tool/document_builders.cpp
    tool/transformations.cpp
    tool/output_generators.cpp
    tool/options.cpp
    tool/parsers.cpp
    logger.cpp)

add_library(standardese ${src})
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

target_link_libraries(standardese PUBLIC pugixml)

target_link_libraries(standardese PUBLIC spdlog)

target_link_libraries(standardese PUBLIC fmt)

# link Boost
find_package(Boost COMPONENTS program_options filesystem iostreams REQUIRED)
target_include_directories(standardese PUBLIC ${Boost_INCLUDE_DIR})
target_link_libraries(standardese PUBLIC ${Boost_LIBRARIES})

# install library: TODO
#install(TARGETS standardese EXPORT standardese DESTINATION "${lib_dest}")
#install(FILES ${header} DESTINATION "${include_dest}/standardese")
#install(FILES ${detail_header} DESTINATION "${include_dest}/standardese/detail")
