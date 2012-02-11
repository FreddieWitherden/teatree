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

#ifndef TEATREE_CONSTRAINT_OPEN_HPP
#define TEATREE_CONSTRAINT_OPEN_HPP

#include "simulation/options.hpp"

#include <boost/serialization/access.hpp>

namespace teatree
{

template<typename ParticleT>
class constraint_open
{
public: // Constructors
    constraint_open() {}
    constraint_open(const simulation_options&) {}

public: // Functor
    template<typename RandomRangeT>
    void operator()(RandomRangeT&)
    {
        // No op
    }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT&, unsigned /*file version*/)
    {}
};

}

#endif // TEATREE_CONSTRAINT_OPEN_HPP
