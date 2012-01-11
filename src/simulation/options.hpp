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
#include <boost/serialization/set.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace teatree
{

/**
 * Provides storage for runtime simulation options.
 */
class simulation_options
{
public: // Constructors
    simulation_options()
        : epsilon_(1.0e-6)
        , dt_(1.0e-4)
        , theta_(0.3)
        , nd_(1000)
        , output_header_(true)
    {
        //
    }

public: // Accessors
    double epsilon()     const { return epsilon_; }
    double dt()          const { return dt_;    }
    double theta()       const { return theta_; }
    double nd()          const { return nd_; }
    double qtomcutoff()     const { return qtomcutoff_; }
    bool output_header() const { return output_header_; }

    int    nsteps()  const
    { return output_steps_.empty() ? 0 : *output_steps_.rbegin(); }

    const std::string& output_basename() const { return output_basename_; }

    bool output_step(int s) const { return output_steps_.count(s); }

public: // Settors
    simulation_options& epsilon(double s);
    simulation_options& dt(double s);
    simulation_options& theta(double s);
    simulation_options& nd(double s);
    simulation_options& qtomcutoff(double s);
    simulation_options& output_header(bool s);
    simulation_options& output_steps(const std::set<int>& s);
    simulation_options& output_basename(const std::string& s);

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/);

private: // Members
    double epsilon_;
    double dt_;
    double theta_;
    double nd_;
    double qtomcutoff_;
    bool   output_header_;
    std::string output_basename_;
    std::set<int> output_steps_;
};

simulation_options& simulation_options::epsilon(double s)
{
    if (s <= 0) throw std::invalid_argument("bad epsilon");
    epsilon_ = s; return *this;
}

simulation_options& simulation_options::dt(double s)
{
    if (s <= 0) throw std::invalid_argument("bad dt");
    dt_ = s; return *this;
}

simulation_options& simulation_options::theta(double s)
{
    if (s < 0.0) throw std::invalid_argument("bad theta");
    theta_ = s; return *this;
}

simulation_options& simulation_options::nd(double s)
{
    if (s <= 0.0) throw std::invalid_argument("bad nd");
    nd_ = s; return *this;
}

simulation_options& simulation_options::qtomcutoff(double s)
{
    if (s <= 0.0) throw std::invalid_argument("bad q-to-m cutoff");
    qtomcutoff_ = s; return *this;
}

simulation_options& simulation_options::output_header(bool s)
{
    output_header_ = s; return *this;
}

simulation_options& simulation_options::output_steps(const std::set<int>& s)
{
    std::set<int> ns(s);
    output_steps_.swap(ns);

    if (output_steps_.empty())
        throw std::invalid_argument("no output steps provided");
    if (*output_steps_.begin() <= 0)
        throw std::invalid_argument("bad output step provided");
    return *this;
}

simulation_options& simulation_options::output_basename(const std::string& s)
{
    if (s.length() == 0) throw std::invalid_argument("bad output base name");
    output_basename_ = s; return *this;
}

template<typename ArchiveT>
void simulation_options::serialize(ArchiveT& ar, unsigned)
{
    ar & epsilon_;
    ar & dt_;
    ar & theta_;
    ar & nd_;
    ar & qtomcutoff_;
    ar & output_header_;
    ar & output_steps_;
    ar & output_basename_;
}

}

#endif // TEATREE_SIMULATION_OPTIONS_HPP
