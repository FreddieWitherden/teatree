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

#ifndef TEATREE_MAC_MIN_DIST_HPP
#define TEATREE_MAC_MIN_DIST_HPP

#include "mac/base.hpp"
#include "particle/typedefs.hpp"
#include "simulation/options.hpp"
#include "utils/name_traits.hpp"

#include <string>

namespace teatree
{

/**
 * Salman-Warren minimum distance MAC as described in "Skeletons from
 *  the treecode closet".
 */
template<typename DerivedT, typename PParticleT, typename ReturnT>
class mac_min_dist : public mac_base<DerivedT,PParticleT,ReturnT>
{
private:
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

public:
    mac_min_dist(const vector_type& r,
                 const simulation_options& so)
        : mac_base<DerivedT,PParticleT,ReturnT>(r)
        , theta2_(so.theta()*so.theta())
    {}

    bool accept(const PParticleT& p) const
    {
        const array_type u = p.min().min(this->r_.array());
        const array_type v = p.max().max(this->r_.array());

        const scalar_type d2 = (p.min()-u).matrix().squaredNorm()
                             + (v-p.max()).matrix().squaredNorm();
        const scalar_type s2 = p.size2();

        return s2 < d2*theta2_;
    }

private:
    const scalar_type theta2_;
};

// Traits
template<typename D, typename P, typename R>
struct name_traits<mac_min_dist<D,P,R> >
{
    static std::string abbr() { return "M"; }
    static std::string name() { return "Salmon-Warren minimum distance"; }
};

}

#endif // TEATREE_MAC_MIN_DIST_HPP
