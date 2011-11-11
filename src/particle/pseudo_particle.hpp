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

#ifndef TEATREE_PSEUDO_PARTICLE_HPP
#define TEATREE_PSEUDO_PARTICLE_HPP

#include "particle/pseudo_particle_visitor.hpp"
#include "tree/branch.hpp"
#include "utils/ipow.hpp"

#include <Eigen/Core>


namespace teatree
{

/**
 * A pseudo particle.
 */
template<typename ParticleT>
class pseudo_particle : public tree_branch<ParticleT,
                                           pseudo_particle<ParticleT>,
                                           ParticleT::dimension>
{
public: // Types & constants
    typedef ParticleT                  particle_type;
    typedef pseudo_particle<ParticleT> pseudo_particle_type;

    typedef typename particle_type::vector_type vector_type;
    typedef typename particle_type::scalar_type scalar_type;
    typedef typename particle_type::array_type  array_type;

    typedef tree_branch<ParticleT,pseudo_particle_type,
                        ParticleT::dimension> base_type;

    enum constants {
        dimension    = particle_type::dimension,
        num_children = ipow<2,dimension>::value
    };

public: // Constructors
    /**
     *
     */
    template<typename PartFactT, typename IteratorT>
    pseudo_particle(PartFactT partition_factory,
                    IteratorT first,
                    IteratorT last);

    ~pseudo_particle() {}

public: // Accessors
    const vector_type& r()   const { return r_; }
    const array_type& min() const { return min_; }
    const array_type& max() const { return max_; }

    scalar_type q()    const { return q_; }
    scalar_type absq() const { return absq_; }
    scalar_type size() const { return size_; }

private:
    vector_type r_;
    array_type min_;
    array_type max_;
    scalar_type q_;
    scalar_type absq_;
    scalar_type size_;
};

template<typename ParticleT>
template<typename PartFactT, typename IteratorT>
pseudo_particle<ParticleT>::pseudo_particle(
    PartFactT partition_factory,
    IteratorT first,
    IteratorT last)
    : base_type(partition_factory, first, last)
    , min_(vector_type::Zero())
    , max_(vector_type::Zero())
{
    // Visit our children to determine our aggregate properties
    pseudo_particle_visitor<pseudo_particle_type> tv;
    this->visit_children(tv);
    tv.reduce(q_, absq_, r_, min_, max_, size_);
}

}

#endif // TEATREE_PSEUDO_PARTICLE_HPP
