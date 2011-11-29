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

#ifndef TEATREE_PARTICLE_MOMENTS_TRAITS_HPP
#define TEATREE_PARTICLE_MOMENTS_TRAITS_HPP

#include <string>

namespace teatree
{

template<int MulP>
struct moments_traits
{
    static std::string abbr();
    static std::string name();
};

/// Monopole traits
template<> struct moments_traits<0>
{
    static std::string abbr() { return "M"; }
    static std::string name() { return "Monopole"; }
};

/// Dipole traits
template<> struct moments_traits<1>
{
    static std::string abbr() { return "D"; }
    static std::string name() { return "Dipole"; }
};

/// Dipole traits
template<> struct moments_traits<2>
{
    static std::string abbr() { return "Q"; }
    static std::string name() { return "Quadrupole"; }
};

/// Octupole traits
template<> struct moments_traits<3>
{
    static std::string abbr() { return "O"; }
    static std::string name() { return "Octupole"; }
};

}

#endif // TEATREE_PARTICLE_MOMENTS_TRAITS_HPP
