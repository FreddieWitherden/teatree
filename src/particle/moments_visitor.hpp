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

#ifndef TEATREE_PARTICLE_MOMENTS_VISITOR_HPP
#define TEATREE_PARTICLE_MOMENTS_VISITOR_HPP

#include "particle/typedefs.hpp"
#include "tree/visitor.hpp"

#include <boost/static_assert.hpp>

namespace teatree { namespace detail
{

template<typename ScalarT, int Dim, int MultP>
struct moments_recurse
{
    // Static assert
    BOOST_STATIC_ASSERT_MSG(sizeof(ScalarT) == 0, "Invalid multipole order.");
};

/**
 * Monopole moment (net charge) in either 2D or 3D.  This is a no-op as the
 *  net charge is computed by pseudo_particle_visitor.
 */
template<typename ScalarT, int Dim>
struct moments_recurse<ScalarT,Dim,0>
{
    typedef particle_moments<ScalarT,Dim,0> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r) {}
};


/**
 * Computes the dipole moment in 2D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,2,1>
{
    typedef particle_moments<ScalarT,2,1> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT pD = ArrayT(d.Dx,d.Dy) - r*d.M;
        p.Dx += pD.x(); p.Dy += pD.y();
    }
};

/**
 * Computes the dipole moment in 3D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,3,1>
{
    typedef particle_moments<ScalarT,3,1> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        const ArrayT pD = ArrayT(d.Dx,d.Dy,d.Dz) - r*d.M;
        p.Dx += pD.x(); p.Dy += pD.y(); p.Dz += pD.z();
    }
};

/**
 * Computes the dipole and quadrupole moments in 2D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,2,2>
{
    typedef particle_moments<ScalarT,2,2> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        // Diagonal; compute Qxx and Qyy together
        const ArrayT pQ = ArrayT(d.Qxx,d.Qyy) - 2*r*ArrayT(d.Dx,d.Dy) + r*r*d.M;
        p.Qxx += pQ.x(); p.Qyy += pQ.y();

        // Off diagonal
        p.Qxy += d.Qxy - r.x()*d.Dy - r.y()*d.Dx + r.x()*r.y()*d.M;

        // Recurse to compute the dipole
        moments_recurse<ScalarT,2,1>::exec(p, d, r);
    }
};

/**
 * Computes the dipole and quadrupole moments in 2D.
 */
template<typename ScalarT>
struct moments_recurse<ScalarT,3,2>
{
    typedef particle_moments<ScalarT,3,2> moments_type;

    template<typename ArrayT>
    static void exec(moments_type& p, const moments_type& d, const ArrayT& r)
    {
        // Diagonal; compute Qxxx, Qyyy and Qzzz together
        const ArrayT pdQ = ArrayT(d.Qxx,d.Qyy,d.Qzz)
                         - 2*r*ArrayT(d.Dx,d.Dy,d.Dz)
                         + r*r*d.M;
        p.Qxx += pdQ.x(); p.Qyy += pdQ.y(); p.Qzz += pdQ.z();

        // Off diagonal
        p.Qxy += d.Qxy - r.x()*d.Dy - r.y()*d.Dx + r.x()*r.y()*d.M;
        p.Qxz += d.Qxz - r.x()*d.Dz - r.z()*d.Dx + r.x()*r.z()*d.M;
        p.Qyz += d.Qyz - r.y()*d.Dz - r.z()*d.Dy + r.y()*r.z()*d.M;

        // Recurse to compute the dipole
        moments_recurse<ScalarT,3,1>::exec(p, d, r);
    }
};

}

/**
 *
 */
template<typename PParticleT>
class moments_visitor
    : public tree_visitor<moments_visitor<PParticleT>,PParticleT>
{
private: // Types
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    typedef detail::moments_recurse< scalar_type
                                   , PParticleT::dimension
                                   , PParticleT::multipole_order
                                   > moments_recurse;

public: // Methods
    moments_visitor(particle_moments_type& m, const vector_type& r)
        : m_(m)
        , r_(r)
    {}

    bool accept(const pseudo_particle_type&) { return true; }
    void visit(const particle_type& p);
    void visit(const pseudo_particle_type& pp);

private: // Members
    particle_moments_type& m_;
    const vector_type& r_;
};

template<typename PParticleT> inline
void moments_visitor<PParticleT>::visit(const particle_type& p)
{
    const array_type d = r_ - p.r();

    // Individual particles have only a monopole moment
    particle_moments_type p_moments;
    p_moments.M = p.q();

    moments_recurse::exec(m_, p_moments, d);
}

template<typename PParticleT> inline
void moments_visitor<PParticleT>::visit(const pseudo_particle_type& pp)
{
    const array_type d = r_ - pp.r();
    moments_recurse::exec(m_, pp.moments(), d);
}

}

#endif // TEATREE_PARTICLE_MOMENTS_VISITOR_HPP
