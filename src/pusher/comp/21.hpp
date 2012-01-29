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

#ifndef TEATREE_PUSHER_COMP_21_HPP
#define TEATREE_PUSHER_COMP_21_HPP

#include "pusher/comp/base.hpp"

#include <string>

namespace teatree
{

/**
 * Coefficients for the 21 composition method.
 */
template<typename ScalarT>
struct pusher_comp_21_coeffs
{
    enum { N = 1 };
    static boost::array<ScalarT,N> coeffs()
    {
        boost::array<ScalarT,N> c = {{ 1.0 }}; return c;
    }
};

TEATREE_PUSHER_GENERATE_COMP(pusher_comp_21, pusher_comp_21_coeffs);

// Traits
template<typename A>
struct name_traits<pusher_comp_21<A> >
{
    static std::string abbr() { return "C21"; }
    static std::string name() { return "Störmer-Verlet (order = 2; evals = 1)"; }
};

}

#endif // TEATREE_PUSHER_COMP_21_HPP
