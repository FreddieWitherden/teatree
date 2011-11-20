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

#include "particle/moments/shift_recurse.hpp"
#include "particle/typedefs.hpp"
#include "tree/visitor.hpp"

#include <boost/static_assert.hpp>

namespace teatree
{

/**
 *
 */
template<typename PParticleT>
class moments_visitor
    : public tree_visitor<moments_visitor<PParticleT>,PParticleT>
{
private: // Types
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    typedef moments_shift_recurse< scalar_type
                                 , pseudo_particle_type::dimension
                                 , pseudo_particle_type::multipole_order
                                 > moments_shift_recuse_type;

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

    moments_shift_recuse_type::exec(m_, p_moments, d);
}

template<typename PParticleT> inline
void moments_visitor<PParticleT>::visit(const pseudo_particle_type& pp)
{
    const array_type d = r_ - pp.r();
    moments_shift_recuse_type::exec(m_, pp.moments(), d);
}

}

#endif // TEATREE_PARTICLE_MOMENTS_VISITOR_HPP
