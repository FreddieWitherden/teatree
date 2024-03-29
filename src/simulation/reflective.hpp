/*
    This file is part of teatree.
    Copyright (C) 2012  Freddie Witherden <freddie@witherden.org>

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

#ifndef TEATREE_SIMULATION_REFLECTIVE_HPP
#define TEATREE_SIMULATION_REFLECTIVE_HPP

#include "accel/open.hpp"
#include "constraint/reflective.hpp"
#include "simulation/impl.hpp"
#include "simulation/traits.hpp"
#include "utils/name_traits.hpp"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>

namespace teatree
{

template< typename ScalarT
        , int Dim
        , int MulP
        , template<typename, typename, typename> class MacT
        , template<typename, template<typename, typename, typename> class, int> class EfieldT
        , template<typename, typename> class PusherT
        >
class simulation_reflective : public simulation_impl< ScalarT
                                                    , Dim
                                                    , MulP
                                                    , MacT
                                                    , EfieldT
                                                    , accel_open
                                                    , constraint_reflective
                                                    , PusherT
                                                    >
{
public: // Types
    typedef simulation_impl< ScalarT
                           , Dim
                           , MulP
                           , MacT
                           , EfieldT
                           , accel_open
                           , constraint_reflective
                           , PusherT> base_type;

public: // Constructors
    simulation_reflective() {}

    simulation_reflective(std::istream& is, const simulation_options& so)
        : base_type(is, so)
    {}

private: // Virtual methods
    std::string do_type() const
    { return simulation_traits<simulation_reflective>::type(); }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file_version*/)
    {
        ar & boost::serialization::base_object<base_type>(*this);
    }
};

template< typename S
        , int D
        , int Mu
        , template<typename, typename, typename> class M
        , template<typename, template<typename, typename, typename> class, int> class E
        , template<typename, typename> class P
        >
struct name_traits<simulation_reflective<S,D,Mu,M,E,P> >
{
    static std::string abbr() { return "R"; }
    static std::string name() { return "Reflective"; }
};

}

#endif // TEATREE_SIMULATION_REFLECTIVE_HPP
