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

#ifndef TEATREE_PARTICLE_MOMENTS_MOMENTS_HPP
#define TEATREE_PARTICLE_MOMENTS_MOMENTS_HPP

#include <boost/static_assert.hpp>

namespace teatree
{

template<typename ArrayT, int Dim, int MultP>
struct particle_moments
{
    BOOST_STATIC_ASSERT_MSG(sizeof(ArrayT) == 0, "Invalid multipole order");
};

/// Monopole
template<typename ArrayT, int Dim>
struct particle_moments<ArrayT,Dim,0>
{
    typename ArrayT::Scalar M;

    particle_moments() : M(0.0) {}
};

/// 2D Dipole
template<typename ArrayT>
struct particle_moments<ArrayT,2,1> : public particle_moments<ArrayT,2,0>
{
    ArrayT Dx;

    particle_moments() : Dx(ArrayT::Zero()) {}
};

/// 3D Dipole
template<typename ArrayT>
struct particle_moments<ArrayT,3,1> : public particle_moments<ArrayT,3,0>
{
    ArrayT Dx;

    particle_moments() : Dx(ArrayT::Zero()) {}

};

/// 2D Quadrupole
template<typename ArrayT>
struct particle_moments<ArrayT,2,2> : public particle_moments<ArrayT,2,1>
{
    ArrayT Qxx;
    typename ArrayT::Scalar Qxy;

    particle_moments() : Qxx(ArrayT::Zero()), Qxy(0.0) {}
};

/// 3D Quadrupole
template<typename ArrayT>
struct particle_moments<ArrayT,3,2> : public particle_moments<ArrayT,3,1>
{
    ArrayT Qxx, Qxy;

    particle_moments()
        : Qxx(ArrayT::Zero()), Qxy(ArrayT::Zero())
    {}
};

/// 2D Octupole
template<typename ArrayT>
struct particle_moments<ArrayT,2,3> : public particle_moments<ArrayT,2,2>
{
    ArrayT Oxxx, Oxxy;

    particle_moments() : Oxxx(ArrayT::Zero()), Oxxy(ArrayT::Zero()) {}
};

/// 3D Octupole
template<typename ArrayT>
struct particle_moments<ArrayT,3,3> : public particle_moments<ArrayT,3,2>
{
    ArrayT Oxxx, Oxxy, Oxzz;
    typename ArrayT::Scalar Oxyz;

    particle_moments()
        : Oxxx(ArrayT::Zero()), Oxxy(ArrayT::Zero())
        , Oxzz(ArrayT::Zero()), Oxyz(0.0)
    {}
};

}

#endif // TEATREE_PARTICLE_MOMENTS_MOMENTS_HPP
