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

#ifndef TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_2D_HPP
#define TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_2D_HPP

namespace teatree { namespace detail
{

/**
 * Computes the dipole moment in 2D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,2,1>
{
    typedef particle_moments<ScalarT,2,1> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT pD = ArrayT(d.Dx,d.Dy) - r*d.M;
        p.Dx += pD.x(); p.Dy += pD.y();
    }
};

/**
 * Computes the dipole and quadrupole moments in 2D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,2,2>
{
    typedef particle_moments<ScalarT,2,2> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        // Diagonal; compute Qxx and Qyy together
        const ArrayT pQ = ArrayT(d.Qxx,d.Qyy) - 2*r*ArrayT(d.Dx,d.Dy) + r*r*d.M;
        p.Qxx += pQ.x(); p.Qyy += pQ.y();

        // Off diagonal
        p.Qxy += d.Qxy - r.x()*d.Dy - r.y()*d.Dx + r.x()*r.y()*d.M;

        // Recurse to compute the dipole
        moments_recurse<ScalarT,2,1>::exec(p, d, r);
    }
};

}

}

#endif // TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_2D_HPP
