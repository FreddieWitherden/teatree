/*
    This file is part of teatree.
    Copyright (C) 2011  Freddie Witherden <freddie@witherden.org>

    teatree is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    teatree is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with teatree.  If not, see
    <http://www.gnu.org/licenses/>.
*/

#ifndef TEATREE_TREE_VISITOR_HPP
#define TEATREE_TREE_VISITOR_HPP

#include <boost/type_traits.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/variant.hpp>

namespace teatree { namespace tree_visitor_
{

using boost::disable_if;
using boost::is_void;

/**
 * A tree visitor.
 */
template<typename DerivedT, typename BranchT, typename ReturnT = void>
struct tree_visitor : public boost::static_visitor<ReturnT>
{
    ReturnT operator()(const typename BranchT::leaf_type* l)
    { return static_cast<DerivedT&>(*this).visit(*l); }

    ReturnT operator()(const typename BranchT::leaf_type* l) const
    { return static_cast<const DerivedT&>(*this).visit(*l); }

    ReturnT operator()(const BranchT* b)
    {
        DerivedT& d = static_cast<DerivedT&>(*this);

        if (d.accept(*b))
            return d.visit(*b);
        else
            return b->visit_children(d);
    }

    ReturnT operator()(const BranchT* b) const
    {
        const DerivedT& d = static_cast<const DerivedT&>(*this);

        if (d.accept(*b))
            return d.visit(*b);
        else
            return b->visit_children(d);
    }

    template<typename T>
    void operator()(T& sum, const T& val,
                    typename disable_if<is_void<T> >::type* dummy = 0) const
    { static_cast<const DerivedT&>(*this).reduce(sum, val); }

    template<typename T>
    void reduce(T& sum, const T& val) const
    { sum += val; }
};

}

using tree_visitor_::tree_visitor;

}

#endif // TEATREE_TREE_VISITOR_HPP
