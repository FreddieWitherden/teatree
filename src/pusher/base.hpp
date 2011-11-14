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

#ifndef TEATREE_PUSHER_BASE_HPP
#define TEATREE_PUSHER_BASE_HPP

#include "particle/typedefs.hpp"

#include <vector>

#include <boost/range/algorithm/copy.hpp>
#include <boost/swap.hpp>

/**
 * Works around the dependant template name resolution `feature' of C++
 *  which prevents template sub-classes from being able to access the
 *  typedef's and members in a base class.
 */
#define TEATREE_PUSHER_GENERATE_TYPEDEFS(AccelEvalT)                       \
    typedef pusher_base<AccelEvalT> base_type;                             \
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(typename base_type::particle_type); \
    typedef typename base_type::random_access_range random_access_range;   \
    using base_type::accel_eval;                                           \
    using base_type::acceleval_;                                           \
    using base_type::dt_;                                                  \
    using base_type::t_


namespace teatree { namespace pusher_base_
{

namespace rng = boost::range;

/**
 * Base particle pusher class.
 */
template<typename AccelEvalT>
class pusher_base
{
public: // Types
    typedef typename AccelEvalT::particle_type  particle_type;
    typedef typename particle_type::scalar_type scalar_type;
    typedef typename particle_type::vector_type vector_type;

protected:
    typedef std::vector<particle_type> random_access_range;

    typedef AccelEvalT accel_eval_type;

public: // Constructors
    template<typename ForwardRangeT>
    pusher_base(const ForwardRangeT& in,
                accel_eval_type acceleval, scalar_type t0, scalar_type dt)
        : nparticles_(in.size())
        , acceleval_(acceleval)
        , nacceleval_(0), nsteps_(0)
        , t_(t0), dt_(dt)
        , pcurr_(in.begin(), in.end())
        , ptemp_(in.begin(), in.end())
    {}

    virtual ~pusher_base() {}

public: // Accessors
    /**
     * Returns the number of acceleration evaluations thus far.
     */
    int nacceleval() { return nacceleval_; }

    /**
     * Returns the current simulation time.
     */
    scalar_type t() { return t_; }

    /**
     * Returns the time-step being used for the simulation.
     */
    scalar_type dt() { return dt_; }

    /**
     * Returns the number of steps which have been taken thus far.
     */
    int nsteps() { return nsteps_; }

public: // Simulation control
    /**
     * Advances the simulation to a time >= newt and places the resulting
     *  particles into the passed output iterator.
     */
    template<typename OutputItT>
    void advance(scalar_type newt, OutputItT out);

protected: // Virtuals

    /**
     *
     */
    virtual void take_step(const random_access_range& in,
                           random_access_range& out) = 0;

    template<typename RandomInputRangeT, typename RandomOutputRangeT>
    void accel_eval(scalar_type t,
                    const RandomInputRangeT& in,
                    RandomOutputRangeT& out)
    { acceleval_(t, in, out); ++nacceleval_; }

protected: // Protected members
    /// The number of particles (equations) being pushed.
    const int nparticles_;

    accel_eval_type acceleval_;

    /// The number of acceleration evaluations thus far.
    int nacceleval_;

    /// The number of steps, each of length dt, taken thus far.
    int nsteps_;

    /// The current simulation time.
    scalar_type t_;

    /// The simulation time step.
    const scalar_type dt_;

private: // Members variables
    std::vector<particle_type> pcurr_;
    std::vector<particle_type> ptemp_;
};

template<typename AccelEvalT>
template<typename OutputItT>
void pusher_base<AccelEvalT>::advance(scalar_type newt, OutputItT out)
{
    while (t_ < newt)
    {
        // Take a step using the solver
        take_step(pcurr_, ptemp_);

        // Update the current time
        t_ = ++nsteps_ * dt_;

        // Swizzle the current and temp vectors
        boost::swap(pcurr_, ptemp_);
    }

    // Copy the output to the output iterator
    rng::copy(pcurr_, out);
}

}

using pusher_base_::pusher_base;

}

#endif // TEATREE_PUSHER_BASE_HPP
