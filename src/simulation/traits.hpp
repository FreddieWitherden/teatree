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

#ifndef TEATREE_SIMULATION_TRAITS_HPP
#define TEATREE_SIMULATION_TRAITS_HPP

#include "particle/moments/traits.hpp"
#include "utils/name_traits.hpp"
#include "utils/vector_traits.hpp"

#include <ostream>

namespace teatree
{

/**
 * Reports information about a simulation type such as its short-name
 *  and a description of what it simulates.
 */
template<typename SimT>
struct simulation_traits
{
    typedef vector_traits< typename SimT::scalar_type
                         , SimT::dimension>         vector_traits_type;
    typedef moments_traits<SimT::multipole_order>   moments_traits_type;
    typedef name_traits<typename SimT::mac_type>    mac_traits_type;
    typedef name_traits<typename SimT::efield_type> efield_traits_type;
    typedef name_traits<typename SimT::pusher_type> pusher_traits_type;

    static std::string type()
    {
        std::ostringstream os;
        os << name_traits<SimT>::abbr()
           << vector_traits_type::abbr()
           << moments_traits_type::abbr()
           << mac_traits_type::abbr()
           << efield_traits_type::abbr()
           << pusher_traits_type::abbr();
        return os.str();
    }

    static std::string desc()
    {
        std::ostringstream os;
        os << "  Type: "              << type()
           << "\n  Boundaries: "      << name_traits<SimT>::name()
           << "\n  Vector type: "     << vector_traits_type::name()
           << "\n  Moments: "         << moments_traits_type::name()
           << "\n  Mac: "             << mac_traits_type::name()
           << "\n  Field: "           << efield_traits_type::name()
           << "\n  Particle pusher: " << pusher_traits_type::name()
           << "\n";
        return os.str();
    }
};

}

#endif // TEATREE_SIMULATION_TRAITS_HPP
