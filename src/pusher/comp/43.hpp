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

#ifndef TEATREE_PUSHER_COMP_43_HPP
#define TEATREE_PUSHER_COMP_43_HPP

#include "pusher/comp/base.hpp"

#include <string>

namespace teatree
{

/**
 * Coefficients for the 43 composition method.
 */
template<typename ScalarT>
struct pusher_comp_43_coeffs
{
    enum { N = 2 };
    static boost::array<ScalarT,N> coeffs()
    {
        boost::array<ScalarT,N> c;
        c[0] =  1.3512071919596576340476878;
        c[1] = -1.7024143839193152680953756;
        return c;
    }
};

TEATREE_PUSHER_GENERATE_COMP(pusher_comp_43, pusher_comp_43_coeffs);

// Traits
template<typename A>
struct name_traits<pusher_comp_43<A> >
{
    static std::string abbr() { return "C43"; }
    static std::string name() { return "Composition (order = 4; evals = 3)"; }
};

}

#endif // TEATREE_PUSHER_COMP_43_HPP
