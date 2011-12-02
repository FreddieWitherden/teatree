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
#include <iosfwd>
#include <iomanip>
#include <stdexcept>
#include <string>

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
    const scalar_type  m()    const { return q_/qtom_; }
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
    , q_(1.0)
    , qtom_(1.0)
{}

template<typename VectorT> inline
particle<VectorT>::particle(const vector_type& r,
                            const vector_type& v,
                            const scalar_type& q,
                            const scalar_type& m)
    : r_(r), v_(v), q_(q), qtom_(q/m)
{
    // Parameter checking
    if (m <= 0.0) throw std::domain_error("bad particle mass");
    if (q == 0.0) throw std::domain_error("bad particle charge");
}

template<typename VectorT>
template<typename ArchiveT>
void particle<VectorT>::serialize(ArchiveT& ar, unsigned int)
{
    ar & r_;
    ar & v_;
    ar & q_;
    ar & qtom_;
}

/**
 * Writes a suitable set of column heading to the stream that describe
 *  the columns of output produced by std::ostream& << particle.
 */
template<typename ParticleT>
std::string particle_format_header()
{
    const char* colh = "xyz";
    std::ostringstream ss;

    for (int i = 0; i < ParticleT::dimension; ++i)
        ss << "   r" << std::setw(10) << std::left << colh[i];
    for (int i = 0; i < ParticleT::dimension; ++i)
        ss << "   v" << std::setw(10) << std::left << colh[i];
    ss << std::setw(10) << "   q" << std::setw(7) << 'm';

    return ss.str();
}

template<typename VectorT>
std::ostream& operator<<(std::ostream& os, const particle<VectorT>& p)
{
    std::ostringstream ss;
    ss << std::setprecision(5) << std::scientific;

    for (int i = 0; i < particle<VectorT>::dimension; ++i)
        ss << std::setw(14) << p.r()[i];
    for (int i = 0; i < particle<VectorT>::dimension; ++i)
        ss << std::setw(14) << p.v()[i];
    ss << std::resetiosflags(std::ios::scientific) << std::setprecision(3);
    ss << std::setw(7) << p.q() << std::setw(7) << p.m();

    return os << ss.str();
}

template<typename VectorT>
std::istream& operator>>(std::istream& is, particle<VectorT>& p)
{
    typename particle<VectorT>::vector_type r, v;
    typename particle<VectorT>::scalar_type q, m;

    for (int i = 0; i < particle<VectorT>::dimension; ++i)
        is >> r[i];
    for (int i = 0; i < particle<VectorT>::dimension; ++i)
        is >> v[i];
    is >> q >> m;

    p = particle<VectorT>(r, v, q, m);

    return is;
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
