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
    parser/markdown_parser.cpp
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
    transformer/transformer.cpp
    transformer/group_uncommented_transformer.cpp
    transformer/group_transformer.cpp
    transformer/link_text_transformer.cpp
    transformer/link_target_internal_transformer.cpp
    transformer/link_target_external_transformer.cpp
    transformer/link_external_legacy_transformer.cpp
    transformer/link_target_unresolved_transformer.cpp
    transformer/link_href_internal_transformer.cpp
    transformer/link_sphinx_transformer.cpp
    transformer/entity_heading_transformer.cpp
    transformer/synopsis_transformer.cpp
    transformer/exclude_uncommented_transformer.cpp
    transformer/output_group_transformer.cpp
    transformer/anchor_transformer.cpp
    formatter/inja_formatter.cpp
    formatter/inja_formatter.parse.cpp
    formatter/inja_formatter.json.cpp
    formatter/inja_formatter.format.cpp
    formatter/inja_formatter.add_callback.cpp
    formatter/inja_formatter.name.cpp
    formatter/inja_formatter.namespace.cpp
    formatter/inja_formatter.md.cpp
    formatter/inja_formatter.absolute.cpp
    formatter/inja_formatter.relative.cpp
    formatter/inja_formatter.scope.cpp
    formatter/inja_formatter.filename.cpp
    formatter/inja_formatter.sanitize_basename.cpp
    formatter/inja_formatter.md_escape.cpp
    formatter/inja_formatter.reject.cpp
    formatter/inja_formatter.declaration_specifiers.cpp
    formatter/inja_formatter.target.cpp
    formatter/inja_formatter.parameters.cpp
    formatter/inja_formatter.const_qualification.cpp
    formatter/inja_formatter.volatile_qualification.cpp
    formatter/inja_formatter.ref_qualification.cpp
    formatter/inja_formatter.cppast_kind.cpp
    formatter/inja_formatter.kind.cpp
    formatter/inja_formatter.synopsis.cpp
    formatter/inja_formatter.code.cpp
    formatter/inja_formatter.option.cpp
    formatter/inja_formatter.return_type.cpp
    formatter/inja_formatter.variable_type.cpp
    formatter/inja_formatter.type.cpp
    formatter/inja_formatter.arguments.cpp
    formatter/inja_formatter.entity.cpp
    formatter/inja_formatter.text.cpp
    formatter/inja_formatter.replace.cpp
    formatter/inja_formatter.to_string.cpp
    formatter/inja_formatter.error.cpp
    formatter/inja_formatter.warn.cpp
    formatter/inja_formatter.info.cpp
    formatter/inja_formatter.debug.cpp
    formatter/inja_formatter.trace.cpp
    formatter/inja_formatter.output_section.cpp
    formatter/synopsis_formatter.cpp
    formatter/code_formatter.cpp
    output_generator/cmark_generator.cpp
    output_generator/html/html_generator.cpp
    output_generator/markdown/markdown_generator.cpp
    output_generator/text/text_generator.cpp
    output_generator/sphinx/inventory_generator.cpp
    output_generator/xml/xml_generator.cpp
    document_builder/index_document_builder.cpp
    document_builder/entity_document_builder.cpp
    model/document.cpp
    model/link_target.cpp
    model/unordered_entities.cpp
    model/visitor/recursive_visitor.cpp
    threading/pool.cpp
    threading/threaded_pool.cpp
    threading/unthreaded_pool.cpp
    tool/document_builders.cpp
    tool/transformers.cpp
    tool/output_generators.cpp
    tool/options.cpp
    tool/parsers.cpp
    util/regex.cpp
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
