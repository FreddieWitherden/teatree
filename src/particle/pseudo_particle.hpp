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

#ifndef TEATREE_PARTICLE_PSEUDO_PARTICLE_HPP
#define TEATREE_PARTICLE_PSEUDO_PARTICLE_HPP

#include "particle/moments/moments.hpp"
#include "particle/moments/visitor.hpp"
#include "particle/pseudo_particle_visitor.hpp"
#include "particle/typedefs.hpp"
#include "tree/branch.hpp"
#include "utils/ipow.hpp"

#include <cmath>
#include <boost/assert.hpp>
#include <Eigen/Core>


namespace teatree
{

/**
 * A pseudo particle.
 */
template<typename ParticleT, int MultP>
class pseudo_particle : public tree_branch< const ParticleT
                                          , pseudo_particle<ParticleT,MultP>
                                          , ParticleT::dimension
                                          >
{
public: // Types & constants
    typedef pseudo_particle<ParticleT,MultP> pseudo_particle_type;

    TEATREE_PARTICLE_GENERATE_TYPEDEFS(ParticleT);

    enum constants {
        dimension       = particle_type::dimension,
        multipole_order = MultP
    };

    typedef tree_branch< const ParticleT
                       , pseudo_particle_type
                       , ParticleT::dimension
                       > base_type;

    typedef particle_moments< scalar_type
                            , dimension
                            , multipole_order
                            > particle_moments_type;

public: // Constructors
    /**
     *
     */
    template<typename PartFactT, typename IteratorT>
    pseudo_particle(PartFactT partition_factory,
                    IteratorT first,
                    IteratorT last,
                    int level = 0);

    ~pseudo_particle() {}

public: // Accessors
    const vector_type& r()  const { return r_; }
    const array_type& min() const { return min_; }
    const array_type& max() const { return max_; }

    scalar_type q()     const { return moments_.M; }
    scalar_type absq()  const { return absq_; }
    scalar_type size()  const { return std::sqrt(size2_); }
    scalar_type size2() const { return size2_; }

    const particle_moments_type& moments() const { return moments_; }

private:
    vector_type r_;
    array_type  min_;
    array_type  max_;
    scalar_type absq_;
    scalar_type size2_;
    particle_moments_type moments_;
};

template<typename ParticleT, int MultP>
template<typename PartFactT, typename IteratorT>
pseudo_particle<ParticleT,MultP>::pseudo_particle(
    PartFactT partition_factory,
    IteratorT first,
    IteratorT last,
    int level)
    : base_type(partition_factory, first, last, level)
{
    // Visit our children to determine our aggregate properties
    pseudo_particle_visitor<pseudo_particle_type> pv;
    this->visit_children(pv);
    pv.reduce(moments_.M, absq_, r_, min_, max_, size2_);

    // We should never have a zero size
    BOOST_ASSERT_MSG(size2_ != 0, "Invalid pseudo particle size!");

    // Compute the higher order multipole moments (if any)
    moments_visitor<pseudo_particle_type> mv(moments_, r_);
    this->visit_children(mv);
}

}

#endif // TEATREE_PARTICLE_PSEUDO_PARTICLE_HPP
