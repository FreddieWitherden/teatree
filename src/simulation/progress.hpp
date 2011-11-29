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

#ifndef TEATREE_SIMULATION_PROGRESS_HPP
#define TEATREE_SIMULATION_PROGRESS_HPP

#include <boost/assert.hpp>
#include <boost/format.hpp>
#include <boost/chrono.hpp>
#include <boost/serialization/access.hpp>
#include <boost/utility.hpp>

#include <iostream>
#include <string>

namespace teatree { namespace simulation_progress_
{

using namespace boost::chrono;

/**
 * Tracks the progress of a simulation.
 */
template<typename ChronoClockT = steady_clock>
class simulation_progress : private boost::noncopyable
{
private: // Types & constants
    typedef typename ChronoClockT::time_point time_point;

public: // Constructors
    simulation_progress(std::ostream& os = std::cerr,
                        int max_refresh_hz = 10);

    simulation_progress(int total_iter,
                        int start_iter,
                        std::ostream& os = std::cerr,
                        int max_refresh_hz = 10);

    ~simulation_progress() { if (os_.good()) os_ << std::endl; }

public: // Operators
    int operator+=(int incr);
    int operator++() { return operator+=(1); }

    int completed() const { return curr_iter_; }
    int remaining() const { return total_iter_ - curr_iter_; }
    int total()     const { return total_iter_; }

    duration<double> time_elapsed() const
    { return ChronoClockT::now() - start_; }

    duration<double> time_remaining() const
    { return static_cast<double>(remaining())/elapsed_iter_ * time_elapsed(); }


private: // Private methods
    void display();

    template<typename DurationT>
    void display_hms(boost::format& fmt, const DurationT& d);

private: // Serialization
    friend class boost::serialization::access;

    template<typename ArchiveT>
    void serialize(ArchiveT& ar, unsigned /*file version*/);

private: // Members
    /// When we started the simulation
    const time_point start_;

    /// The number of completed iterations
    int curr_iter_;

    /// The number of iterations completed since start_
    int elapsed_iter_;

    /// The expected (total) number of iterations
    const int total_iter_;

    /// Output format string
    boost::format disp_fmt_;

    /// Stream to output to
    std::ostream& os_;

    /// Number of columns to use for the progress bar
    const int nprogbarcol_;

    /// Number of columns in the terminal
    const int ncol_;

    /// Minimum time between display updates
    const duration<double> min_refresh_duration_;

    /// Time of the last update
    time_point last_display_time_;
};

template<typename ChronoClockT>
simulation_progress<ChronoClockT>::simulation_progress(std::ostream& os,
                                                       int max_refresh_hz)
   : start_(ChronoClockT::now())
   , curr_iter_(0)
   , elapsed_iter_(0)
   , total_iter_(100)
   , disp_fmt_("%|6.1f|%% [%s%s>%s] %d/%d "
               "ela: %02d:%02d:%02d rem: %02d:%02d:%02d")
   , os_(os)
   , nprogbarcol_(30)
   , ncol_(80)
   , min_refresh_duration_(1.0/max_refresh_hz)
{
    BOOST_ASSERT(max_refresh_hz > 0);
}

template<typename ChronoClockT>
simulation_progress<ChronoClockT>::simulation_progress(int total_iter,
                                                       int start_iter,
                                                       std::ostream& os,
                                                       int max_refresh_hz)
    : start_(ChronoClockT::now())
    , curr_iter_(start_iter)
    , elapsed_iter_(0)
    , total_iter_(total_iter)
    , disp_fmt_("%|6.1f|%% [%s%s>%s] %d/%d "
                "ela: %02d:%02d:%02d rem: %02d:%02d:%02d")
    , os_(os)
    , nprogbarcol_(30)
    , ncol_(80)
    , min_refresh_duration_(1.0/max_refresh_hz)
{
    BOOST_ASSERT(max_refresh_hz > 0);
}

template<typename ChronoClockT>
int simulation_progress<ChronoClockT>::operator+=(int incr)
{
    // Ensure the count is in range
    BOOST_ASSERT(curr_iter_ + incr <= total_iter_);

    // Increment the counters
    curr_iter_    += incr;
    elapsed_iter_ += incr;

    // Display
    display();

    return curr_iter_;
}

template<typename ChronoClockT>
void simulation_progress<ChronoClockT>::display()
{
    const time_point now = ChronoClockT::now();

    // If we updated recently then do not update again
    if ((now - last_display_time_) < min_refresh_duration_
     && curr_iter_ != total_iter_)
        return;

    // Output the percentage of the simulation we've completed
    disp_fmt_ % (100.0*static_cast<double>(curr_iter_)/total_iter_);

    // Decide how many '+', '=' and ' ' to output for the progress bar
    const int n   = nprogbarcol_ - 1;
    const int nps = n * (curr_iter_ - elapsed_iter_)/total_iter_;
    const int neq = n * elapsed_iter_/total_iter_;
    const int nsp = n - nps - neq;

    // Build suitable strings and output
    disp_fmt_ % std::string(nps, '+')
              % std::string(neq, '=')
              % std::string(nsp, ' ');

    // Display the iterations completed against the total
    disp_fmt_ % curr_iter_ % total_iter_;

    // Compute and display the elapsed time
    display_hms(disp_fmt_, time_elapsed());

    // If we have done some iterations then estimate the time remaining
    if (elapsed_iter_ > 0)
        display_hms(disp_fmt_, time_remaining());
    // Otherwise just blank out the remaining time segment
    else
        disp_fmt_ % "--" % "--" % "--";

    // Clear the terminal if we have already printed something
    if (last_display_time_ > time_point())
        os_ << '\r';

    // Output the progress to the terminal
    os_ << disp_fmt_;

    // Pad the remaining space with spaces (this prevents ghosting)
    os_ << std::string(ncol_ - disp_fmt_.size(), ' ');

    // Flush the stream
    os_.flush();

    // Save the time of this update
    last_display_time_ = now;
}

template<typename ChronoClockT>
template<typename DurationT> inline
void simulation_progress<ChronoClockT>::display_hms(boost::format& fmt,
                                                    const DurationT& d)
{
    const hours   h = duration_cast<hours>(d);
    const minutes m = duration_cast<minutes>(d - h);
    const seconds s = duration_cast<seconds>(d - h - m);

    fmt % h.count() % m.count() % s.count();
}

template<typename ChronoClockT>
template<typename ArchiveT>
void simulation_progress<ChronoClockT>::serialize(ArchiveT& ar, unsigned)
{
    ar & curr_iter_;
    ar & const_cast<int&>(total_iter_);
}

}

using simulation_progress_::simulation_progress;

}

#endif // TEATREE_SIMULATION_PROGRESS_HPP
