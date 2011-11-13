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

/**
 * A simple visitor to count the number of particles in the tree.
 */
struct counting_visitor
    : public tree_visitor<counting_visitor,pseudo_particle_type,int>
{
    bool accept(const pseudo_particle_type&) const { return false; }
    int visit(const pseudo_particle_type&)   const { return 0; }
    int visit(const particle_type&)          const { return 1; }
};

/**
 * Walks the leaves of a tree computing the minimum position as it goes.
 *  This makes use of a custom reduction operator.
 */
struct min_visitor : public tree_visitor<min_visitor,
                                         pseudo_particle_type,
                                         particle_type::array_type>
{
    typedef particle_type::array_type array_type;

    bool accept(const pseudo_particle_type&)      const { return false; }
    array_type visit(const pseudo_particle_type&) const { return array_type(); }
    array_type visit(const particle_type& p)      const { return p.r(); }
    void reduce(array_type& m, const array_type& val) const { m = m.min(val); }
};

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
    const pseudo_particle_type pp = make_pseudo_particle(p.begin(), p.end());

    // Run the minimum visitor over the leaf nodes
    const particle_type::array_type minv = pp.visit_children(min_visitor());

    BOOST_CHECK_EQUAL(pp.q(), N);
    BOOST_CHECK_EQUAL(pp.absq(), N);
    BOOST_CHECK_EQUAL(pp.visit_children(counting_visitor()), N);

    BOOST_CHECK_CLOSE(pp.r().x(), mean(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.r().y(), mean(yacc), TOL);

    BOOST_CHECK_EQUAL(pp.min().x(), min(xacc));
    BOOST_CHECK_EQUAL(pp.min().y(), min(yacc));

    BOOST_CHECK_EQUAL(pp.max().x(), max(xacc));
    BOOST_CHECK_EQUAL(pp.max().y(), max(yacc));

    BOOST_CHECK_EQUAL(pp.min().x(), minv.x());
    BOOST_CHECK_EQUAL(pp.min().y(), minv.y());
}
