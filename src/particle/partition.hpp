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

#ifndef TEATREE_PARTICLE_PARTITION_HPP
#define TEATREE_PARTICLE_PARTITION_HPP

#include "particle/typedefs.hpp"
#include "utils/ipow.hpp"

#include <boost/array.hpp>
#include <boost/iterator/permutation_iterator.hpp>

namespace teatree
{

/**
 * Partitions up particles in N-dimensional real space into orthants.
 */
template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT>
class particle_partition
{
public: // Class types & constants
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(ParticleT);

    typedef EleIteratorT particle_iterator;
    typedef IdxIteratorT index_iterator;
    typedef boost::permutation_iterator< particle_iterator
                                       , index_iterator
                                       > particle_p_iterator;

private:
    enum constants {
        dimension    = particle_type::dimension,
        num_orthants = ipow<2,dimension>::value
    };

    typedef boost::array<particle_p_iterator,num_orthants+1> orthant_list;

public:
    typedef typename orthant_list::iterator orthant_iterator;

public: // Constructors
    /**
     * Given an iterator to the first element of a particle container and two
     *  permutation iterators, first and last, which map into this container
     *  the constructor picks a pivot element and recursively partitions
     *  about this element.
     */
    particle_partition(particle_iterator first_e,
                       particle_p_iterator first_p,
                       particle_p_iterator last_p,
                       int level);

public: // Orthant iteration
    orthant_iterator begin() { return orthants_.begin(); }
    orthant_iterator end()   { return orthants_.end(); }

private: // Mid-point partitioning
    /**
     * Predecate for passing to std::partition
     */
    template<int L>
    class partition_mid_pred
    {
    public:
        partition_mid_pred(particle_iterator first, const vector_type& v)
            : first_(first), v_(v)
        {}

        bool operator()(int i) const { return (first_+i)->r()[L] < v_[L]; }

    private:
        const particle_iterator first_;
        const vector_type& v_;
    };

    /**
     * Template metaprogram for recursive partitioning of each spatial
     *  dimension.  The divide and conquer algorithm is a compile-time
     *  variant of the ruler drawing example in "Algorithms in C" Parts
     *  1-4 (3E).
     */
    template<int D = dimension, int L = 0, int R = num_orthants, int M = (L+R)/2>
    struct partition_mid_recurse
    {
        template<typename OutputIteratorT>
        static void exec(particle_iterator first, const vector_type& v,
                         index_iterator lower, index_iterator upper,
                         OutputIteratorT result)
        {
            partition_mid_pred<dimension-D> pred(first, v);
            index_iterator mid = std::partition(lower, upper, pred);
            partition_mid_recurse<D-1,L,M>::exec(first, v, lower, mid, result);
            partition_mid_recurse<D-1,M,R>::exec(first, v, mid, upper, result);
            *(result+M) = particle_p_iterator(first, mid);
        }
    };

    /**
     * Specialization for the partitioning of the final dimension.
     */
    template<int L, int R, int M>
    struct partition_mid_recurse<0, L, R, M>
    {
        template<typename OutputIteratorT>
        static void exec(particle_iterator first, const vector_type& v,
                         index_iterator lower, index_iterator upper,
                         OutputIteratorT result)
        {
            partition_mid_pred<dimension-1> pred(first, v);
            index_iterator mid = std::partition(lower, upper, pred);
            *(result+M) = particle_p_iterator(first, mid);
        }
    };
    
private: // Member variables
    orthant_list orthants_;
};

template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT>
particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::particle_partition
    (particle_iterator first_e,
     particle_p_iterator first_p,
     particle_p_iterator last_p,
     int level)
{
    array_type min = first_p->r(), max = first_p->r();

    // Get the min and max values
    for (particle_p_iterator it = first_p+1; it != last_p; ++it)
    {
        const array_type r = it->r(); min = min.min(r); max = max.max(r);
    }

    partition_mid_recurse<>::exec(first_e, 0.5*(min + max), first_p.base(),
                                  last_p.base(), orthants_.begin());

    // Explicitly set the boundary elements
    orthants_.front() = first_p;
    orthants_.back()  = last_p;
}

}

#endif // TEATREE_PARTICLE_PARTITION_HPP
