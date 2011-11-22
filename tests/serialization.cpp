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

#define BOOST_TEST_MODULE serialization

#include <BoostTestTargetConfig.h>

#include "config.h"

#include <Eigen/Core>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/array.hpp>

#include <sstream>
#include <string>

using namespace Eigen;
using namespace boost::archive;

/**
 * Attempts to serialize and unserialize the object t1.  The process is
 *  verified by serializing the unserialized object and comparing it
 *  against the first serialization.
 */
template<typename T>
static void serialize_test(const T& t1)
{
    // Create an output archive and serialize t1
    std::stringstream ss1;
    text_oarchive oa1(ss1);
    oa1 << t1;

    // Un-serialize t1 into t2
    text_iarchive ia(ss1);
    T t2;
    ia >> t2;

    // Create a fresh output archive and serialize t2
    std::stringstream ss2;
    text_oarchive oa2(ss2);
    oa2 << t2;

    // If everything worked the two serializations should be the same
    BOOST_CHECK_EQUAL(ss1.str(), ss2.str());
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
