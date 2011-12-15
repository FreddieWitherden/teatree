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

#ifndef TEATREE_PUSHER_COMP_BASE_HPP
#define TEATREE_PUSHER_COMP_BASE_HPP

#include "pusher/base.hpp"
#include "utils/name_traits.hpp"

#include <boost/array.hpp>
#include <boost/phoenix/core.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/numeric.hpp>
#include <boost/serialization/base_object.hpp>

#include <vector>

namespace teatree
{

/**
 *
 */
template< typename AccelEvalT
        , template<typename> class CoeffsT
        >
class pusher_comp_base : public pusher_base<AccelEvalT,1>
{
public: // Types & constants
    typedef pusher_base<AccelEvalT,1> base_type;
    TEATREE_PUSHER_GENERATE_TYPEDEFS(base_type);

private:
    typedef CoeffsT<scalar_type> coeff_type;

    enum {
        // As the coefficients are symmetric we have 2*N-1 of them in total
        Ncoeff = 2*coeff_type::N - 1
    };

private: // Private methods
    void init_coeffs(scalar_type dt);

public: // Constructors
    pusher_comp_base() {}

    template<typename ForwardRangeT>
    pusher_comp_base(const ForwardRangeT& in,
                     const AccelEvalT& acceleval, scalar_type t0, scalar_type dt)
        : base_type(in, acceleval, t0, dt)
    { init_coeffs(dt); }

private: // Concrete implementations of pure virtuals from pusher_base
    void take_step(const particle_range& in, particle_range& out);

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file_version*/);

private: // Members
    boost::array<scalar_type,Ncoeff> hg_;
    boost::array<scalar_type,Ncoeff+1> hgp_;
};

template<typename AccelEvalT, template<typename> class CoeffsT>
void pusher_comp_base<AccelEvalT,CoeffsT>::init_coeffs(scalar_type dt)
{
    using boost::phoenix::arg_names::arg1;
    using boost::phoenix::arg_names::arg2;
    using boost::phoenix::val;

    // Get our expanded coefficients
    boost::array<scalar_type,Ncoeff> c;
    boost::copy(coeff_type::coeffs(), c.begin());
    boost::copy(coeff_type::coeffs(), c.rbegin());

    // Compute the position and velocity time-steps
    boost::transform(c, hg_.begin(), val(dt)*arg1);
    boost::adjacent_difference(hg_, hgp_.begin(), 0.5*(arg1 + arg2));

    // Fix-up the first and last velocity steps
    hgp_.front() = 0.5*hg_.front();
    hgp_.back()  = 0.5*hg_.back();
}

template<typename AccelEvalT, template<typename> class CoeffsT>
void pusher_comp_base<AccelEvalT,CoeffsT>::take_step(const particle_range& in,
                                                     particle_range& out)
{
    // Copy over in to out so we can work in-place
    boost::copy(in, out.begin());

    // For convenience
    const int N = this->num_particles();
    accel_range& accel = this->get_accel_range(0);
    
    // Use the current velocities to advance the positions
    for (int i = 0; i < N; ++i)
        out[i].r() += hgp_[0]*out[i].v();

    scalar_type t = this->t() + hgp_[0];
    
    for (int i = 0; i < Ncoeff; ++i)
    {
        // Evaluate the acceleration at the time t into accel_
        this->accel_eval(t, out, accel);

        // Update the velocities and positions
        for (int j = 0; j < N; ++j)
        {
            out[j].v() += hg_[i]*accel[j];
            out[j].r() += hgp_[i+1]*out[j].v();
        }

        // Update the time
        t += hgp_[i+1];
    }
}

template<typename AccelEvalT, template<typename> class CoeffsT>
template<typename ArchiveT>
void pusher_comp_base<AccelEvalT,CoeffsT>::serialize(ArchiveT& ar, unsigned)
{
    ar & boost::serialization::base_object<base_type>(*this);

    if (ArchiveT::is_loading::value)
    {
        init_coeffs(this->dt());
    }
}

/**
 * Macro to generate a composition method with a class name of name
 *  using coefficients from the coeffs class/structure.
 */
#define TEATREE_PUSHER_GENERATE_COMP(name,coeffs)                        \
    template<typename AccelEvalT>                                        \
    class name : public pusher_comp_base<AccelEvalT,coeffs>              \
    {                                                                    \
    public:                                                              \
        typedef pusher_comp_base<AccelEvalT,coeffs> base_type;           \
        TEATREE_PUSHER_GENERATE_TYPEDEFS(base_type);                     \
                                                                         \
        name() {}                                                        \
                                                                         \
        template<typename ForwardRangeT>                                 \
        name(const ForwardRangeT& in, AccelEvalT acceleval,              \
             scalar_type t0, scalar_type dt)                             \
            : pusher_comp_base<AccelEvalT,coeffs>(in, acceleval, t0, dt) \
        {}                                                               \
    }

}

#endif // TEATREE_PUSHER_COMP_BASE_HPP
