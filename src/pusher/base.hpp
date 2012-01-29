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

#ifndef TEATREE_PUSHER_BASE_HPP
#define TEATREE_PUSHER_BASE_HPP

#include "particle/typedefs.hpp"

#include <vector>

#include <boost/array.hpp>
#include <boost/foreach.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/swap.hpp>

/**
 * Works around the dependant template name resolution `feature' of C++
 *  which prevents template sub-classes from being able to access the
 *  typedef's and members in a base class.
 */
#define TEATREE_PUSHER_GENERATE_TYPEDEFS(BaseT)                            \
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(typename BaseT::particle_type);     \
    typedef typename BaseT::particle_range particle_range;                 \
    typedef typename BaseT::accel_range accel_range

namespace teatree { namespace pusher_base_
{

namespace rng = boost::range;

/**
 * Base particle pusher class.
 */
template<typename AccelEvalT, int NAccel>
class pusher_base
{
public: // Types
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(typename AccelEvalT::particle_type);

protected:
    typedef std::vector<particle_type> particle_range;
    typedef std::vector<vector_type> accel_range;

    typedef AccelEvalT accel_eval_type;

public: // Constructors
    pusher_base() {}

    template<typename ForwardRangeT>
    pusher_base(const ForwardRangeT& in,
                const accel_eval_type& acceleval,
                scalar_type t0, scalar_type dt)
        : pcurr_(in.begin(), in.end())
        , ptemp_(pcurr_)
        , num_acceleval_(0), num_steps_(0)
        , t0_(t0), dt_(dt)
        , acceleval_(acceleval)
    { BOOST_FOREACH(accel_range& a, atemp_) a.resize(this->num_particles()); }

    virtual ~pusher_base() {}

public: // Accessors
    /**
     * Returns the current simulation time.
     */
    scalar_type t() const { return t0_ + num_steps_*dt_; }

    /**
     * Returns the time-step being used for the simulation.
     */
    scalar_type dt() const { return dt_; }

    /**
     * Returns the number of steps which have been taken thus far.
     */
    int num_steps() const { return num_steps_; }

    /**
     * Returns the number of particles being pushed.
     */
    int num_particles() const { return pcurr_.size(); }

    /**
     * Returns the number of acceleration evaluations thus far.
     */
    int num_acceleval() const { return num_acceleval_; }

public: // Simulation control
    /**
     * Advances the simulation by a time step dt.
     */
    void advance();

    /**
     *
     */
    template<typename OutputItT>
    void output(OutputItT out) const
    { rng::copy(pcurr_, out); }

    accel_eval_type& acceleval() { return acceleval_; }

private: // Pure virtuals
    /**
     *
     */
    virtual void take_step(const particle_range& in, particle_range& out) = 0;

protected: // Internal hierarchy methods
    template<typename RandomInputRangeT, typename RandomOutputRangeT>
    void accel_eval(scalar_type t,
                    const RandomInputRangeT& in,
                    RandomOutputRangeT& out)
    { acceleval_(t, in, out); ++num_acceleval_; }

    accel_range& get_accel_range(int i) { return atemp_[i]; }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/);

private: // Member variables
    std::vector<particle_type> pcurr_;
    std::vector<particle_type> ptemp_;

    boost::array<accel_range,NAccel> atemp_;

    /// The number of acceleration evaluations thus far.
    int num_acceleval_;

    /// The number of steps, each of length dt, taken thus far.
    int num_steps_;

    /// The starting simulation time
    scalar_type t0_;

    /// The simulation time step.
    scalar_type dt_;

    // Acceleration evaluator
    accel_eval_type acceleval_;
};

template<typename AccelEvalT, int NEval>
void pusher_base<AccelEvalT,NEval>::advance()
{
    // Take a step using the solver
    take_step(pcurr_, ptemp_);

    // Update the number of steps taken
    ++num_steps_;

    // Swizzle the current and temp vectors
    boost::swap(pcurr_, ptemp_);
}

template<typename AccelEvalT, int NEval>
template<typename ArchiveT>
void pusher_base<AccelEvalT,NEval>::serialize(ArchiveT& ar, unsigned)
{
    ar & acceleval_;
    ar & num_acceleval_;
    ar & num_steps_;
    ar & t0_;
    ar & dt_;
    ar & pcurr_;

    // Copy pcurr_ into ptemp_ for the ->q() and ->m() members
    if (ArchiveT::is_loading::value)
    {
        ptemp_ = pcurr_;
        BOOST_FOREACH(accel_range& a, atemp_) a.resize(this->num_particles());
    }
}

}

using pusher_base_::pusher_base;

}

#endif // TEATREE_PUSHER_BASE_HPP
