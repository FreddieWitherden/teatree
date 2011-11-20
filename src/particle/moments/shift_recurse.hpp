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

#ifndef TEATREE_PARTICLE_MOMENTS_SHIFT_RECURSE_HPP
#define TEATREE_PARTICLE_MOMENTS_SHIFT_RECURSE_HPP

#include "particle/moments/shift.hpp"

namespace teatree
{

template<typename ScalarT, int Dim, int MultP>
struct moments_shift_recurse
{
    typedef particle_moments<ScalarT,Dim,MultP> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& m, const moments_type& d, const ArrayT& r)
    {
        // Shift the MultP moment
        moments_shift<ScalarT,Dim,MultP>::exec(m, d, r);

        // Recurse to sort the remaining moments
        moments_shift_recurse<ScalarT,Dim,MultP-1>::exec(m, d, r);
    }
};

template<typename ScalarT, int Dim>
struct moments_shift_recurse<ScalarT,Dim,0>
{
    typedef particle_moments<ScalarT,Dim,0> moments_type;

    template<typename ArrayT>
    static void exec(moments_type&, const moments_type&, const ArrayT&)
    {
        // No-op as the monopole moment does not require shifting
    }
};

}

#endif // TEATREE_PARTICLE_MOMENTS_SHIFT_RECURSE_HPP
