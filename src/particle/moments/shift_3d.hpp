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

template<typename ScalarT, int Dim, int MultP> struct moments_shift;

/**
 * Shifts the dipole moment in 3D.
 */
template<typename ScalarT>
struct moments_shift<ScalarT,3,1>
{
    typedef particle_moments<ScalarT,3,1> moments_type;

    template<typename ArrayT> TEATREE_STRONG_INLINE
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT pD = ArrayT(d.Dx,d.Dy,d.Dz) - r*d.M;
        p.Dx += pD.x(); p.Dy += pD.y(); p.Dz += pD.z();
    }
};

/**
 * Shifts the quadrupole moment in 3D.
 */
template<typename ScalarT>
struct moments_shift<ScalarT,3,2>
{
    typedef particle_moments<ScalarT,3,2> moments_type;

    template<typename ArrayT> TEATREE_STRONG_INLINE
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT D(d.Dx,d.Dy,d.Dz);

        // Diagonal
        const ArrayT pQd = ArrayT(d.Qxx,d.Qyy,d.Qzz) - 2*r*D + r*r*d.M;
        p.Qxx += pQd.x(); p.Qyy += pQd.y(); p.Qzz += pQd.z();

        // Off diagonal
        const ArrayT pQod = ArrayT(d.Qxy,d.Qxz,d.Qyz) - r.xxy()*D.yzz()
                          - r.yzz()*D.xxy() + r.xxy()*r.yzz()*d.M;
        p.Qxy += pQod.x(); p.Qxz += pQod.y(); p.Qyz += pQod.z();
    }
};

}

#endif // TEATREE_PARTICLE_MOMENTS_SHIFT_3D_HPP
