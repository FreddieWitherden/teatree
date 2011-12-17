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

#ifndef TEATREE_PARTICLE_MOMENTS_SHIFT_3D_HPP
#define TEATREE_PARTICLE_MOMENTS_SHIFT_3D_HPP

#include "config.h"

namespace teatree
{

template<typename ArrayT, int Dim, int MultP> struct moments_shift;

/**
 * Shifts the dipole moment in 3D.
 */
template<typename ArrayT>
struct moments_shift<ArrayT,3,1>
{
    typedef particle_moments<ArrayT,3,1> moments_type;

    TEATREE_FLATTEN
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        p.Dx += d.Dx - r*d.M;
    }
};

/**
 * Shifts the quadrupole moment in 3D.
 */
template<typename ArrayT>
struct moments_shift<ArrayT,3,2>
{
    typedef particle_moments<ArrayT,3,2> moments_type;

    TEATREE_FLATTEN
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        p.Qxx += d.Qxx - 2*r*d.Dx + r*r*d.M;
        p.Qxy += d.Qxy - r.xxy()*d.Dx.yzz() - r.yzz()*d.Dx.xxy() + r.xxy()*r.yzz()*d.M;
    }
};

}

#endif // TEATREE_PARTICLE_MOMENTS_SHIFT_3D_HPP
