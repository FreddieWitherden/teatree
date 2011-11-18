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

#define BOOST_TEST_MODULE pusher

#include <BoostTestTargetConfig.h>

#include "config.h"

#include "particle/particle.hpp"
#include "pusher/verlet.hpp"

#include <cmath>

using namespace teatree;
using namespace Eigen;

/**
 * Computes the acceleration of a small near-massless object interacting
 *  gravitationally with two larger masses -- the restricted three body
 *  problem.  In this case the two larger masses are the Earth and the
 *  Moon and the massless object a satellite.  See Hairer et al (1991,
 *  Springer) p. 130.
 */
struct arenstorf
{
    typedef particle<Vector2d> particle_type;
    typedef Vector2d vector_type;

    template<typename RandomInputRangeT, typename RandomOutputRangeT>
    void operator()(double, RandomInputRangeT& in, RandomOutputRangeT& out)
    {
        const vector_type r = in[0].r(), v = in[0].v();

        // Reduced mass for the Earth-Moon system
        const double mu = 0.012277471;
        const double mu_p = 1.0 - mu;

        const double D1 = pow(pow(r.x()+mu  , 2) + pow(r.y(), 2), 3.0/2.0);
        const double D2 = pow(pow(r.x()-mu_p, 2) + pow(r.y(), 2), 3.0/2.0);

        out[0].x() = r.x() + 2*v.y() - mu_p*(r.x()+mu)/D1 - mu*(r.x()-mu_p)/D2;
        out[0].y() = r.y() - 2*v.x() - mu_p*r.y()/D1      - mu*r.y()/D2;
    }
};

BOOST_AUTO_TEST_CASE(order)
{
    std::vector<arenstorf::particle_type> p;
    arenstorf::particle_type newp;

    /*
     * Initial conditions.  These are chosen such that the resulting
     * motion is periodic with a period of ~17.07.
     */
    p.push_back(arenstorf::particle_type(Vector2d(0.994,  0.0),
                                         Vector2d(0.0  , -2.0015851063790825),
                                         1.0, 1.0));

    // Solver
    pusher_verlet<arenstorf> verlet(p, arenstorf(), 0, 0.0001);
    verlet.advance(17.06521656015796, &newp);

    BOOST_CHECK_CLOSE(newp.r().x(), p.front().r().x(), 1.0);
    BOOST_CHECK_SMALL(newp.r().y(), 0.025);
}
