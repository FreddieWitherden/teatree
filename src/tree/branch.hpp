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

#ifndef TEATREE_TREE_BRANCH
#define TEATREE_TREE_BRANCH

#include "utils/pool_object.hpp"
#include "utils/ipow.hpp"

#include <algorithm>

#include <boost/array.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/variant.hpp>

namespace teatree
{

/**
 * An abstract tree branch.
 */
template<typename LeafT, typename BranchT, int D>
class tree_branch : public pool_object<BranchT>
{
public: // Types & constants
    typedef LeafT   leaf_type;
    typedef BranchT branch_type;
    typedef boost::variant < leaf_type*,
                             branch_type*
                           > node_type;

    enum constants {
        dimension    = D,
        max_children = ipow<2,dimension>::value
    };

public: // Constructors & destructors
    template<typename PartFactT, typename IteratorT>
    tree_branch(PartFactT partition_factory,
                IteratorT first,
                IteratorT last,
                int level);
    virtual ~tree_branch();

public: // Visitation
    /*
     * C++ does not allow us to provide a partial-specialization of a template
     * member function inside of a template class.  However, this is required
     * for the correct handling of void visitors (those which do not return a
     * value).  We work around this by using regular function overloading
     * instead.
     */
    template<typename T> struct tag {};

    template<typename DerivedT, typename ReturnT>
    ReturnT visit_children(tree_visitor<DerivedT,branch_type,ReturnT>& tv) const
    { return visit_children(tag<ReturnT>(), tv); }

    template<typename DerivedT, typename ReturnT>
    ReturnT visit_children(const tree_visitor<DerivedT,branch_type,ReturnT>& tv) const
    { return visit_children(tag<ReturnT>(), tv); }

private:
    template<typename ReturnT, typename VisitorT>
    ReturnT visit_children(tag<ReturnT>, VisitorT& tv) const;

    template<typename VisitorT>
    void visit_children(tag<void>, VisitorT& tv) const;

protected:
    boost::array<node_type,max_children> children_;
    int num_children_;
};

template<typename LeafT, typename BranchT, int D>
template<typename PartFactT, typename IteratorT>
tree_branch<LeafT,BranchT,D>::tree_branch(
    PartFactT partition_factory,
    IteratorT first,
    IteratorT last,
    int level)
    : num_children_(0)
{
    typedef typename boost::result_of <
                                        PartFactT(IteratorT,IteratorT)
                                      >::type partition_type;

    // Create a partition object to subdivide the branch
    partition_type p(partition_factory(first, last, level));
    typename partition_type::orthant_iterator it = p.begin();


    // Make branches/leaves from these subdivisions
    for (int i = 0; i < max_children; ++i)
    {
        IteratorT lower = *(it+i), upper = *(it+i+1);

        switch (upper.base() - lower.base())
        {
        case 0:
            continue;
        case 1:
            children_[num_children_++] = &(*lower);
            break;
        default:
            children_[num_children_++] = new branch_type(partition_factory,
                                                         lower, upper, level+1);
            break;
        }
    }
}

template<typename LeafT, typename BranchT, int D>
tree_branch<LeafT,BranchT,D>::~tree_branch()
{
    /*
     * Delete our children; we do this in the reverse order that we allocated
     * them in to make the allocator's life easier.
     */
    for (int i = num_children_; --i != -1;)
        if (branch_type** b = boost::get<branch_type*>(&children_[i]))
            delete *b;
}

template<typename LeafT, typename BranchT, int D>
template<typename ReturnT, typename VisitorT> inline
ReturnT tree_branch<LeafT,BranchT,D>::visit_children
    (tag<ReturnT>, VisitorT& tv) const
{
    // Visit the our first child to get the return value
    ReturnT ret = boost::apply_visitor(tv, children_[0]);

    // Visit our remaining children applying the reduction as we go
    for (int i = 1; i < num_children_; ++i)
        tv(ret, boost::apply_visitor(tv, children_[i]));

    return ret;
}

template<typename LeafT, typename BranchT, int D>
template<typename VisitorT> inline
void tree_branch<LeafT,BranchT,D>::visit_children
    (tag<void>, VisitorT& tv) const
{
    for (int i = 0; i < num_children_; ++i)
        boost::apply_visitor(tv, children_[i]);
}

}

#endif // TEATREE_TREE_BRANCH
