// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MARKUP_GENERATOR_HPP_INCLUDED
#define STANDARDESE_MARKUP_GENERATOR_HPP_INCLUDED

#include <boost/filesystem.hpp>

#include <functional>
#include <iosfwd>
#include <string>

namespace standardese
{
namespace markup
{
    class entity;

    /// A generator.
    ///
    /// It will write the entity representation to the given stream.
    using generator = std::function<void(std::ostream&, const entity&)>;

    /// Renders an entity to a string.
    ///
    /// \returns The string representation of the entity in the given format.
    std::string render(generator gen, const entity& e);

    /// An HTML generator.
    ///
    /// \returns A generator that will generate the HTML representation.
    generator html_generator(const std::string& link_prefix, const std::string& extension) noexcept;

    /// Renders an entity as HTML.
    ///
    /// \returns `render(html_generator(), e)`.
    inline std::string as_html(const entity& e)
    {
        return render(html_generator("", "html"), e);
    }

    /// A markdown generator.
    ///
    /// \returns A generator that will generate a CommonMark representation.
    /// If `use_html` is `true`, it will use HTML for complex parts that cannot be described using
    /// CommonMark.
    generator markdown_generator(bool use_html, const std::string& link_prefix,
                                 const std::string& extension, const boost::filesystem::path& root = boost::filesystem::current_path()) noexcept;

    /// Renders an entity as CommonMark.
    ///
    /// \returns `render(commonmark_generator(), e)`.
    inline std::string as_markdown(const entity& e)
    {
        return render(markdown_generator(true, "", "md", boost::filesystem::current_path()), e);
    }

    /// A plain text generator.
    /// \returns A generator that will generate plain text.
    generator text_generator() noexcept;

    /// Renders an entity as text.
    inline std::string as_text(const entity& e)
    {
        return render(text_generator(), e);
    }

    /// An XML generator.
    ///
    /// It will use a simple XML format to describe the markup AST.
    ///
    /// \returns A generator that will generate the XML representation.
    generator xml_generator(bool include_attributes = true) noexcept;

    /// Renders an entity as XML.
    ///
    /// \returns `render(xml_generator(), e)`.
    inline std::string as_xml(const entity& e)
    {
        return render(xml_generator(), e);
    }
} // namespace markup
} // namespace standardese

#endif // STANDARDESE_MARKUP_GENERATOR_HPP_INCLUDED
