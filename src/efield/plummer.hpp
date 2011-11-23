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

#ifndef TEATREE_EFIELD_PLUMMER_HPP
#define TEATREE_EFIELD_PLUMMER_HPP

#include "efield/base.hpp"
#include "simulation/options.hpp"

#include <boost/static_assert.hpp>

namespace teatree
{

/**
 * Computes the electric field according to the Plummer regime in which the
 *  raw Coulomb potential is softened at short distances.  The degree of
 *  softening is controlled by the parameter epsilon.  As this factor is
 *  only relevant at short distances particle-pseudo-particle interactions
 *  are unaffected.
 */
template< typename PParticleT
        , template<typename D, typename P, typename R> class MacT
        , int MulP = PParticleT::multipole_order
        >
class efield_plummer : public efield_base< efield_plummer<PParticleT,MacT,MulP>
                                         , PParticleT
                                         , MacT
                                         , MulP
                                         >
{
private: // Internal convenience typedef's
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    typedef efield_base< efield_plummer<PParticleT,MacT,MulP>
                       , PParticleT
                       , MacT
                       , MulP
                       > base_type;

public: // Methods
    efield_plummer(const vector_type& r,
            const simulation_options<scalar_type>& so)
        : base_type(r, so)
        , epsilon2_(so.epsilon()*so.epsilon())
    {}

    /*
     * The plummer approximation is only relevant at short distances;
     * for longer-range particle-pseudo particle interactions we
     * simply delegate to our base classes visit method.
     */
    using base_type::visit;

    vector_type visit(const particle_type& p) const
    {
        const vector_type R = this->r_ - p.r();
        const scalar_type Denom = R.squaredNorm() + epsilon2_;

        // 2D case; q*R/(|R|^2 + ϵ^2) field
        if (PParticleT::dimension == 2)
            return p.q()*R/Denom;
        // Otherwise 3D case; q*R/(|R|^2 + ϵ^2)^(3/2)
        else
            return p.q()*R/(Denom*std::sqrt(Denom));
    }

private: // Assertions
    BOOST_STATIC_ASSERT(PParticleT::dimension == 2
                     || PParticleT::dimension == 3);

private: // Members
    const scalar_type epsilon2_;
};

}

#endif // TEATREE_EFIELD_PLUMMER_HPP
