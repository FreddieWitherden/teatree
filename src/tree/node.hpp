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

#ifndef TEATREE_TREE_NODE
#define TEATREE_TREE_NODE

#include "tree/visitor.hpp"

namespace teatree
{

/**
 * An abstract tree node.
 */
template<typename LeafT, typename BranchT, typename PartFactT>
class tree_node
{
public:
    typedef tree_visitor<LeafT,BranchT> tree_visitor_type;

public:
    virtual ~tree_node() {};

    virtual void visit(tree_visitor_type& tv) = 0;
};

}

#endif // TEATREE_TREE_NODE
