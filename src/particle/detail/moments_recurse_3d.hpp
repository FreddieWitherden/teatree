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

#ifndef TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_3D_HPP
#define TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_3D_HPP

namespace teatree { namespace detail
{

/**
 * Computes the dipole moment in 3D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,3,1>
{
    typedef particle_moments<ScalarT,3,1> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT pD = ArrayT(d.Dx,d.Dy,d.Dz) - r*d.M;
        p.Dx += pD.x(); p.Dy += pD.y(); p.Dz += pD.z();
    }
};

/**
 * Computes the dipole and quadrupole moments in 3D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,3,2>
{
    typedef particle_moments<ScalarT,3,2> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        // Diagonal; compute Qxxx, Qyyy and Qzzz together
        const ArrayT pdQ = ArrayT(d.Qxx,d.Qyy,d.Qzz)
                         - 2*r*ArrayT(d.Dx,d.Dy,d.Dz)
                         + r*r*d.M;
        p.Qxx += pdQ.x(); p.Qyy += pdQ.y(); p.Qzz += pdQ.z();

        // Off diagonal
        p.Qxy += d.Qxy - r.x()*d.Dy - r.y()*d.Dx + r.x()*r.y()*d.M;
        p.Qxz += d.Qxz - r.x()*d.Dz - r.z()*d.Dx + r.x()*r.z()*d.M;
        p.Qyz += d.Qyz - r.y()*d.Dz - r.z()*d.Dy + r.y()*r.z()*d.M;

        // Recurse to compute the dipole
        moments_recurse<ScalarT,3,1>::exec(p, d, r);
    }
};

}

}

#endif // TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_3D_HPP
