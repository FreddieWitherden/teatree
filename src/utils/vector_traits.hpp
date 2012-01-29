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

#ifndef TEATREE_UTILS_VECTOR_TRAITS_HPP
#define TEATREE_UTILS_VECTOR_TRAITS_HPP

#include "config.h"

#include <Eigen/Core>

#include <boost/lexical_cast.hpp>
#include <boost/type_traits/is_same.hpp>

#include <string>

namespace teatree
{

template<typename ScalarT, int Dim>
struct vector_traits
{
    typedef Eigen::Matrix<ScalarT, Dim, 1> type;

    static std::string name()
    {
        return "Vector" + abbr();
    }

    static std::string abbr()
    {
        std::string ret = boost::lexical_cast<std::string>(Dim);

        if (boost::is_same<ScalarT,float>::value)
            ret += 'f';
        else if (boost::is_same<ScalarT,double>::value)
            ret += 'd';
        else
            ret += 'x';

        return ret;
    }


};

}

#endif // TEATREE_UTILS_VECTOR_TRAITS_HPP
