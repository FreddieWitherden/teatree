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

#ifndef TEATREE_TREE_LEAF
#define TEATREE_TREE_LEAF

#include "tree/node.hpp"
#include "utils/pool_object.hpp"

namespace teatree
{

/**
 *
 */
template<typename LeafT, typename BranchT>
class tree_leaf : public tree_node<LeafT,BranchT>
                , public pool_object<tree_leaf<LeafT,BranchT> >
{
public:
    typedef tree_visitor<LeafT,BranchT> tree_visitor_type;

public:
    tree_leaf(LeafT& leaf) : leaf_(leaf) {}
    ~tree_leaf() {}

    void visit(tree_visitor_type& tv) { tv.visit(leaf_); }

private:
    LeafT& leaf_;
};

}

#endif // TEATREE_TREE_LEAF
