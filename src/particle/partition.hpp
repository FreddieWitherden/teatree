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

#ifndef TEATREE_PARTICLE_PARTITION
#define TEATREE_PARTICLE_PARTITION

#include "utils/ipow.hpp"

#include <boost/array.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <boost/swap.hpp>

namespace teatree { namespace particle_partition_
{

using boost::array;
using boost::permutation_iterator;
using boost::swap;

/**
 * Partitions up particles in N-dimensional real space into orthants.
 */
template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT>
class particle_partition
{
public: // Class types & constants
    typedef ParticleT                           particle_type;
    typedef typename particle_type::vector_type vector_type;
    typedef typename particle_type::scalar_type scalar_type;

    typedef EleIteratorT particle_iterator;
    typedef IdxIteratorT index_iterator;
    typedef permutation_iterator<particle_iterator,
                                 index_iterator> particle_p_iterator;

private:
    enum constants {
        dimension    = particle_type::dimension,
        num_orthants = ipow<2,dimension>::value
    };

    typedef array<particle_p_iterator,num_orthants+1> orthant_list;

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
                       particle_p_iterator last_p);

public: // Orthant iteration
    orthant_iterator begin() { return orthants_.begin(); }
    orthant_iterator end()   { return orthants_.end(); }

private: // Nested classes
    /**
     * Predicate for passing to std::partition.
     */
    template<int L>
    class partition_pred
    {
    public:
        partition_pred(particle_iterator first) : first_(first) {}

        bool operator()(int i, int j) const
        { return (first_+i)->r()[L] < (first_+j)->r()[L]; }

    private:
        const particle_iterator first_;
    };

    /**
     * Template metaprogram for recursive partitioning of each spatial
     *  dimension.  The divide and conquer algorithm is a compile-time
     *  variant of the ruler drawing example in "Algorithms in C" Parts
     *  1-4 (3E).
     */
    template<int D = dimension, int L = 0, int R = num_orthants, int M = (L+R)/2>
    struct partition_recurse
    {
        template<typename OutputIteratorT>
        static void exec(particle_iterator first,
                         index_iterator lower, index_iterator upper,
                         OutputIteratorT result)
        {
            partition_pred<dimension-D> pred(first);
            index_iterator mid = lower + (upper - lower)/2;
            std::nth_element(lower, mid, upper, pred);
            partition_recurse<D-1,L,M>::exec(first, lower, mid, result);
            partition_recurse<D-1,M,R>::exec(first, mid, upper, result);
            *(result+M) = particle_p_iterator(first, mid);
        }
    };

    /**
     * Specialization for the partitioning of the final dimension.
     */
    template<int L, int R, int M>
    struct partition_recurse<0, L, R, M>
    {
        template<typename OutputIteratorT>
        static void exec(particle_iterator first,
                         index_iterator lower, index_iterator upper,
                         OutputIteratorT result)
        {
            partition_pred<dimension-1> pred(first);
            index_iterator mid = lower + (upper - lower)/2;
            std::nth_element(lower, mid, upper, pred);
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
     particle_p_iterator last_p)
{
    // Perform the partitioning
    partition_recurse<>::exec(first_e, first_p.base(), last_p.base(),
                              orthants_.begin());

    // Explicitly set the boundary elements
    orthants_.front() = first_p;
    orthants_.back()  = last_p;
}

}

using particle_partition_::particle_partition;

}

#endif // TEATREE_PARTICLE_PARTITION
