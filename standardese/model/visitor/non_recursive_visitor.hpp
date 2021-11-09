// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_NON_RECURSIVE_VISITOR_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_NON_RECURSIVE_VISITOR_HPP_INCLUDED

#include "visitor.hpp"

namespace standardese::model::visitor
{

template<bool is_const = false>
class non_recursive_visitor : public visitor<is_const>
{
  public:
    virtual void visit(typename visitor<is_const>::block_quote&) override;
    virtual void visit(typename visitor<is_const>::brief_section&)  override;
    virtual void visit(typename visitor<is_const>::code&)  override;
    virtual void visit(typename visitor<is_const>::code_block&)  override;
    virtual void visit(typename visitor<is_const>::details_section&)  override;
    virtual void visit(typename visitor<is_const>::emphasis&)  override;
    virtual void visit(typename visitor<is_const>::cppast_documentation&)  override;
    virtual void visit(typename visitor<is_const>::hard_break&)  override;
    virtual void visit(typename visitor<is_const>::header_file&)  override;
    virtual void visit(typename visitor<is_const>::heading&)  override;
    virtual void visit(typename visitor<is_const>::link&)  override;
    virtual void visit(typename visitor<is_const>::list&)  override;
    virtual void visit(typename visitor<is_const>::list_item&)  override;
    virtual void visit(typename visitor<is_const>::module&)  override;
    virtual void visit(typename visitor<is_const>::paragraph&)  override;
    virtual void visit(typename visitor<is_const>::section&)  override;
    virtual void visit(typename visitor<is_const>::soft_break&)  override;
    virtual void visit(typename visitor<is_const>::strong_emphasis&)  override;
    virtual void visit(typename visitor<is_const>::text&)  override;
    virtual void visit(typename visitor<is_const>::thematic_break&) override;
    virtual void visit(typename visitor<is_const>::index_document&) override;

    virtual void visit();
};

}

#endif
