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

#ifndef TEATREE_PARTICLE_MOMENTS_SHIFT_HPP
#define TEATREE_PARTICLE_MOMENTS_SHIFT_HPP

#include <boost/static_assert.hpp>

namespace teatree
{

/**
 * Catch all template to detect unsupported multipole orders.
 */
template<typename ScalarT, int Dim, int MultP>
struct moments_shift
{
    // Static assert
    BOOST_STATIC_ASSERT_MSG(sizeof(ScalarT) == 0, "Invalid multipole order.");
};

}

// Include the specializations for 2D and 3D
#include "particle/moments/shift_2d.hpp"
#include "particle/moments/shift_3d.hpp"

#endif // TEATREE_PARTICLE_MOMENTS_SHIFT_HPP
