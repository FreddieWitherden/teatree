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

#ifndef TEATREE_PARTICLE_MAKE_PSEUDO_PARTICLE_HPP
#define TEATREE_PARTICLE_MAKE_PSEUDO_PARTICLE_HPP

#include "particle/partition.hpp"
#include "particle/pseudo_particle.hpp"

#include <boost/phoenix/core.hpp>
#include <boost/phoenix/object/construct.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/iterator_traits.hpp>

#include <cmath>
#include <vector>

namespace teatree { namespace make_pseudo_particle_
{

using boost::counting_iterator;
using boost::iterator_value;
using boost::phoenix::construct;
using boost::phoenix::arg_names::arg1;
using boost::phoenix::arg_names::arg2;
using boost::phoenix::arg_names::arg3;

/**
 *
 */
template<int MultP, typename EleItT, typename IdxItT> inline
pseudo_particle<typename iterator_value<EleItT>::type,MultP>
make_pseudo_particle(const EleItT begin_e, IdxItT begin_i, IdxItT end_i)
{
    // Extract the particle type from the element iterator
    typedef typename iterator_value<EleItT>::type particle_type;

    // Determine the type of partition we need to create to build the tree
    typedef particle_partition<particle_type,EleItT,IdxItT> partition_type;
    typedef typename partition_type::particle_p_iterator permutation_iterator;

    // Create the required permutation iterators
    permutation_iterator begin_p = permutation_iterator(begin_e, begin_i);
    permutation_iterator end_p   = permutation_iterator(begin_e, end_i);

    return pseudo_particle<particle_type,MultP>
        (construct<partition_type>(begin_e, arg1, arg2, arg3), begin_p, end_p);
}

/**
 *
 */
template<int MultP, typename EleItT> inline
pseudo_particle<typename iterator_value<EleItT>::type,MultP>
make_pseudo_particle(EleItT begin_e, EleItT end_e)
{
    // Create a suitable index vector (temporary)
    std::vector<int> idx(counting_iterator<int>(0),
                         counting_iterator<int>(end_e - begin_e));

    // Defer to the more general function for the actual construction
    return make_pseudo_particle<MultP>(begin_e, idx.begin(), idx.end());
}

}

using make_pseudo_particle_::make_pseudo_particle;

}

#endif // TEATREE_PARTICLE_MAKE_PSEUDO_PARTICLE_HPP
