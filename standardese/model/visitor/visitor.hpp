// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_VISITOR_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_VISITOR_HPP_INCLUDED

#include <type_traits>

#include "../../forward.hpp"

namespace standardese::model::visitor
{

// TODO: Should we tear this apart into two classes? Boolean template flags indicate that something is wrong.
// TODO: Should we rename the visits so that we don't overload? Might make interaction with cppyy easier.
template<bool is_const = false>
class visitor 
{
    template <typename T>
    using conditional_const = std::conditional_t<is_const, const T, T>;

  protected:

    using block_quote = conditional_const<model::markup::block_quote>;
    using code = conditional_const<model::markup::code>;
    using code_block = conditional_const<model::markup::code_block>;
    using emphasis = conditional_const<model::markup::emphasis>;
    using cpp_entity_documentation = conditional_const<model::cpp_entity_documentation>;
    using group_documentation = conditional_const<model::group_documentation>;
    using hard_break = conditional_const<model::markup::hard_break>;
    using heading = conditional_const<model::markup::heading>;
    using link = conditional_const<model::markup::link>;
    using list = conditional_const<model::markup::list>;
    using list_item = conditional_const<model::markup::list_item>;
    using module = conditional_const<model::module>;
    using paragraph = conditional_const<model::markup::paragraph>;
    using section = conditional_const<model::section>;
    using soft_break = conditional_const<model::markup::soft_break>;
    using strong_emphasis = conditional_const<model::markup::strong_emphasis>;
    using text = conditional_const<model::markup::text>;
    using thematic_break = conditional_const<model::markup::thematic_break>;
    using document = conditional_const<model::document>;
    using image = conditional_const<model::markup::image>;

  public:
    virtual void visit(block_quote&) = 0;
    virtual void visit(code&) = 0;
    virtual void visit(code_block&) = 0;
    virtual void visit(emphasis&) = 0;
    virtual void visit(cpp_entity_documentation&) = 0;
    virtual void visit(group_documentation&) = 0;
    virtual void visit(hard_break&) = 0;
    virtual void visit(heading&) = 0;
    virtual void visit(link&) = 0;
    virtual void visit(list&) = 0;
    virtual void visit(list_item&) = 0;
    virtual void visit(module&) = 0;
    virtual void visit(paragraph&) = 0;
    virtual void visit(section&) = 0;
    virtual void visit(soft_break&) = 0;
    virtual void visit(strong_emphasis&) = 0;
    virtual void visit(text&) = 0;
    virtual void visit(thematic_break&) = 0;
    virtual void visit(document&) = 0;
    virtual void visit(image&) = 0;
};

}

#endif
