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

#define BOOST_TEST_MODULE serialization

#include <BoostTestTargetConfig.h>

#include "config.h"

#include "accel/open.hpp"
#include "constraint/open.hpp"
#include "efield/plummer.hpp"
#include "mac/opening_angle.hpp"
#include "particle/particle.hpp"
#include "particle/pseudo_particle.hpp"
#include "pusher/comp/21.hpp"
#include "simulation/options.hpp"


#include <Eigen/Core>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/array.hpp>

#include <sstream>
#include <string>

using namespace Eigen;
using namespace boost::archive;
using namespace teatree;

/**
 * Runs the object in through a serialize-unserialize cycle.
 */
template<typename T>
static std::string serialize_cycle(const T& in, T& out)
{
    // Create an output archive and serialize t1
    std::stringstream ss;
    binary_oarchive oa(ss);
    oa << in;

    // Un-serialize t1 into t2
    binary_iarchive ia(ss);
    ia >> out;

    return ss.str();
}

/**
 *
 */
template<typename T>
static void serialize_cmp(const T& t1, const T& t2)
{
    // Create an output archive and serialize t1
    std::stringstream ss1, ss2;
    binary_oarchive oa1(ss1), oa2(ss2);
    oa1 << t1; oa2 << t2;

    BOOST_CHECK_EQUAL(ss1.str(), ss2.str());
}

/**
 * Checks to see if the object t1 can be serialized and unserialized
 *  successfully.  The test requires that the type T has a default
 *  constructor.
 */
template<typename T>
static void serialize_test(const T& t1)
{
    T t2, t3;
    const std::string s1 = serialize_cycle(t1, t2);
    const std::string s2 = serialize_cycle(t2, t3);

    // If everything worked the two serializations should be the same
    BOOST_CHECK_EQUAL(s1, s2);
}

BOOST_AUTO_TEST_CASE(eigen)
{
    // Test the commonly used vector types
    for (int i = 0; i < 150; ++i)
    {
        Vector2f a(Vector2f::Random()); serialize_test(a);
        Vector2d b(Vector2d::Random()); serialize_test(b);
        Vector3f c(Vector3f::Random()); serialize_test(c);
        Vector3d d(Vector3d::Random()); serialize_test(d);
    }

    // Do a similar thing with the array types
    for (int i = 0; i < 150; ++i)
    {
        Array2f a(Array2f::Random()); serialize_test(a);
        Array2d b(Array2d::Random()); serialize_test(b);
        Array3f c(Array3f::Random()); serialize_test(c);
        Array3d d(Array3d::Random()); serialize_test(d);
    }
}

BOOST_AUTO_TEST_CASE(simulation_options_t)
{
    simulation_options so; so.theta(0.25).epsilon(1.0e-5);
    serialize_test(so);
}

BOOST_AUTO_TEST_CASE(accel_eval_t)
{
    // Prerequisite types
    typedef particle<Vector3f> particle_type;
    typedef pseudo_particle<particle_type,2> pseudo_particle_type;
    typedef efield_plummer<pseudo_particle_type,mac_opening_angle> ef_type;

    // Trial values
    int N = 320;
    simulation_options so; so.theta(0.25).epsilon(1.0e-5);

    accel_open<ef_type> ae(so);
    serialize_test(ae);
}

BOOST_AUTO_TEST_CASE(pusher_verlet_t)
{
    // Prerequisite types
    typedef particle<Vector3f> particle_type;
    typedef pseudo_particle<particle_type,1> pseudo_particle_type;
    typedef efield_plummer<pseudo_particle_type,mac_opening_angle> ef_type;
    typedef accel_open<ef_type> accel_eval_type;
    typedef constraint_open<particle_type> constraint_type;
    typedef pusher_comp_21<accel_eval_type, constraint_type> pusher_type;

    const int N = 320;

    // Generate some particles
    std::vector<particle_type> p;
    p.reserve(N);

    for (int i = 0; i < N; ++i)
    {
        Vector3f r = Vector3f::Random(), v = 0.1*Vector3f::Random();

        p.push_back(particle_type(r, v, 1, 1));
    }

    // Pick some reasonable simulation options
    simulation_options so; so.theta(0.3).epsilon(1.0e-4).nd(20);

    // Create the particle pusher
    pusher_type pusher(p, accel_eval_type(so), constraint_type(), 0.0, so.dt());

    // Advance it a few times
    for (int i = 0; i < 20; ++i) pusher.advance();

    // Run the object through a serialization cycle
    pusher_type pusher_mid;
    serialize_cycle(pusher, pusher_mid);

    // Advance both objects a few more times
    for (int i = 0; i < 20; ++i) pusher.advance(), pusher_mid.advance();

    // Check that both give the same result
    serialize_cmp(pusher, pusher_mid);
}
