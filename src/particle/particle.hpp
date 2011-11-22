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

#ifndef TEATREE_PARTICLE_PARTICLE_HPP
#define TEATREE_PARTICLE_PARTICLE_HPP

#include <algorithm>
#include <cmath>

#include <boost/serialization/access.hpp>
#include <boost/serialization/serialization.hpp>
#include <boost/type_traits.hpp>

#include <Eigen/Core>
#include <Eigen/StdVector>

namespace teatree
{

/**
 * A particle in spacetime with a position, velocity, charge and mass.
 */
template<typename VectorT>
class particle
{
public: // Types and constants
    typedef VectorT vector_type;
    typedef typename Eigen::ArrayWrapper<vector_type>::PlainObject array_type;
    typedef typename vector_type::Scalar scalar_type;

    enum constants { dimension = vector_type::RowsAtCompileTime };

public: // Constructors
    particle();

    particle(const vector_type& rinit, const vector_type& vinit,
             const scalar_type& q, const scalar_type& m);

public: // Accessor methods
    const vector_type& r()    const { return r_; }
    const vector_type& v()    const { return v_; }
    const scalar_type  q()    const { return q_; }
    const scalar_type  absq() const { return std::fabs(q_); }
    const scalar_type  qtom() const { return qtom_; }

    vector_type& r() { return r_; }
    vector_type& v() { return v_; }

private: // Allow for serialisation
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned int /*version*/);

private: // Storage
    vector_type r_;
    vector_type v_;

    scalar_type q_;
    scalar_type qtom_;

private: // Alignment considerations
    enum {
        needs_alignment_ = (boost::alignment_of<vector_type>::value % 16) == 0
    };

public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW_IF(needs_alignment_);
};

template<typename VectorT> inline
particle<VectorT>::particle()
    : r_(vector_type::Zero())
    , v_(vector_type::Zero())
    , q_(0.0)
    , qtom_(0.0)
{}

template<typename VectorT> inline
particle<VectorT>::particle(const vector_type& r,
                            const vector_type& v,
                            const scalar_type& q,
                            const scalar_type& m)
    : r_(r), v_(v), q_(q), qtom_(q/m)
{}

template<typename VectorT>
template<typename ArchiveT>
void particle<VectorT>::serialize(ArchiveT& ar, unsigned int)
{
    ar & r_;
    ar & v_;
    ar & q_;
    ar & qtom_;
}

}

/*
 * To place objects containing ``fixed-sized vectorizable" Eigen
 * members into a std::vector it is necessary to first define a
 * specialization of std::vector.  We do this here for vectors of
 * particles and vectors of vectors.
 *
 * See: http://eigen.tuxfamily.org/dox/TopicStlContainers.html
 */
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(Eigen::Vector2d);
EIGEN_DEFINE_STL_VECTOR_SPECIALIZATION(teatree::particle<Eigen::Vector2d>);

#endif // TEATREE_PARTICLE_PARTICLE_HPP
