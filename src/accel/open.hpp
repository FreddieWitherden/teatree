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

#ifndef TEATREE_ACCEL_OPEN_HPP
#define TEATREE_ACCEL_OPEN_HPP

#include "particle/make_pseudo_particle.hpp"
#include "particle/typedefs.hpp"
#include "simulation/options.hpp"

#include <boost/iterator/counting_iterator.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>

#include <vector>

namespace teatree
{

template<typename EfieldT>
class accel_open
{
public: // Types & constants
    typedef EfieldT efield_type;

    TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(typename EfieldT::pseudo_particle_type);

    enum constants {
        dimension       = pseudo_particle_type::dimension,
        multipole_order = pseudo_particle_type::multipole_order
    };

public: // Constructors
    accel_open() {}

    accel_open(const simulation_options& so)
        : so_(so)
    {}

    template<typename RandomInputRangeT, typename RandomOutputRangeT>
    void operator()(scalar_type t,
                    const RandomInputRangeT& in,
                    RandomOutputRangeT& out);

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/);

private: // Member variables
    std::vector<int> idx_;
    simulation_options so_;
};

template<typename EfieldT>
template<typename RandomInputRangeT, typename RandomOutputRangeT>
void accel_open<EfieldT>::operator()(scalar_type t,
                                     const RandomInputRangeT& in,
                                     RandomOutputRangeT& out)
{
    using boost::counting_iterator;

    const int N = in.size();
    const scalar_type inv_dimnd = 1.0/(dimension*so_.nd());

    // Ensure we have a valid list of indexes
    if (idx_.empty())
        idx_.assign(counting_iterator<int>(0), counting_iterator<int>(N));

    // Build the tree to yield a pseudo particle
    const pseudo_particle_type pp = make_pseudo_particle<multipole_order>
                                        (in.begin(), idx_.begin(), idx_.end());

    // Iterate over each particle in the tree and compute the acceleration
    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < N; ++i)
    {
        const particle_type& p = in[i];
        const efield_type ef(p.r(), so_);

        out[i] = inv_dimnd*p.qtom()*pp.visit_children(ef);
    }
}

template<typename EfieldT>
template<typename ArchiveT>
void accel_open<EfieldT>::serialize(ArchiveT& ar, unsigned)
{
    ar & idx_;
    ar & so_;
}

}

#endif // TEATREE_ACCEL_OPEN_HPP