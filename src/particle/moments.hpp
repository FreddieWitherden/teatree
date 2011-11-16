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

#ifndef TEATREE_PARTICLE_MOMENTS_HPP
#define TEATREE_PARTICLE_MOMENTS_HPP

#include <boost/static_assert.hpp>

namespace teatree
{

template<typename ScalarT, int Dim, int MultP>
struct particle_moments
{
    BOOST_STATIC_ASSERT_MSG(sizeof(ScalarT) == 0, "Invalid multipole order");
};

/// Monopole
template<typename ScalarT, int Dim>
struct particle_moments<ScalarT,Dim,0>
{
    ScalarT M;

    particle_moments() : M(0.0) {}
};

/// 2D Dipole
template<typename ScalarT>
struct particle_moments<ScalarT,2,1> : public particle_moments<ScalarT,2,0>
{
    ScalarT Dx, Dy;

    particle_moments() : Dx(0.0), Dy(0.0) {}
};

/// 3D Dipole
template<typename ScalarT>
struct particle_moments<ScalarT,3,1> : public particle_moments<ScalarT,3,0>
{
    ScalarT Dx, Dy, Dz;

    particle_moments() : Dx(0.0), Dy(0.0), Dz(0.0) {}

};

/// 2D Quadrupole
template<typename ScalarT>
struct particle_moments<ScalarT,2,2> : public particle_moments<ScalarT,2,1>
{
    ScalarT Qxx, Qxy;
    ScalarT      Qyy;

    particle_moments() : Qxx(0.0), Qxy(0.0), Qyy(0.0) {}
};

/// 3D Quadrupole
template<typename ScalarT>
struct particle_moments<ScalarT,3,2> : public particle_moments<ScalarT,3,1>
{
    ScalarT Qxx, Qxy, Qxz;
    ScalarT      Qyy, Qyz;
    ScalarT           Qzz;

    particle_moments()
        : Qxx(0.0), Qxy(0.0), Qxz(0.0), Qyy(0.0), Qyz(0.0), Qzz(0.0)
    {}
};

/// 2D Octopole
template<typename ScalarT>
struct particle_moments<ScalarT,2,3> : public particle_moments<ScalarT,2,2>
{
    ScalarT Oxxx, Oyyy;
    ScalarT Oxxy, Oxyy;

    particle_moments() : Oxxx(0.0), Oyyy(0.0), Oxxy(0.0), Oxyy(0.0) {}
};

/// 3D Octopole
template<typename ScalarT>
struct particle_moments<ScalarT,3,3> : public particle_moments<ScalarT,3,2>
{
    ScalarT Oxxx, Oyyy, Ozzz;
    ScalarT Oxyy, Oxzz, Oxyz;
    ScalarT Oxxz, Oxxy;
    ScalarT Oyyz, Oyzz;

    particle_moments()
        : Oxxx(0.0), Oyyy(0.0), Ozzz(0.0), Oxyy(0.0), Oxzz(0.0)
        , Oxyz(0.0), Oxxz(0.0), Oxxy(0.0), Oyyz(0.0), Oyzz(0.0)
    {}
};

}

#endif // TEATREE_PARTICLE_MOMENTS_HPP
