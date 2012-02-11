/*
    This file is part of teatree.
    Copyright (C) 2012  Freddie Witherden <freddie@witherden.org>

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

#ifndef TEATREE_CONSTRAINT_REFLECTIVE_HPP
#define TEATREE_CONSTRAINT_REFLECTIVE_HPP

#include "particle/typedefs.hpp"
#include "simulation/options.hpp"

#include <boost/foreach.hpp>
#include <boost/serialization/access.hpp>

namespace teatree
{

template<typename ParticleT>
class constraint_reflective
{
public: // Types
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(ParticleT);

public: // Constructors
    constraint_reflective() {}
    constraint_reflective(const simulation_options& so)
        : min_bound_(so.min_bound())
        , max_bound_(so.max_bound())
    {
        BOOST_ASSERT(min_bound_ < max_bound_);
    }

public: // Functor
    template<typename RandomRangeT>
    void operator()(RandomRangeT& ptcls)
    {
        // Loop over each particle being pushed
        BOOST_FOREACH(particle_type& p, ptcls)
        {
            // See if any of the components are out of range
            for (int i = 0; i < particle_type::dimension; ++i)
            {
                if (p.r()[i] < min_bound_ && p.v()[i] < 0.0)
                    p.v()[i] *= -1.0;
                else if (p.r()[i] > max_bound_ && p.v()[i] > 0.0)
                    p.v()[i] *= -1.0;
            }
        }
    }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/)
    {
        ar & min_bound_;
        ar & max_bound_;
    }

private: // Members
    scalar_type min_bound_;
    scalar_type max_bound_;
};

}

#endif // TEATREE_CONSTRAINT_REFLECTIVE_HPP
