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

#ifndef TEATREE_UTILS_SERIALISE_EIGEN_HPP
#define TEATREE_UTILS_SERIALISE_EIGEN_HPP

#include <Eigen/Core>

namespace boost { namespace serialization
{

/**
 * Allows for serialisation of any fixed-sized Eigen::Matrix type.
 */
template<typename ArchiveT, typename ScalarT,
         int Rows, int Cols>
void serialize(ArchiveT& ar, Eigen::Matrix<ScalarT,Rows,Cols>& m,
               unsigned int /*version*/)
{
    for (int i = 0; i < Rows; ++i)
        for (int j = 0; j < Cols; ++j)
            ar & m(i,j);
}

}
}

#endif // TEATREE_UTILS_SERIALISE_EIGEN_HPP
