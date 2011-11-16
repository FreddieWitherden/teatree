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

#ifndef TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_HPP
#define TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_HPP

#include <boost/static_assert.hpp>

namespace teatree { namespace detail
{

/**
 * Catch all template to detect unsupported multipole orders.
 */
template<typename ScalarT, int Dim, int MultP>
struct moments_recurse
{
    // Static assert
    BOOST_STATIC_ASSERT_MSG(sizeof(ScalarT) == 0, "Invalid multipole order.");
};

/**
 * Monopole moment (net charge) in either 2D or 3D.  This is a no-op as the
 *  net charge is computed by pseudo_particle_visitor.
 */
template<typename ScalarT, int Dim>
struct moments_recurse<ScalarT,Dim,0>
{
    typedef particle_moments<ScalarT,Dim,0> moments_type;

    template<typename ArrayT>
    static void exec(moments_type&, const moments_type&, const ArrayT&) {}
};

}

}

// Include the specializations for 2D and 3D
#include "particle/detail/moments_recurse_2d.hpp"
#include "particle/detail/moments_recurse_3d.hpp"

#endif // TEATREE_PARTICLE_DETAIL_MOMENTS_RECURSE_HPP
