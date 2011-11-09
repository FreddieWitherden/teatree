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

#define BOOST_TEST_MODULE tree

#include <BoostTestTargetConfig.h>

#include "particle/particle.hpp"
#include "particle/make_pseudo_particle.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/uniform_real.hpp>

#include <vector>

using namespace boost::accumulators;
using namespace boost::random;
using namespace Eigen;
using namespace teatree;

// Type of particle being used for the test
typedef particle<Vector2d> particle_type;
typedef pseudo_particle<particle_type> pseudo_particle_type;

// The number of particles to use for testing
const int N = 5000;
const double TOL = 0.01;

// Accumulator type required
typedef accumulator_set<double,stats<tag::mean,tag::min,tag::max> > acc_type;


BOOST_AUTO_TEST_CASE(construction)
{
    // Random number generation
    lagged_fibonacci607 eng(31415);
    uniform_real_distribution<> xdist(-50.0, 200.0), ydist(1, 200.0);

    // Statistical accumulators
    acc_type xacc, yacc;

    // Particles
    std::vector<particle_type> p;
    p.reserve(N);

    // Create the particles and accumulate statistics
    for (int i = 0; i < N; ++i)
    {
        const double x = xdist(eng), y = ydist(eng);

        xacc(x); yacc(y);
        p.push_back(particle_type(Vector2d(x, y), Vector2d::Zero(), 1, 1));
    }

    // Create the tree
    pseudo_particle_type pp = make_pseudo_particle(p.begin(), p.end());

    BOOST_CHECK_EQUAL(pp.q(), N);
    BOOST_CHECK_EQUAL(pp.absq(), N);

    BOOST_CHECK_CLOSE(pp.r().x(), mean(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.r().y(), mean(yacc), TOL);

    BOOST_CHECK_CLOSE(pp.min().x(), min(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.min().y(), min(yacc), TOL);

    BOOST_CHECK_CLOSE(pp.max().x(), max(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.max().y(), max(yacc), TOL);
}
