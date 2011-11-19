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

namespace teatree
{

/**
 * Provides storage for runtime simulation options.
 */
template<typename ScalarT>
class simulation_options
{
public: // Accessors
    ScalarT epsilon() const { return epsilon_; }
    ScalarT theta()   const { return theta_; }

public: // Settors
    simulation_options& epsilon(ScalarT s) { epsilon_ = s; return *this; }
    simulation_options& theta(ScalarT s)   { theta_ = s; return *this; }

private: // Members
    ScalarT epsilon_;
    ScalarT theta_;
};

}

#endif // TEATREE_SIMULATION_OPTIONS_HPP
