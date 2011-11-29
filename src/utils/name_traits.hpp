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

#ifndef TEATREE_UTILS_NAME_TRAITS_HPP
#define TEATREE_UTILS_NAME_TRAITS_HPP

#include <string>

namespace teatree
{

/**
 * Traits for getting the name and abbreviated name of a type T.
 */
template<typename T>
struct name_traits
{
    /// The abbreviated, or short, name of T
    static std::string abbr();
    /// The full name of T
    static std::string name();
};

}

#endif // TEATREE_UTILS_NAME_TRAITS_HPP
