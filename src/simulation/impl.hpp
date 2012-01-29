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

#ifndef TEATREE_SIMULATION_IMPL_HPP
#define TEATREE_SIMULATION_IMPL_HPP

#include "particle/particle.hpp"
#include "particle/pseudo_particle.hpp"
#include "simulation/simulation.hpp"
#include "utils/vector_traits.hpp"

#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <boost/cstdint.hpp>
#include <boost/range/istream_range.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/tuple/tuple.hpp>

namespace teatree
{

template< typename ScalarT
        , int Dim
        , int MulP
        , template<typename, typename, typename> class MacT
        , template<typename, template<typename, typename, typename> class, int> class EfieldT
        , template<typename> class AccelEvalT
        , template<typename> class PusherT
        >
class simulation_impl : public simulation
{
public: // Types & constants
    typedef ScalarT scalar_type;
    typedef typename vector_traits<scalar_type,Dim>::type vector_type;

    typedef particle<vector_type>                   particle_type;
    typedef pseudo_particle<particle_type,MulP>     pseudo_particle_type;

    typedef EfieldT<pseudo_particle_type,MacT,MulP> efield_type;

    typedef typename efield_type::mac_type          mac_type;

    typedef AccelEvalT<efield_type>                 accel_eval_type;
    typedef PusherT<accel_eval_type>                pusher_type;

    enum constants {
        dimension       = pseudo_particle_type::dimension,
        multipole_order = pseudo_particle_type::multipole_order
    };

public: // Constructors
    /**
     * Default constructor.
     */
    simulation_impl() { init(); }

    /**
     * Normal constructor; reads particles from is uses these particles
     *  along with the provided simulation options to construct a
     *  simulation.
     */
    simulation_impl(std::istream& is,
                    const simulation_options& so)
        : simulation(so)
        , particle_pusher_(boost::istream_range<particle_type>(is),
                           accel_eval_type(so), 0.0, so.dt())
    { init(); }

private: // Constructor helpers
    void init()
    {
        using boost::bind;

        // Register our slot for monitoring acceleration timings
        accel_eval_type& ae = particle_pusher_.acceleval();
        ae.connect_accel_timings(bind(&simulation_impl::accel_timings_slot,
                                      this, _1, _2, _3, _4));
    }

private: // Virtual methods
    iteration_stats do_iteration()
    {
        using namespace boost::chrono;
        using boost::make_tuple;

        // Zero the counters (to be set during advance() by our slot)
        tree_time_ = particle_time_ = 0.0;
        particles_visited_ = pparticles_visited_ = 0;

        // Get the number of acceleration evals before advancing
        const int init_neval = particle_pusher_.num_acceleval();

        // Get the current time
        const steady_clock::time_point start_time = steady_clock::now();

        // Advance the system
        particle_pusher_.advance();

        // Determine how much time has elapsed
        const duration<double> net_push_time = steady_clock::now() - start_time;

        // Generate the iteration statistics; see accel_timings_slot
        return make_tuple(tree_time_,
                          particle_time_,
                          net_push_time.count() - tree_time_ - particle_time_,
                          particles_visited_,
                          pparticles_visited_,
                          particle_pusher_.num_acceleval() - init_neval);
    }

    int do_num_particles() const { return particle_pusher_.num_particles(); }

    void do_output(std::ostream& os, bool header) const
    {
        // File header
        if (header)
            os << particle_format_header<particle_type>() << "\n";

        // Data
        particle_pusher_.output(std::ostream_iterator<particle_type>(os, "\n"));
    }

private: // Slots
    void accel_timings_slot(scalar_type tree, scalar_type vis,
                            int64_t pvis, int64_t ppvis)
    {
        tree_time_          += tree;
        particle_time_      += vis;
        particles_visited_  += pvis;
        pparticles_visited_ += ppvis;
    }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file_version*/)
    {
        ar & boost::serialization::base_object<simulation>(*this);
        ar & particle_pusher_;
    }

private: // Members
    pusher_type particle_pusher_;

    // Temporaries used to hold the statistics from AccelEvalT
    double tree_time_, particle_time_;
    int64_t particles_visited_, pparticles_visited_;
};

}

#endif // TEATREE_SIMULATION_IMPL_HPP
