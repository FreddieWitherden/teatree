/*
    This file is part of teatree.
    Copyright (C) 2011–2012  Freddie Witherden <freddie@witherden.org>

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

#ifndef TEATREE_PARTICLE_MOMENTS_SHIFT_2D_HPP
#define TEATREE_PARTICLE_MOMENTS_SHIFT_2D_HPP

#include "config.h"

namespace teatree
{

template<typename ArrayT, int Dim, int MultP> struct moments_shift;

/**
 * Shifts the dipole moment in 2D.
 */
template<typename ArrayT>
struct moments_shift<ArrayT,2,1>
{
    typedef particle_moments<ArrayT,2,1> moments_type;

    TEATREE_FLATTEN
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        p.Dx += d.Dx - r*d.M;
    }
};

/**
 * Shifts the quadrupole moment in 2D.
 */
template<typename ArrayT>
struct moments_shift<ArrayT,2,2>
{
    typedef particle_moments<ArrayT,2,2> moments_type;

    TEATREE_FLATTEN
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        p.Qxx += d.Qxx + r*(-2*d.Dx + r*d.M);
        p.Qxy += d.Qxy - r.x()*d.Dx.y() - r.y()*d.Dx.x() + r.x()*r.y()*d.M;
    }
};

/**
 * Shifts the octupole moment in 2D.
 */
template<typename ArrayT>
struct moments_shift<ArrayT,2,3>
{
    typedef particle_moments<ArrayT,2,3> moments_type;

    TEATREE_FLATTEN
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        p.Oxxx += d.Oxxx + r*(-3*d.Qxx + r*(3*d.Dx - r*d.M));
        p.Oxxy += d.Oxxy - r.yx()*d.Qxx
                + r*(-2*d.Qxy + 2*r.yx()*d.Dx + r*(d.Dx.yx()-r.yx()*d.M));
    }
};

}

#endif // TEATREE_PARTICLE_MOMENTS_SHIFT_2D_HPP
