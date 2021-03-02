// Copyright (C) 2021 Julian Rüth <julian.rueth@fsfe.org>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#ifndef STANDARDESE_MODEL_VISITOR_GENERIC_VISITOR_HPP_INCLUDED
#define STANDARDESE_MODEL_VISITOR_GENERIC_VISITOR_HPP_INCLUDED

#include "visitor.hpp"

#include "../markup/block_quote.hpp"
#include "../markup/code.hpp"
#include "../markup/code_block.hpp"
#include "../section.hpp"
#include "../markup/emphasis.hpp"
#include "../cpp_entity_documentation.hpp"
#include "../markup/hard_break.hpp"
#include "../markup/heading.hpp"
#include "../markup/link.hpp"
#include "../markup/list_item.hpp"
#include "../module.hpp"
#include "../markup/list.hpp"
#include "../markup/paragraph.hpp"
#include "../markup/soft_break.hpp"
#include "../markup/strong_emphasis.hpp"
#include "../markup/text.hpp"
#include "../markup/thematic_break.hpp"
#include "../markup/image.hpp"
#include "../document.hpp"

namespace standardese::model::visitor {

// TODO: Maybe we should call `visit` of the base class once we renamed all the visit to visit_*.
template <typename T, typename Base = visitor<true>>
class generic_visitor : public Base {
  public:
    virtual void visit(typename Base::block_quote& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::code& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::code_block& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::emphasis& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::cpp_entity_documentation& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::hard_break& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::heading& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::link& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::list& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::list_item& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::module& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::paragraph& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::section& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::soft_break& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::strong_emphasis& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::text& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::thematic_break& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::document& e) { static_cast<T&>(*this)(e); }
    virtual void visit(typename Base::image& e) { static_cast<T&>(*this)(e); }
};

}

#endif
