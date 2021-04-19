// Copyright (C) 2016-2019 Jonathan Müller <jonathanmueller.dev@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_COMMENT_HPP_INCLUDED
#define STANDARDESE_COMMENT_HPP_INCLUDED

#include <mutex>
#include <unordered_map>

#include <cppast/forward.hpp>
#include <stdexcept>
#include <cassert>

#include <type_safe/optional_ref.hpp>

#include "comment/doc_comment.hpp"
#include "comment/config.hpp"
#include "comment/parser.hpp"

namespace standardese
{
/// A registry of the comments for all entities.
class comment_registry
{
public:
    /// \effects Registers everything from the other comment registry.
    void merge(comment_registry&& other);

    /// \effects Registers the comment for the given entity.
    /// \returns Whether or not a comment was registered already.
    /// \notes It will merge multiple comments where appropriate.
    bool register_comment(type_safe::object_ref<const cppast::cpp_entity> entity,
                          comment::doc_comment                            comment);

    /// \effects Registers the comment for the given module.
    /// \returns Whether or not a comment was registered already.
    /// \notes It will not merge multiple comments.
    bool register_comment(std::string module_name, comment::doc_comment comment);

    /// \returns The comment of an entity, if there is any.
    type_safe::optional_ref<const comment::doc_comment> get_comment(
        const cppast::cpp_entity& e) const;

    /// \returns The comment of a module, if there is any.
    type_safe::optional_ref<const comment::doc_comment> get_comment(
        const std::string& module_name) const;

    /// \effects Adds an entity to the group of the given name.
    void add_to_group(std::string name, type_safe::object_ref<const cppast::cpp_entity> entity)
    {
        groups_[std::move(name)].push_back(entity);
    }

    /// \returns All the entities belonging to the given group.
    auto lookup_group(const std::string& name) const
        -> type_safe::array_ref<const type_safe::object_ref<const cppast::cpp_entity>>
    {
        auto iter = groups_.find(name);
        if (iter == groups_.end())
            return nullptr;
        return type_safe::ref(iter->second.data(), iter->second.size());
    }

private:
    std::unordered_map<const cppast::cpp_entity*, comment::doc_comment> map_;
    std::unordered_map<std::string, std::vector<type_safe::object_ref<const cppast::cpp_entity>>>
                                                          groups_;
    std::unordered_map<std::string, comment::doc_comment> modules_;
};

/// \returns The unique name of the given entity.
std::string lookup_unique_name(const comment_registry& registry, const cppast::cpp_entity& e);

/// Parses the comments in several files and connects them in a shared registry.
class file_comment_parser
{
public:
    explicit file_comment_parser(type_safe::object_ref<const cppast::diagnostic_logger> logger,
                                 comment::config config = comment::config())
    : config_(std::move(config)), logger_(logger)
    {}

    /// Parse all comments in `file`.
    /// \notes This function is thread-safe.
    void parse(type_safe::object_ref<const cppast::cpp_file> file) const;

    /// Create a registry from this parser.
    /// \returns The registry containing all registered comments.
    /// \requires This function must only be called once,
    /// and you must not call `parse()` afterwards.
    comment_registry finish();

private:
    /// Connect free comments with an `\entity` command to their respective entities.
    /// \notes This function is not thread-safe.
    void resolve_free_comments();

    /// Group uncommented entities with preceding commented entities.
    /// \notes This function is not thread-safe.
    void group_uncommented();

    bool register_commented(type_safe::object_ref<const cppast::cpp_entity> entity,
                            comment::doc_comment comment, bool allow_cmd = true) const;

    void register_uncommented(type_safe::object_ref<const cppast::cpp_entity> entity) const;

    type_safe::optional_ref<const comment::doc_comment> get_comment(
        const cppast::cpp_entity& e) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return registry_.get_comment(e);
    }

    std::string get_parent_unique_name(const cppast::cpp_entity& e) const;

    mutable std::mutex                                                      mutex_;
    mutable std::unordered_multimap<std::string, const cppast::cpp_entity*> uncommented_;
    mutable comment_registry                                                registry_;
    mutable std::vector<comment::parse_result>                              free_comments_;

    comment::config                                        config_;
    type_safe::object_ref<const cppast::diagnostic_logger> logger_;
};
} // namespace standardese

#endif // STANDARDESE_COMMENT_HPP_INCLUDED
