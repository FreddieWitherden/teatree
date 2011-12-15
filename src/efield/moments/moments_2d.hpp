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

#ifndef TEATREE_EFIELD_MOMENTS_MOMENTS_2D_HPP
#define TEATREE_EFIELD_MOMENTS_MOMENTS_2D_HPP

#include "config.h"

#include "particle/typedefs.hpp"

namespace teatree
{

template<typename PParticleT, int MulP, int Dim> struct efield_moments;

/**
 * 2D Monopole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,0,2>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const vector_type& R, scalar_type invR2)
    {
        return m.M*R*invR2;
    }
};

/**
 * 2D Dipole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,1,2>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const vector_type& R, scalar_type invR2)
    {
        const scalar_type invR4 = invR2*invR2;

        const array_type Dx(m.Dx,m.Dy) , Dy(Dx.yx());
        const array_type x(R)          , y(x.yx());

        return ((x*x-y*y)*Dx + 2*x*y*Dy)*invR4;
    }
};

/**
 * 2D Quadrupole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,2,2>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const vector_type& R, scalar_type invR2)
    {
        const scalar_type invR6 = invR2*invR2*invR2;

        const array_type Qxx(m.Qxx,m.Qyy), Qyy(Qxx.yx()), Qxy(m.Qxy,m.Qxy);
        const array_type x(R)            , y(x.yx());
        const array_type x2(x*x)         , y2(x2.yx());

        return (x*(x2-3*y2)*(Qxx-Qyy) + y*Qxy*(6*x2-2*y2))*invR6;
    }
};

/**
 * 2D Octupole field.
 */
template<typename PParticleT>
struct efield_moments<PParticleT,3,2>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    static TEATREE_FLATTEN
    vector_type exec(const particle_moments_type& m,
                     const vector_type& R, scalar_type invR2)
    {
        const scalar_type invR8 = invR2*invR2*invR2*invR2;

        const array_type Oxxx(m.Oxxx,m.Oyyy), Oyyy(Oxxx.yx());
        const array_type Oxxy(m.Oxxy,m.Oxyy), Oxyy(Oxxy.yx());
        const array_type x(R)               , y(x.yx());
        const array_type x2(x*x)            , y2(x2.yx());

        return ((Oxxx-3*Oxyy)*(y2-2*x*y-x2)*(y2+2*x*y-x2)
             + 4*x*y*(y2-x2)*(Oyyy-3*Oxxy))*invR8;
    }
};

}

#endif // TEATREE_EFIELD_MOMENTS_MOMENTS_2D_HPP
