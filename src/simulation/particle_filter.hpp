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

#ifndef TEATREE_SIMULATION_PARTICLE_FILTER
#define TEATREE_SIMULATION_PARTICLE_FILTER

#include <boost/assert.hpp>
#include <boost/iostreams/filter/line.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/xpressive/xpressive.hpp>

#include <stdexcept>

namespace teatree
{

class particle_filter : public boost::iostreams::line_filter
{
public: // Constructors
    particle_filter(int dim)
        : particle_re_(boost::xpressive::sregex::compile(
            "([-+]?[0-9]*"        // Sign, integral part
            "\\.?[0-9]+"          // Point, decimal part
            "([eE][-+]?[0-9]+)?"  // Exponent
            "(\\s+|$))"           // Whitespace (or EOL)
            "{"                   // Repeat 2d+2 times
              + boost::lexical_cast<std::string>(2*dim + 2) +
            "}"))
        , line_count_(0)
        , comment_count_(0)
        , particle_count_(0)
    {
        BOOST_ASSERT(dim == 2 || dim == 3);
    }

public: // Accessors
    int line_count()     const { return line_count_; }
    int particle_count() const { return particle_count_; }

private: // Virtual methods
    std::string do_filter(const std::string& line);

private: // Members
    const boost::xpressive::sregex particle_re_;

    int line_count_;
    int comment_count_;
    int particle_count_;
};

std::string particle_filter::do_filter(const std::string& line)
{
    // Increment the line count
    ++line_count_;

    // If the line is a comment line then skip it
    if (!line.empty() && line[0] == '#')
    {
        ++comment_count_;
        return "";
    }

    // If the line matches the form of a particle then return it
    if (boost::xpressive::regex_match(line, particle_re_))
    {
        ++particle_count_;
        return line;
    }
    // If this is the first non-comment line then treat it as a header and skip
    else if (line_count_-comment_count_ == 1)
        return "";
    // Otherwise, throw
    else
        throw std::invalid_argument("Bad particle on line: "
                                  + boost::lexical_cast<std::string>(line_count_));
}

}

#endif // TEATREE_SIMULATION_PARTICLE_FILTER
