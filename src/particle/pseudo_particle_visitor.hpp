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

#ifndef TEATREE_PARTICLE_PSEUDO_PARTICLE_VISITOR_HPP
#define TEATREE_PARTICLE_PSEUDO_PARTICLE_VISITOR_HPP

#include "particle/typedefs.hpp"
#include "tree/visitor.hpp"

#include <boost/assert.hpp>

namespace teatree
{

template<typename PParticleT>
class pseudo_particle_visitor
    : public tree_visitor<pseudo_particle_visitor<PParticleT>,PParticleT>
{
public:
    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT);

    pseudo_particle_visitor()
      : sum_q_(0)
      , sum_absq_(0)
      , sum_absq_r_(vector_type::Zero())
      , first_(true)
    {}

    bool accept(const PParticleT&) { return true; }
    void visit(const particle_type& p);
    void visit(const PParticleT& pp);

    void reduce(scalar_type& q, scalar_type& absq, vector_type& coq,
                array_type& min, array_type& max, scalar_type& size2);

private:
    template<typename P>
    void visit_common(P& p);

    scalar_type sum_q_;
    scalar_type sum_absq_;
    vector_type sum_absq_r_;
    array_type  min_;
    array_type  max_;
    bool        first_;
};

template<typename PParticleT>
void pseudo_particle_visitor<PParticleT>::visit(const particle_type& p)
{
    min_ = first_ ? p.r() : min_.min(p.r().array());
    max_ = first_ ? p.r() : max_.max(p.r().array());
    visit_common(p);
}

template<typename PParticleT>
void pseudo_particle_visitor<PParticleT>::visit(const PParticleT& p)
{
    min_ = first_ ? p.min() : min_.min(p.min());
    max_ = first_ ? p.max() : max_.max(p.max());
    visit_common(p);
}

template<typename PParticleT> inline
void pseudo_particle_visitor<PParticleT>::reduce
    (scalar_type& q, scalar_type& absq, vector_type& coq,
     array_type& min, array_type& max, scalar_type& size2)
{
    BOOST_ASSERT(!first_);

    q     = sum_q_;
    absq  = sum_absq_;
    coq   = sum_absq_r_/sum_absq_;
    min   = min_;
    max   = max_;
    size2 = (max_ - min_).matrix().squaredNorm();
}

template<typename PParticleT>
template<typename P> inline
void pseudo_particle_visitor<PParticleT>::visit_common(P& p)
{
    sum_q_      += p.q();
    sum_absq_   += p.absq();
    sum_absq_r_ += p.absq()*p.r();
    first_      =  false;
}

}

#endif // TEATREE_PARTICLE_PSEUDO_PARTICLE_VISIOR_HPP
