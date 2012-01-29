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

#ifndef TEATREE_ACCEL_OPEN_HPP
#define TEATREE_ACCEL_OPEN_HPP

#include "particle/make_pseudo_particle.hpp"
#include "particle/typedefs.hpp"
#include "simulation/options.hpp"

#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

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

    typedef boost::signals2::signal< void (scalar_type, scalar_type,
                                           int64_t, int64_t)
                                   > accel_timings_signal_type;

public: // Constructors
    accel_open() : accel_timings_(new accel_timings_signal_type) {}

    accel_open(const simulation_options& so)
        : so_(so)
        , accel_timings_(new accel_timings_signal_type)
    {}

    template<typename SlotT>
    boost::signals2::connection connect_accel_timings(const SlotT& slot)
    { return accel_timings_->connect(slot); }

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
    boost::shared_ptr<accel_timings_signal_type> accel_timings_;
};

template<typename EfieldT>
template<typename RandomInputRangeT, typename RandomOutputRangeT>
void accel_open<EfieldT>::operator()(scalar_type t,
                                     const RandomInputRangeT& in,
                                     RandomOutputRangeT& out)
{
    using namespace boost::chrono;
    using boost::counting_iterator;

    // Convenience values
    const int N = in.size();
    const scalar_type inv_dimnd = 1.0/(dimension*so_.nd());

    // Ensure we have a valid list of indexes
    if (idx_.empty())
        idx_.assign(counting_iterator<int>(0), counting_iterator<int>(N));

    // Save the time when we started building the tree
    const steady_clock::time_point start_time = steady_clock::now();

    // Build the tree to yield a pseudo particle
    const pseudo_particle_type pp = make_pseudo_particle<multipole_order>
                                        (in.begin(), idx_.begin(), idx_.end());

    // Save the time when we finished building the three
    const steady_clock::time_point tree_time = steady_clock::now();

    // Keep track of the number of particles/pseudo particles visited
    int64_t pvis = 0, ppvis = 0;

    // Iterate over each particle in the tree and compute the acceleration
    #pragma omp parallel for schedule(dynamic) reduction(+:pvis,ppvis)
    for (int i = 0; i < N; ++i)
    {
        const particle_type& p = in[i];

        // Ignore particles with a charge-to-mass-ratio below the cutoff
        if (std::fabs(in[i].qtom()) < so_.qtomcutoff())
        {
            out[i] = vector_type::Zero();
        }
        else
        {
            const efield_type ef(p.r(), so_);

            out[i] = inv_dimnd*p.qtom()*pp.visit_children(ef);
            pvis += ef.leaves_visited(); ppvis += ef.branches_visited();
        }
    }

    // Compute times and fire off our signal
    const duration<double> tree_dur  = tree_time - start_time;
    const duration<double> visit_dur = steady_clock::now() - tree_time;
    (*accel_timings_)(tree_dur.count(), visit_dur.count(), pvis, ppvis);
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
