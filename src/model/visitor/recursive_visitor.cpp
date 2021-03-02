// Copyright (C entity) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "../../../standardese/model/visitor/recursive_visitor.hpp"
#include "../../../standardese/model/markup/block_quote.hpp"
#include "../../../standardese/model/markup/code.hpp"
#include "../../../standardese/model/markup/code_block.hpp"
#include "../../../standardese/model/section.hpp"
#include "../../../standardese/model/markup/emphasis.hpp"
#include "../../../standardese/model/cpp_entity_documentation.hpp"
#include "../../../standardese/model/markup/hard_break.hpp"
#include "../../../standardese/model/markup/heading.hpp"
#include "../../../standardese/model/markup/link.hpp"
#include "../../../standardese/model/markup/list_item.hpp"
#include "../../../standardese/model/module.hpp"
#include "../../../standardese/model/markup/list.hpp"
#include "../../../standardese/model/markup/paragraph.hpp"
#include "../../../standardese/model/markup/soft_break.hpp"
#include "../../../standardese/model/markup/strong_emphasis.hpp"
#include "../../../standardese/model/markup/text.hpp"
#include "../../../standardese/model/markup/thematic_break.hpp"
#include "../../../standardese/model/document.hpp"
#include "../../../standardese/model/markup/image.hpp"

#include <stdexcept>

namespace standardese::model::visitor
{

namespace {
template <typename T, typename S>
void visit_children(T&& parent, S&& visitor)
{
    for (auto& child : parent) child.accept(visitor);
}

}

template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::block_quote& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::code& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::code_block& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::section& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::emphasis& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::cpp_entity_documentation& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::hard_break& entity) {}
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::heading& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::link& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::list_item& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::module& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::list& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::paragraph& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::soft_break& entity) {}
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::strong_emphasis& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::text& entity) {}
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::thematic_break& entity) {}
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::document& entity) { visit_children(entity, *this); }
template<bool is_const> void recursive_visitor<is_const>::visit(typename visitor<is_const>::image& entity) { visit_children(entity, *this); }

}

namespace standardese::model::visitor
{
template class recursive_visitor<true>;
template class recursive_visitor<false>;
}

