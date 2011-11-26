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

#ifndef TEATREE_SIMULATION_OPTIONS_HPP
#define TEATREE_SIMULATION_OPTIONS_HPP

#include <boost/serialization/access.hpp>

#include <stdexcept>

namespace teatree
{

/**
 * Provides storage for runtime simulation options.
 */
class simulation_options
{
public: // Accessors
    double epsilon() const { return epsilon_; }
    double theta()   const { return theta_; }

public: // Settors
    simulation_options& epsilon(double s)
    {
        if (s <= 0) throw std::invalid_argument("Bad epsilon");
        epsilon_ = s; return *this;
    }

    simulation_options& theta(double s)
    {
        if (s < 0.0 || s >= 1.0) throw std::invalid_argument("Bad theta");
        theta_ = s; return *this;
    }

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/)
    {
        ar & epsilon_;
        ar & theta_;
    }

private: // Members
    double epsilon_;
    double theta_;
};



}

#endif // TEATREE_SIMULATION_OPTIONS_HPP
