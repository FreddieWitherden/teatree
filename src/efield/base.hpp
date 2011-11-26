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

#ifndef TEATREE_EFIELD_BASE_HPP
#define TEATREE_EFIELD_BASE_HPP

#include "efield/moments/recurse.hpp"
#include "particle/typedefs.hpp"
#include "simulation/options.hpp"
#include "tree/visitor.hpp"

namespace teatree
{

/**
 * Visits particles/pseudo-particles and accumulates the electric field.
 */
template< typename DerivedT
        , typename PParticleT
        , template<typename D, typename P, typename R> class MacT
        , int MulP
        >
struct efield_base
    : public MacT<DerivedT, PParticleT, typename PParticleT::vector_type>
{
    BOOST_STATIC_ASSERT(MulP <= PParticleT::multipole_order);
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    typedef MacT<DerivedT,PParticleT,vector_type> base_type;

    efield_base(const vector_type& r, const simulation_options& so)
        : base_type(r, so)
    {}

    vector_type visit(const PParticleT& pp) const
    {
        const vector_type R = this->r_ - pp.r();

        /*
         * In 3D computing the moments requires powers of 1/|R| while
         * in 2D we require powers of 1/|R|^2.  For convenience and
         * efficiency we compute the required value here.
         */
        const scalar_type invRn = (PParticleT::dimension == 3)
                                ? 1.0/R.norm() : 1.0/R.squaredNorm();

        return efield_moments_recurse<PParticleT,MulP>::exec(pp, R, invRn);
    }
};

}

#endif // TEATREE_EFIELD_BASE_HPP
