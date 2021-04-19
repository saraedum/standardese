// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_COMMENT_MATCHING_ENTITY_HPP_INCLUDED
#define STANDARDESE_COMMENT_MATCHING_ENTITY_HPP_INCLUDED

#include <string>

#include <type_safe/variant.hpp>

#include "../forward.hpp"

namespace standardese
{
namespace comment
{
    //// Tag type to indicate that a comment belongs to the current file.
    struct current_file
    {};

    /// Tag type to indicate that a comment belongs to the entity of the given name.
    struct remote_entity
    {
        std::string unique_name;

        explicit remote_entity(std::string name)
        {
            unique_name.reserve(name.size());
            for (auto c : name)
                if (c != ' ')
                    unique_name += c;
        }
    };

    /// Tag type to indicate that a comment belongs to a (template) parameter of given name.
    struct inline_param
    {
        std::string unique_name;

        explicit inline_param(std::string name) : unique_name(std::move(name)) {}

        friend bool operator==(const inline_param& lhs, const inline_param& rhs)
        {
            return lhs.unique_name == rhs.unique_name;
        }
        friend bool operator!=(const inline_param& lhs, const inline_param& rhs)
        {
            return !(rhs == lhs);
        }
    };

    /// Tag type to indicate that a comment belongs to a base of given name.
    struct inline_base
    {
        std::string unique_name;

        explicit inline_base(std::string name) : unique_name(std::move(name)) {}

        friend bool operator==(const inline_base& lhs, const inline_base& rhs)
        {
            return lhs.unique_name == rhs.unique_name;
        }
        friend bool operator!=(const inline_base& lhs, const inline_base& rhs)
        {
            return !(rhs == lhs);
        }
    };

    /// Tag type to indicate that a comment belongs to the module of the given name.
    struct module
    {
        std::string name;

        explicit module(std::string name) : name(std::move(name)) {}
    };

    /// The entity a comment belongs to.
    ///
    /// If it is empty, it belongs to the associated entity of the comment.
    using matching_entity = type_safe::variant<type_safe::nullvar_t, current_file, remote_entity,
                                               inline_param, inline_base, module>;

    /// \returns Whether or not the comment belongs to a separate entity altogether.
    /// If it is not remote it belongs to the entity the comment text was associated with.
    inline bool is_remote(const matching_entity& entity) noexcept
    {
        return entity.has_value();
    }

    /// \returns Whether or not the comment belongs to the current file.
    inline bool is_file(const matching_entity& entity) noexcept
    {
        return entity.has_value(type_safe::variant_type<current_file>{});
    }

    /// \returns The unique name of the remote entity it refers to, if any.
    inline type_safe::optional<std::string> get_remote_entity(const matching_entity& entity)
    {
        return type_safe::copy(entity.optional_value(type_safe::variant_type<remote_entity>{})
                                   .map(&remote_entity::unique_name));
    }

    /// \returns The unique name of the inline entity it refers to, if any.
    /// \group get_inline Get inline entity
    inline type_safe::optional<std::string> get_inline_param(const matching_entity& entity)
    {
        return type_safe::copy(entity.optional_value(type_safe::variant_type<inline_param>{})
                                   .map(&inline_param::unique_name));
    }

    /// \group get_inline
    inline type_safe::optional<std::string> get_inline_base(const matching_entity& entity)
    {
        return type_safe::copy(entity.optional_value(type_safe::variant_type<inline_base>{})
                                   .map(&inline_base::unique_name));
    }

    /// \returns The name of the module it refers to, if any.
    inline type_safe::optional<std::string> get_module(const matching_entity& entity)
    {
        return type_safe::copy(
            entity.optional_value(type_safe::variant_type<module>{}).map(&module::name));
    }
} // namespace comment
} // namespace standardese

#endif // STANDARDESE_COMMENT_MATCHING_ENTITY_HPP_INCLUDED
