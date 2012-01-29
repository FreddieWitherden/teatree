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

#define BOOST_TEST_MODULE tree

#include <BoostTestTargetConfig.h>

#include "config.h"

#include "particle/particle.hpp"
#include "particle/make_pseudo_particle.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics/stats.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/min.hpp>
#include <boost/accumulators/statistics/max.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/uniform_real.hpp>

#include <vector>

namespace acc = boost::accumulators;
using namespace boost::assign;
using namespace boost::random;
using namespace Eigen;
using namespace teatree;

// The number of particles to use for testing
const int N = 5000;
const double TOL = 0.01;

/**
 * A simple visitor to count the number of particles in the tree.
 */
template<typename PParticleT>
struct counting_visitor
    : public tree_visitor<counting_visitor<PParticleT>,PParticleT,int>
{
    bool accept(const PParticleT&)                       const { return false; }
    int visit(const PParticleT&)                         const { return 0; }
    int visit(const typename PParticleT::particle_type&) const { return 1; }
};

/**
 * Walks the leaves of a tree computing the minimum position as it goes.
 *  This makes use of a custom reduction operator.
 */
template<typename PParticleT>
struct min_visitor : public tree_visitor< min_visitor<PParticleT>
                                        , PParticleT
                                        , typename PParticleT::array_type
                                        >
{
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    bool accept(const pseudo_particle_type&)      const { return false; }
    array_type visit(const pseudo_particle_type&) const { return array_type(); }
    array_type visit(const particle_type& p)      const { return p.r(); }
    void reduce(array_type& m, const array_type& val) const { m = m.min(val); }
};

/**
 * Test the core elements of tree construction and visitation.
 */
