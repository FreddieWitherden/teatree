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

#ifndef TEATREE_MAC_BASE_HPP
#define TEATREE_MAC_BASE_HPP

#include "tree/visitor.hpp"

namespace teatree
{

/**
 * Base which all MACs are derived from.
 */
template<typename DerivedT, typename PParticleT, typename ReturnT>
class mac_base : public tree_visitor<DerivedT,PParticleT,ReturnT>
{
public:
    mac_base(const typename PParticleT::vector_type& r) : r_(r) {}

protected:
    const typename PParticleT::vector_type r_;
};

}

#endif // TEATREE_MAC_BASE_HPP
