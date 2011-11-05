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

#ifndef TEATREE_TREE_VISITOR
#define TEATREE_TREE_VISITOR

namespace teatree
{

/**
 * An abstract tree visitor for visiting the leaf and branch nodes of a
 *  tree.
 */
template<typename LeafT, typename BranchT>
struct tree_visitor
{
    virtual ~tree_visitor() {}

    /**
     * Called every time a branch node is encountered.  If the branch
     *  accepted then it is visited and no further visitation occurs.
     *  Otherwise, if rejected, the child nodes of the branch are
     *  visited.  Leaf nodes are always visited.
     */
    virtual bool accept(BranchT& b) = 0;

    /**
     * Called to visit a leaf node.
     */
    virtual void visit(LeafT& l) = 0;

    /**
     * Called to visit a branch node.
     */
    virtual void visit(BranchT& b) = 0;
};

}

#endif // TEATREE_TREE_VISITOR
