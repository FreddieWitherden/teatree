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

#ifndef TEATREE_PSEUDO_PARTICLE_VISITOR_HPP
#define TEATREE_PSEUDO_PARTICLE_VISITOR_HPP

#include "tree/visitor.hpp"

namespace teatree { namespace pseduo_particle_visitor_
{

template<typename ParticleT, typename PParticleT>
class pseudo_particle_visitor : public tree_visitor<ParticleT,PParticleT>
{
public:
    typedef typename ParticleT::vector_type vector_type;
    typedef typename ParticleT::array_type  array_type;
    typedef typename ParticleT::scalar_type scalar_type;

    pseudo_particle_visitor()
      : sum_q_(0)
      , sum_absq_(0)
      , sum_absq_r_(vector_type::Zero())
      , min_(vector_type::Zero())
      , max_(vector_type::Zero())
    {}

    bool accept(PParticleT&) { return true; }
    void visit(ParticleT& p);
    void visit(PParticleT& pp);

    void reduce(scalar_type& q, scalar_type& absq, vector_type& coq,
                array_type& min, array_type& max, scalar_type& size);

private:
    template<typename P>
    void visit_common(P& p);

    scalar_type sum_q_;
    scalar_type sum_absq_;
    vector_type sum_absq_r_;
    array_type  min_;
    array_type  max_;
};

template<typename ParticleT, typename PParticleT>
void pseudo_particle_visitor<ParticleT,PParticleT>::visit(ParticleT& p)
{
    visit_common(p);
    min_ = min_.min(p.r().array());
    max_ = max_.max(p.r().array());
}

template<typename ParticleT, typename PParticleT>
void pseudo_particle_visitor<ParticleT,PParticleT>::visit(PParticleT& p)
{
    visit_common(p);
    min_ = min_.min(p.min());
    max_ = max_.max(p.max());
}

template<typename ParticleT, typename PParticleT> inline
void pseudo_particle_visitor<ParticleT,PParticleT>::reduce
    (scalar_type& q, scalar_type& absq, vector_type& coq,
     array_type& min, array_type& max, scalar_type& size)
{
    q    = sum_q_;
    absq = sum_absq_;
    coq  = sum_absq_r_/sum_absq_;
    min  = min_;
    max  = max_;
    size = (max_ - min_).matrix().norm();
}

template<typename ParticleT, typename PParticleT>
template<typename P> inline
void pseudo_particle_visitor<ParticleT,PParticleT>::visit_common(P& p)
{
    sum_q_      += p.q();
    sum_absq_   += p.absq();
    sum_absq_r_ += p.absq()*p.r();
}

}

using pseduo_particle_visitor_::pseudo_particle_visitor;

}

#endif // TEATREE_PSEUDO_PARTICLE_VISIOR_HPP
