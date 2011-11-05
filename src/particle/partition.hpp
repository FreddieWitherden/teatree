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

#include <boost/iterator/permutation_iterator.hpp>
#include <boost/swap.hpp>

namespace teatree { namespace particle_partition_
{

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
    typedef boost::permutation_iterator<particle_iterator,
                                        index_iterator> particle_p_iterator;

    typedef particle_p_iterator* orthant_iterator;

    enum {
        dimension    = particle_type::dimension,
        num_orthants = ipow<2,dimension>::value
    };

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
    orthant_iterator begin() { return orthants_; }
    orthant_iterator end()   { return orthants_+num_orthants+1; }

private: // Helper methods
    /**
     * Computes the component-wise median of three vectors.
     */
    vector_type median(const vector_type& l,
                       const vector_type& m,
                       const vector_type& r);

    /**
     * Computes the median of three scalars.
     */
    scalar_type median(scalar_type l,
                       scalar_type m,
                       scalar_type r);

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
    particle_p_iterator orthants_[num_orthants+1];
};

template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT>
particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::particle_partition
    (particle_iterator first_e,
     particle_p_iterator first_p,
     particle_p_iterator last_p)
{
    // Get an iterator to the middle of the range

    //std::cout << *(first_p.base()) << " " << first_p->r().transpose() << std::endl;
    //std::cout << *((last_p-1).base()) << " " << (last_p-1)->r().transpose() << std::endl;

    /*// Median of three in the first dimension
    if (first_p->r()[0] > mid_p->r()[0])
        swap(*first_p.base(), *mid_p.base());
    if (first_p->r()[0] > (last_p-1)->r()[0])
        swap(*first_p.base(), *(last_p-1).base());
    if (mid_p->r()[0] > (last_p-1)->r()[0])
        swap(*mid_p.base(), *(last_p-1).base());

    // Determine the minimum and maximum values
    acc_type_ acc[dimension];
    for (particle_p_iterator it = first_p; it != last_p; ++it)
    {
        const vector_type& v = it->r();
        for (int i = 0; i < dimension; ++i) acc[i](v[i]);
    }

    // Use these to compute a pivot vector
    const vector_type pivot;
    for (int i = 0; i < dimension; ++i)
        pivot[i] = 0.5*(min(acc[i])+max(acc[i]));*/




    /*pivot = median(first_p->r(), mid_p->r(), (last_p-1)->r());

    //std::cout << "Avg: " << partition.transpose() << std::endl;

    if (last_p - first_p == 2)
    {
        std::cout << "Pivot " << pivot.transpose() << std::endl;
        std::cout << first_p->r().transpose() << std::endl;
        std::cout << (last_p-1)->r().transpose() << std::endl;
        std::cout << partition_pred<0>(first_e,pivot)(*first_p.base())
                  << " "
                  << partition_pred<0>(first_e,pivot)(*(last_p-1).base())
                  << std::endl;
        if (first_p->r()[0] > (last_p-1)->r()[0])
            swap(*(first_p.base()), *((last_p-1).base()));
        orthants_[1] = first_p;
        orthants_[2] = (last_p-1);
        orthants_[3] = last_p;
    }
    else
    {*/
    // Perform the partitioning
    partition_recurse<>::exec(first_e, first_p.base(), last_p.base(), orthants_);
    //}
    /*partition_pred<0> predx(first_e, pivot);
    partition_pred<1> predy(first_e, pivot);

    index_iterator pivx = std::partition(first_p.base(), last_p.base(), predx);
    index_iterator pivx0 = std::partition(first_p.base(), pivx, predy);
    index_iterator pivx1 = std::partition(pivx, last_p.base(), predy);

    orthants_[1] = particle_p_iterator(first_e, pivx0);
    orthants_[2] = particle_p_iterator(first_e, pivx);
    orthants_[3] = particle_p_iterator(first_e, pivx1);*/

    // Explicitly set the tail
    orthants_[0] = first_p;
    orthants_[num_orthants] = last_p;

    // Sanity
    bool good = false;
    for (int i = 0; i < num_orthants+1; ++i)
        if (orthants_[i] != first_p && orthants_[i] != last_p)
            good = true;

    if (!good)
    {
        std::cout << "Distance " << (last_p - first_p) << std::endl;
        std::cout << first_p->r().transpose() << std::endl;
        std::cout << (last_p-1)->r().transpose() << std::endl;
        BOOST_ASSERT(good);
    }
}

template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT> inline
typename particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::vector_type
particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::median
    (const vector_type& l,
     const vector_type& m,
     const vector_type& r)
{
    vector_type ret;
    for (int i = 0; i < dimension; ++i) ret[i] = median(l[i], m[i], r[i]);
    return ret;
}

template<typename ParticleT, typename EleIteratorT, typename IdxIteratorT> inline
typename particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::scalar_type
particle_partition<ParticleT,EleIteratorT,IdxIteratorT>::median
    (scalar_type l,
     scalar_type m,
     scalar_type r)
{
    if (l > m) swap(l, m);
    if (m > r) swap(m, r);
    if (l > m) swap(l, m);
    return m;
}

}

using particle_partition_::particle_partition;

}

#endif // TEATREE_PARTICLE_PARTITION
