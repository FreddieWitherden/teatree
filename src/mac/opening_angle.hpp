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

#ifndef TEATREE_MAC_OPENING_ANGLE_HPP
#define TEATREE_MAC_OPENING_ANGLE_HPP

#include "mac/base.hpp"
#include "particle/typedefs.hpp"
#include "utils/simulation_options.hpp"

#include <string>

namespace teatree
{

/**
 * Barnes-Hut opening angle criterion MAC.
 */
template<typename DerivedT, typename PParticleT, typename ReturnT>
class mac_opening_angle : public mac_base<DerivedT,PParticleT,ReturnT>
{
private:
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

public:
    mac_opening_angle(const vector_type& r,
                      const simulation_options& so)
        : mac_base<DerivedT,PParticleT,ReturnT>(r)
        , theta_(so.find(theta_s_)->second)
    {}

    bool accept(const PParticleT& p) const
    {
        const scalar_type d = (this->r_ - p.r()).norm();
        const scalar_type s = p.size();

        return s/d < theta_;
    }

private:
    const scalar_type theta_;
    static const std::string theta_s_;
};

template<typename D, typename P, typename R>
const std::string mac_opening_angle<D,P,R>::theta_s_("theta");

}

#endif // TEATREE_MAC_OPENING_ANGLE_HPP
