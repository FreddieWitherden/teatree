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

#include "tree/node.hpp"
#include "tree/leaf.hpp"
#include "utils/pool_object.hpp"
#include "utils/ipow.hpp"

#include <boost/array.hpp>

#include <algorithm>

#include <boost/utility/result_of.hpp>

namespace teatree { namespace tree_branch_
{

using boost::array;
using boost::result_of;

/**
 * An abstract tree branch.
 */
template<typename LeafT, typename BranchT, int D>
class tree_branch : public tree_node<LeafT,BranchT>
                  , public pool_object<BranchT>
{
public: // Types & constants
    typedef tree_node<LeafT,BranchT> node_type;
    typedef tree_leaf<LeafT,BranchT> leaf_type;
    typedef BranchT                            branch_type;
    typedef tree_visitor<LeafT,BranchT>        visitor_type;

    enum constants {
        dimension    = D,
        max_children = ipow<2,dimension>::value
    };

public: // Constructors & destructors
    template<typename PartFactT, typename IteratorT>
    tree_branch(PartFactT partition_factory,
                IteratorT first,
                IteratorT last);
    ~tree_branch();

public: // Visitation
    void visit_children(visitor_type& tv);

private: // Helpers
    /**
     * Depending on the distance between first and last this function
     *  creates either a branch, leaf, or NULL node.
     */
    template<typename PartFactT, typename IteratorT> static
    node_type* make_tree_node(PartFactT partition_factory,
                              IteratorT first,
                              IteratorT last);

protected:
    array<node_type*,max_children> children_;
    int num_children_;
};

template<typename LeafT, typename BranchT, int D>
template<typename PartFactT, typename IteratorT>
tree_branch<LeafT,BranchT,D>::tree_branch(
    PartFactT partition_factory,
    IteratorT first,
    IteratorT last)
    : num_children_(0)
{
    typedef typename result_of<PartFactT(IteratorT,IteratorT)>::type
                     partition_type;

    // Create a partition object to subdivide the branch
    partition_type p(partition_factory(first, last));
    typename partition_type::orthant_iterator it = p.begin();


    // Make branches/leaves from these subdivisions
    for (int i = 0; i < max_children; ++i)
    {
        node_type* n = make_tree_node(partition_factory, *(it+i), *(it+i+1));
        if (n) children_[num_children_++] = n;
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
        delete children_[i];
}

template<typename LeafT, typename BranchT, int D> inline
void tree_branch<LeafT,BranchT,D>::visit_children(visitor_type& tv)
{
    for (int i = 0; i < num_children_; ++i)
        children_[i]->visit(tv);
}

template<typename LeafT, typename BranchT, int D>
template<typename PartFactT, typename IteratorT> inline
typename tree_branch<LeafT,BranchT,D>::node_type*
tree_branch<LeafT,BranchT,D>::make_tree_node(
    PartFactT partition_factory,
    IteratorT first,
    IteratorT last)
{
    switch (last.base() - first.base())
    {
    case 0:
        return static_cast<node_type*>(0);
    case 1:
        return new leaf_type(*first);
    default:
        return new branch_type(partition_factory, first, last);
    }
}

}

using tree_branch_::tree_branch;

}

#endif // TEATREE_TREE_BRANCH
