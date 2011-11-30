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

#ifndef TEATREE_EFIELD_MOMENTS_RECURSE_HPP
#define TEATREE_EFIELD_MOMENTS_RECURSE_HPP

#include "config.h"

#include "efield/moments/moments_2d.hpp"
#include "efield/moments/moments_3d.hpp"
#include "particle/typedefs.hpp"

namespace teatree
{

/**
 * Template metaprogram to compute E(R) = E_{MulP}(R) + E_{MulP-1}(R)
 *  + ... + E_{0}(R) where E_{N} is the contribution of the Nth order
 *  multipole moment.
 */
template<typename PParticleT, int MulP = PParticleT::multipole_order>
struct efield_moments_recurse
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    // Type to compute the contribution of the MulP'th moment
    typedef efield_moments< pseudo_particle_type
                          , MulP
                          , pseudo_particle_type::dimension
                          > efield_moments_type;

    // Type to compute the contribution of the remaining moments
    typedef efield_moments_recurse< pseudo_particle_type
                                  , MulP - 1
                                  > efield_moments_recurse_type;

    static TEATREE_STRONG_INLINE
    vector_type exec(const pseudo_particle_type& pp,
                     const vector_type& R,
                     scalar_type invRn)
    {
        return efield_moments_type::exec(pp.moments(), R, invRn)
             + efield_moments_recurse_type::exec(pp, R, invRn);
    }
};

template<typename PParticleT>
struct efield_moments_recurse<PParticleT,0>
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    // Type to compute the monopole moment contribution
    typedef efield_moments< pseudo_particle_type
                          , 0
                          , pseudo_particle_type::dimension
                          > efield_moments_type;

    static TEATREE_STRONG_INLINE
    vector_type exec(const pseudo_particle_type& pp,
                     const vector_type& R,
                     scalar_type invRn)
    {
        return efield_moments_type::exec(pp.moments(), R, invRn);
    }
};

}

#endif // TEATREE_EFIELD_MOMENTS_RECURSE_HPP
