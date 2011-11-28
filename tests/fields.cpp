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

#define BOOST_TEST_MODULE fields

#include <BoostTestTargetConfig.h>

#include "config.h"

#include "accel/eval.hpp"
#include "efield/plummer.hpp"
#include "particle/particle.hpp"
#include "particle/make_pseudo_particle.hpp"
#include "mac/opening_angle.hpp"
#include "simulation/options.hpp"

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/random/discrete_distribution.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <boost/random/uniform_real.hpp>

#include <vector>

using namespace boost::assign;
using namespace boost::random;
using namespace Eigen;
using namespace teatree;

const int N = 500;
const double EPSILON = 1.0e-5;
const double TOL = 1.0e-4;

/**
 * Test the 2D electric field in the particle-particle regime assuming
 *  particles to be Plummer rods.
 */
BOOST_AUTO_TEST_CASE(plummer2d)
{
    typedef particle<Vector2d> particle_type;
    typedef pseudo_particle<particle_type,0> pseudo_particle_type;

    typedef efield_plummer<pseudo_particle_type,mac_opening_angle> efield_type;

    // Create two oppositely charged particles/rods
    const particle_type p0(Vector2d(-5.0, 0.0), Vector2d::Zero(),  1, 1);
    const particle_type p1(Vector2d( 5.0, 0.0), Vector2d::Zero(), -1, 1);

    // Some simple simulation options
    simulation_options so; so.epsilon(EPSILON).theta(0.0);

    const efield_type ef0(p0.r(), so), ef1(p1.r(), so);

    // Ensure we do not have any self-iteraction
    BOOST_CHECK_SMALL(ef0.visit(p0).x(), TOL);
    BOOST_CHECK_SMALL(ef0.visit(p0).y(), TOL);
    BOOST_CHECK_SMALL(ef1.visit(p1).x(), TOL);
    BOOST_CHECK_SMALL(ef1.visit(p1).y(), TOL);

    // Acceleration is related to E through a = F/m = q/m * E
    const Vector2d ap0 = p0.qtom()*ef0.visit(p1);
    const Vector2d ap1 = p1.qtom()*ef1.visit(p0);

    // The two rods should attract each other with |a| ~= [ 0.1, 0.0 ]
    BOOST_CHECK_CLOSE(ap0.x(),  0.1,  TOL);
    BOOST_CHECK_CLOSE(ap1.x(), -0.1, TOL);
    BOOST_CHECK_SMALL(ap0.y(), TOL);
    BOOST_CHECK_SMALL(ap1.y(), TOL);
}

BOOST_AUTO_TEST_CASE(pseudo2d)
{
    typedef particle<Vector2d> particle_type;
    typedef pseudo_particle<particle_type,2> pseudo_particle_type;
    typedef simulation_options options_type;
    typedef efield_plummer<pseudo_particle_type,mac_opening_angle,0> ef_m_type;
    typedef efield_plummer<pseudo_particle_type,mac_opening_angle,1> ef_d_type;
    typedef efield_plummer<pseudo_particle_type,mac_opening_angle,2> ef_q_type;

    // Random number generation
    lagged_fibonacci607 eng(26535);
    uniform_real_distribution<> xdist(5.0, 10.0), ydist(12.0, 17.0);
    discrete_distribution<int>  qdist = list_of(45)(100-45);

    // Particle storage
    std::vector<particle_type> p;
    p.reserve(N);

    // Create the particles
    for (int i = 0; i < N; ++i)
    {
        const double x = xdist(eng), y = ydist(eng);
        const double q = qdist(eng) ? 1 : -1;
        p.push_back(particle_type(Vector2d(x, y), Vector2d::Zero(), q, 1));
    }

    // Create the pseudo-particle
    const pseudo_particle_type pp = make_pseudo_particle<2>(p.begin(), p.end());

    /*
     * Checking the validity of the multipole expansion is complicated
     * by the fact that the expansion does not always converge and
     * sometimes adding extra terms can increase the error rather than
     * reduce it.  Here we just check that it has an overall positive
     * effect on the simulation.
     */

    // Count the number of failures
    int nfail_md = 0, nfail_dq = 0, nfail_mq = 0;

    options_type opt; opt.epsilon(EPSILON).theta(0.6);
    accel_eval<ef_q_type> ae(opt);

    BOOST_FOREACH(const particle_type& tp, p)
    {
        // Compute E(pos) directly by setting theta to zero
        options_type d_opts; d_opts.epsilon(EPSILON).theta(0.0);
        const Vector2d E_d = ef_m_type(tp.r(), d_opts)(&pp);

        // Compute E(pos) using the tree at various multipole orders
        options_type pp_opts; pp_opts.epsilon(EPSILON).theta(0.6);
        const Vector2d E_ppm = ef_m_type(tp.r(), pp_opts)(&pp);
        const Vector2d E_ppd = ef_d_type(tp.r(), pp_opts)(&pp);
        const Vector2d E_ppq = ef_q_type(tp.r(), pp_opts)(&pp);

        // Define the error to be the deviation from the direct computation
        const double sigma_m = (E_d - E_ppm).norm();
        const double sigma_d = (E_d - E_ppd).norm();
        const double sigma_q = (E_d - E_ppq).norm();

        /*
         * Generally, the error should be of the form σ_q < σ_d < σ_m.
         * However, in some pathological cases the error can be
         * increased by including extra terms.
         */
        nfail_md += sigma_d > sigma_m;
        nfail_dq += sigma_q > sigma_d;
        nfail_mq += sigma_q > sigma_m;
    }

    // Failures should be less than N/10
    BOOST_CHECK_LT(nfail_md, N/10);
    BOOST_CHECK_LT(nfail_dq, N/10);
    BOOST_CHECK_LT(nfail_mq, N/10);
}