BOOST_AUTO_TEST_CASE(construction2d)
{
    // Type of particle and visitors being used for the test
    typedef particle<Vector2d> particle_type;
    typedef pseudo_particle<particle_type,3> pseudo_particle_type;
    typedef min_visitor<pseudo_particle_type> min_visitor_type;
    typedef counting_visitor<pseudo_particle_type> counting_visitor_type;

    // Random number generation
    lagged_fibonacci607 eng(31415);
    uniform_real_distribution<> xdist(-50.0, 200.0), ydist(1, 200.0);

    // General statistical accumulators
    acc::accumulator_set< double
                        , acc::stats< acc::tag::mean
                                    , acc::tag::min
                                    , acc::tag::max
                        > > xacc, yacc;

    // Multipole moment accumulators
    acc::accumulator_set< double
                        , acc::stats<acc::tag::sum>
                        > Qxx, Qyy, Qxy, Oxxx, Oyyy, Oxxy, Oxyy;

    // Particle storage
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
    const pseudo_particle_type pp = make_pseudo_particle<3>(p.begin(), p.end());

    // Manually compute the (relevant) multipole moments
    for (int i = 0; i < N; ++i)
    {
        const Vector2d R = p[i].r() - pp.r();
        const double q   = p[i].q();

        Qxx(q*R.x()*R.x()); Qyy(q*R.y()*R.y()); Qxy(q*R.x()*R.y());
        Oxxx(q*R.x()*R.x()*R.x()); Oyyy(q*R.y()*R.y()*R.y());
        Oxxy(q*R.x()*R.x()*R.y()); Oxyy(q*R.x()*R.y()*R.y());
    }

    // Create and run the minimum visitor over the leaf nodes
    min_visitor_type mv;
    const particle_type::array_type minv = pp.visit_children(mv);

    // Check particle count and net charge; N particles each with Q = +1
    BOOST_CHECK_EQUAL(pp.q(), N);
    BOOST_CHECK_EQUAL(pp.absq(), N);
    BOOST_CHECK_EQUAL(pp.visit_children(counting_visitor_type()), N);

    // Check CoQ given that the distribution is uniform in x and y
    BOOST_CHECK_CLOSE(pp.r().x(), acc::mean(xacc), TOL);
    BOOST_CHECK_CLOSE(pp.r().y(), acc::mean(yacc), TOL);

    // Check the min and max coordinates match up
    BOOST_CHECK_EQUAL(pp.min().x(), acc::min(xacc));
    BOOST_CHECK_EQUAL(pp.min().y(), acc::min(yacc));
    BOOST_CHECK_EQUAL(pp.max().x(), acc::max(xacc));
    BOOST_CHECK_EQUAL(pp.max().y(), acc::max(yacc));

    // These should agree with our custom min_visitor
    BOOST_CHECK_EQUAL(pp.min().x(), minv.x());
    BOOST_CHECK_EQUAL(pp.min().y(), minv.y());

    // Ensure that the min_visitor only visited leaves (particles)
    BOOST_CHECK_EQUAL(mv.branches_visited(), 0);
    BOOST_CHECK_EQUAL(mv.leaves_visited(), N);

    // The dipole moment should be zero about the CoQ as Q is always +1
    BOOST_CHECK_SMALL(pp.moments().Dx.x(), TOL);
    BOOST_CHECK_SMALL(pp.moments().Dx.y(), TOL);

    // Quadrupole moments
    BOOST_CHECK_CLOSE(pp.moments().Qxx.x(), acc::sum(Qxx), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxx.y(), acc::sum(Qyy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxy, acc::sum(Qxy), TOL);

    // Octupole moments
    BOOST_CHECK_CLOSE(pp.moments().Oxxx.x(), acc::sum(Oxxx), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Oxxx.y(), acc::sum(Oyyy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Oxxy.x(), acc::sum(Oxxy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Oxxy.y(), acc::sum(Oxyy), TOL);
}

/**
 * Tests tree construction in 3D; assumes 2D construction is functional.
 */
BOOST_AUTO_TEST_CASE(construction3d)
{
    // Type of particles used in the test
    typedef particle<Vector3f> particle_type;
    typedef pseudo_particle<particle_type,2> pseudo_particle_type;

    // Random number generation, charge is biased towards +ve
    lagged_fibonacci607 eng(27182);
    uniform_real_distribution<float> xdist(-20.0, 20.0),
                                     ydist(-20.0, 20.0),
                                     zdist(  0.0, 10.0);
    discrete_distribution<int,float> qdist = list_of(46)(100-46);

    // CoQ accumulators; as Q = ±1 we can use the regular mean
    acc::accumulator_set< float
                        , acc::stats<acc::tag::mean>
                        > xm, ym, zm;

    // Multipole moment accumulators; we just need the sum
    acc::accumulator_set< float
                        , acc::stats<acc::tag::sum>
                        > M, Dx, Dy, Dz, Qxx, Qyy, Qzz, Qxy, Qxz, Qyz;

    // Particles
    std::vector<particle_type> p;
    p.reserve(N);

    // Create the particles
    for (int i = 0; i < N; ++i)
    {
        const float x = xdist(eng), y = ydist(eng), z = zdist(eng);
        const float q = qdist(eng) ? 1 : -1;

        xm(x); ym(y); zm(z);
        p.push_back(particle_type(Vector3f(x,y,z), Vector3f::Zero(), q, 1));
    }

    // Create the tree
    const pseudo_particle_type pp = make_pseudo_particle<2>(p.begin(), p.end());

    // Manually compute the multipole moments
    for (int i = 0; i < N; ++i)
    {
        const Vector3f R = p[i].r() - pp.r();
        const float q    = p[i].q();

        M(q);
        Dx(q*R.x()); Dy(q*R.y()); Dz(q*R.z());
        Qxx(q*R.x()*R.x()); Qyy(q*R.y()*R.y()); Qzz(q*R.z()*R.z());
        Qxy(q*R.x()*R.y()); Qxz(q*R.x()*R.z()); Qyz(q*R.y()*R.z());
    }

    // Particle count, using Q = ±1
    BOOST_CHECK_EQUAL(pp.absq(), N);

    // CoQ
    BOOST_CHECK_CLOSE(pp.r().x(), acc::mean(xm), TOL);
    BOOST_CHECK_CLOSE(pp.r().y(), acc::mean(ym), TOL);
    BOOST_CHECK_CLOSE(pp.r().z(), acc::mean(zm), TOL);

    // Monopole moment
    BOOST_CHECK_EQUAL(pp.moments().M, pp.q());
    BOOST_CHECK_CLOSE(pp.moments().M, acc::sum(M), TOL);

    // Dipole moment
    BOOST_CHECK_CLOSE(pp.moments().Dx.x(), acc::sum(Dx), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Dx.y(), acc::sum(Dy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Dx.z(), acc::sum(Dz), TOL);

    // Quadrupole moment
    BOOST_CHECK_CLOSE(pp.moments().Qxx.x(), acc::sum(Qxx), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxx.y(), acc::sum(Qyy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxx.z(), acc::sum(Qzz), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxy.x(), acc::sum(Qxy), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxy.y(), acc::sum(Qxz), TOL);
    BOOST_CHECK_CLOSE(pp.moments().Qxy.z(), acc::sum(Qyz), TOL);
}
