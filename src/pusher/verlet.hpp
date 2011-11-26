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

#ifndef TEATREE_PUSHER_VERLET_HPP
#define TEATREE_PUSHER_VERLET_HPP

#include "pusher/base.hpp"

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/split_member.hpp>

#include <vector>

namespace teatree
{

/**
 * A second-order Störmer-Verlet integrator.
 */
template<typename AccelEvalT>
class pusher_verlet : public pusher_base<AccelEvalT>
{
public:
    TEATREE_PUSHER_GENERATE_TYPEDEFS(AccelEvalT);

    pusher_verlet() {}

    template<typename ForwardRangeT>
    pusher_verlet(const ForwardRangeT& in,
                  AccelEvalT acceleval, scalar_type t0, scalar_type dt)
        : base_type(in, acceleval, t0, dt)
        , accel_(this->nparticles_)
        , tmp_(this->nparticles_)
    {}

public:
    void take_step(const random_access_range& in,
                   random_access_range& out);

private: // Serialization
    friend class boost::serialization::access;

    BOOST_SERIALIZATION_SPLIT_MEMBER();

    template<typename ArchiveT>
    void save(ArchiveT& ar, unsigned /*file version*/) const;

    template<typename ArchiveT>
    void load(ArchiveT& ar, unsigned /*file version*/);

private:
    std::vector<vector_type> accel_;
    std::vector<particle_type> tmp_;
};


template<typename AccelEvalT>
void pusher_verlet<AccelEvalT>::take_step
    (const random_access_range& in,
     random_access_range& out)
{
    // Evaluate the acceleration at time t
    accel_eval(t_, in, accel_);

    // Update the positions and velocities
    for (int i = 0; i < this->nparticles_; ++i)
    {
        tmp_[i].v() = in[i].v() + dt_/2*accel_[i];
        tmp_[i].r() = in[i].r() + dt_*tmp_[i].v();
    }

    // Do a second evaluation to get it at t+dt
    accel_eval(t_+dt_, tmp_, accel_);

    // Do the final update
    for (int i = 0; i < this->nparticles_; ++i)
    {
        out[i].r() = tmp_[i].r();
        out[i].v() = tmp_[i].v() + dt_/2*accel_[i];
    }
}

template<typename AccelEvalT>
template<typename ArchiveT>
void pusher_verlet<AccelEvalT>::save(ArchiveT& ar, unsigned) const
{
    ar << boost::serialization::base_object<base_type>(*this);
}

template<typename AccelEvalT>
template<typename ArchiveT>
void pusher_verlet<AccelEvalT>::load(ArchiveT& ar, unsigned)
{
    ar >> boost::serialization::base_object<base_type>(*this);

    // Resize our member vectors
    accel_.resize(this->nparticles_);
    tmp_.resize(this->nparticles_);
}

}

#endif // TEATREE_PUSHER_VERLET_HPP
