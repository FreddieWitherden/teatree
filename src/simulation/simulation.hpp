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

#ifndef TEATREE_SIMULATION_SIMULATION_HPP
#define TEATREE_SIMULATION_SIMULATION_HPP

#include "simulation/progress.hpp"

#include <boost/serialization/access.hpp>
#include <boost/signals2.hpp>

#include <string>
#include <fstream>
#include <sstream>

namespace teatree
{

/**
 * Public simulation interface.
 */
class simulation
{
private: // Types
    typedef boost::signals2::signal< bool (const simulation& sim)
                                   > on_iteration_signal_type;

public: // Constructors & destructors
    simulation() {}

    simulation(const simulation_options& so)
        : completed_steps_(0)
        , so_(so)
    {}

    virtual ~simulation() {}

public: // Public methods for controlling a simulation
    /**
     * Runs the simulation.  A return value of true indicates that the
     *  simulation was explicitly broken out of (from a call to
     *  break_run).  Otherwise, if the simulation finishes of its own
     *  accord then false is returned.
     */
    bool run();

    /**
     * Returns the type of the simulation.  This is a short string
     *  that uniquely identifies a particular simulation class.
     */
    std::string type() const { return do_type(); }

    /**
     * Returns a string containing information about the run-time
     *  parameters of a simulation.
     */
    std::string parameters() const;

    /**
     * Returns the number of particles being simulated.
     */
    int num_particles() const { return do_num_particles(); }

    /**
     * Returns the number of completed iterations.
     */
    int completed_steps() const { return completed_steps_; }

    /**
     * Returns the total number of iterations.
     */
    int total_steps() const { return so_.nsteps(); }

    /**
     * Registers slot to be called after each iteration.
     */
    template<typename SlotT>
    boost::signals2::connection do_on_iteration(const SlotT& slot)
    { return on_iteration_.connect(slot); }

private: // Pure virtuals
    /**
     * Performs one iteration of the simulation.
     */
    virtual void do_iteration() = 0;

    virtual std::string do_type() const = 0;

    virtual int do_num_particles() const = 0;

    virtual void do_output(std::ostream& os, bool header) const = 0;

public: // Private methods
    void output();

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/);

private: // Members
    /// Number of completed steps
    int completed_steps_;

    /// Options
    simulation_options so_;

    /// Signal to be called upon completion of an iteration
    on_iteration_signal_type on_iteration_;
};

inline bool simulation::run()
{
    /*
     * Run the actual simulation until we either finish or are asked
     * to break.
     */
    do
    {
        // Perform the iteration
        do_iteration(); ++completed_steps_;

        // See if we should output anything
        if (so_.output_step(completed_steps_))
            output();
    } while (*on_iteration_(*this)
          && completed_steps_ != so_.nsteps());

    // If there are iterations remaining then the run was broken
    return so_.nsteps() - completed_steps_;
}

inline std::string simulation::parameters() const
{
    std::ostringstream os;
    os.precision(4);
    os << "N = " << this->num_particles() << "; "
          "N_D = "      << so_.nd()       << "; "
          "dt = "       << so_.dt()       << "; "
          "theta = "    << so_.theta()    << "; "
          "epsilon = "  << so_.epsilon();
    return os.str();
}

inline void simulation::output()
{
    // Determine the file name to output
    std::stringstream os;
    os << so_.output_basename() << "-" << completed_steps_ << ".txt";

    // Open the file
    std::ofstream of(os.str().c_str());

    // If asked, print a header with basic info about the simulation
    if (so_.output_header())
    {
        of << std::string(72, '#') << "\n";
        of << "# Teatree " TEATREE_VERSION
              " (built on " TEATREE_BUILD_DATE ") output\n";
        of << "# Simulation type " << type() << " at iter "
           << completed_steps_ << "/" << so_.nsteps() << "\n"
           << "# Parameters: " << parameters() << "\n";
        of << std::string(72, '#') << "\n";
    }

    // Delegate to do_output for the actual writing
    do_output(of, so_.output_header());
}

template<typename ArchiveT>
void simulation::serialize(ArchiveT& ar, unsigned)
{
    ar & completed_steps_;
    ar & so_;
}

}

#endif // TEATREE_SIMULATION_SIMULATION_HPP
