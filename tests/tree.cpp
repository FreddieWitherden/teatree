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
#include <boost/accumulators/statistics/variance.hpp>
#include <boost/accumulators/statistics/covariance.hpp>
#include <boost/accumulators/statistics/variates/covariate.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/uniform_real.hpp>

#include <vector>

namespace acc = boost::accumulators;
using namespace boost::random;
using namespace Eigen;
using namespace teatree;

// Type of particle being used for the test
typedef particle<Vector2d> particle_type;
typedef pseudo_particle<particle_type,2> pseudo_particle_type;

// The number of particles to use for testing
const int N = 5000;
const double TOL = 0.01;

// Accumulator type required
typedef acc::accumulator_set< double
                            , acc::stats< acc::tag::mean
                                        , acc::tag::min
                                        , acc::tag::max
                                        , acc::tag::lazy_variance
                                        , acc::tag::covariance< double
                                                              , acc::tag::covariate1
                                                              >
                                        >
                            > acc_type;

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
struct min_visitor : public tree_visitor< min_visitor
                                        , pseudo_particle_type
                                        , particle_type::array_type
                                        >
{
    typedef particle_type::array_type array_type;

    bool accept(const pseudo_particle_type&)      const { return false; }
    array_type visit(const pseudo_particle_type&) const { return array_type(); }
    array_type visit(const particle_type& p)      const { return p.r(); }
    void reduce(array_type& m, const array_type& val) const { m = m.min(val); }
};

/**
 * Test the core elements of tree construction and visitation.
 */
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

        xacc(x, acc::covariate1 = y); yacc(y, acc::covariate1 = x);
        p.push_back(particle_type(Vector2d(x, y), Vector2d::Zero(), 1, 1));
    }

    // Create the tree
    const pseudo_particle_type pp = make_pseudo_particle<2>(p.begin(), p.end());

    // Create and run the minimum visitor over the leaf nodes
    min_visitor mv;
    const particle_type::array_type minv = pp.visit_children(mv);

    // Check particle count and net charge; N particles each with Q = +1
    BOOST_CHECK_EQUAL(pp.q(), N);
    BOOST_CHECK_EQUAL(pp.absq(), N);
    BOOST_CHECK_EQUAL(pp.visit_children(counting_visitor()), N);

    // Check CoQ given that the distribution is uniform in x and y
    BOOST_CHECK_CLOSE(pp.r().x(), acc::mean(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.r().y(), acc::mean(yacc), TOL);

    // Check the min and max coordinates match up
    BOOST_CHECK_EQUAL(pp.min().x(), acc::min(xacc));
    BOOST_CHECK_EQUAL(pp.min().y(), acc::min(yacc));
    BOOST_CHECK_EQUAL(pp.max().x(), acc::max(xacc));
    BOOST_CHECK_EQUAL(pp.max().y(), acc::max(yacc));

    // These should agree with our custom min-visitor
    BOOST_CHECK_EQUAL(pp.min().x(), minv.x());
    BOOST_CHECK_EQUAL(pp.min().y(), minv.y());

    // Ensure that the min_visitor only visited leaves (particles)
    BOOST_CHECK_EQUAL(mv.branches_visited(), 0);
    BOOST_CHECK_EQUAL(mv.leaves_visited(), N);

    // The dipole moment should be zero about the CoQ as Q is always +1
    BOOST_CHECK_SMALL(pp.moments().Dx, TOL);
    BOOST_CHECK_SMALL(pp.moments().Dy, TOL);

    // The diagonal quadrupole moment should be N*Var(p.[xy])
    BOOST_CHECK_CLOSE(pp.moments().Qxx, N*acc::variance(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qyy, N*acc::variance(yacc), TOL);

    // Off diagonal quadrupole moment should be N*Cov(p.x,p.y)
    BOOST_CHECK_CLOSE(acc::covariance(xacc), acc::covariance(yacc), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxy, N*acc::covariance(xacc), TOL);
}
