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

#ifndef TEATREE_EFIELD_MOMENTS_MOMENTS_3D_HPP
#define TEATREE_EFIELD_MOMENTS_MOMENTS_3D_HPP

#include "config.h"

#include "particle/typedefs.hpp"

#include <cmath>

namespace teatree
{

template<typename PParticleT, int MulP, int Dim> struct efield_moments;

/**
 * 3D Monopole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,0,3>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const vector_type& R, scalar_type invR)
    {
        return m.M*R*invR*invR*invR;
    }
};

/**
 * 3D Dipole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,1,3>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const array_type& R, scalar_type invR)
    {
        const scalar_type invR3  = invR*invR*invR;
        const scalar_type inv3R5 = 3*(invR3*invR*invR);

        const array_type x(R), y(x.yzx()), z(x.zxy());

        return (x*x*inv3R5 - invR3)*m.Dx
             + x*inv3R5*(y*m.Dx.yzx()
                       + z*m.Dx.zxy());
    }
};

/**
 * 3D Quadrupole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,2,3>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const array_type& R, scalar_type invR)
    {
        const scalar_type inv3R5  = 3*std::pow(invR, 5);
        const scalar_type inv15R7 = 15*std::pow(invR, 7);

        const array_type x(R)   , y(x.yzx())  , z(x.zxy());
        const array_type x2(x*x), y2(x2.yzx()), z2(x2.zxy());

        return 0.5*x*((x2*inv15R7 - 3*inv3R5)*m.Qxx
                    + (y2*inv15R7 - inv3R5)*m.Qxx.yzx()
                    + (z2*inv15R7 - inv3R5)*m.Qxx.zxy())
             + y*(x2*inv15R7 - inv3R5)*m.Qxy
             + z*(x2*inv15R7 - inv3R5)*m.Qxy.yzx()
             + (x*y*z*inv15R7)*m.Qxy.zxy();
    }
};

}

#endif // TEATREE_EFIELD_MOMENTS_MOMENTS_3D_HPP
