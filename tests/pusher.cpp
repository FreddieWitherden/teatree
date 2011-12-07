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
#include "pusher/comp/21.hpp"
#include "pusher/comp/43.hpp"
#include "pusher/comp/67.hpp"

#include <boost/assign/list_inserter.hpp>

using namespace boost::assign;
using namespace teatree;


#include <cmath>

using namespace Eigen;

struct kepler
{
    typedef Vector3d vector_type;
    typedef particle<Vector3d> particle_type;

    template<typename RandomInputRangeT, typename RandomOutputRangeT>
    void operator()(double, RandomInputRangeT& in, RandomOutputRangeT& out)
    {
        const double K = 2.95912208286e-4;
        std::fill(out.begin(), out.end(), Vector3d::Zero());

        for (int i = 0; i < in.size(); ++i)
        {
            for (int j = 0; j < in.size(); ++j)
            {
                if (i != j)
                {
                    const Vector3d D = in[j].r() - in[i].r();
                    out[i] += in[j].m()*D/(D.norm()*D.squaredNorm());
                }
            }

            out[i] *= K;
        }
    }
};

BOOST_AUTO_TEST_CASE(order)
{
    std::vector<kepler::particle_type> planets;
    push_back(planets)(Vector3d(-3.5023653,-3.8169847,-1.5507963),
                       Vector3d(0.00565429,-0.0041249,-0.00190589),
                       1, 0.000954786104043)
                      (Vector3d(9.0755314,-3.0458353,-1.6483708),
                       Vector3d(0.00168318,0.00483525,0.00192462),
                       1, 0.000285583733151)
                      (Vector3d(8.3101420,-16.2901086,-7.2521278),
                       Vector3d(0.00354178,0.00137102,0.00055029),
                       1, 0.0000437273164546)
                      (Vector3d(11.4707666,-25.7294829,-10.8169456),
                       Vector3d(0.00288930,0.00114527,0.00039677),
                       1, 0.0000517759138449)
                      // Pluto (so not really a planet)
                      (Vector3d(-15.5387357,-25.2225594,-3.1902382),
                       Vector3d(0.00276725,-0.00170702,-0.00136504),
                       1, 1.0/1.3e8)
                      // The Sun
                      (Vector3d::Zero(), Vector3d::Zero(), 1, 1.00000597682);

    // TODO
}
