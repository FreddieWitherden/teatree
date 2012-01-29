/*
    This file is part of teatree.
    Copyright (C) 2011–2012  Freddie Witherden <freddie@witherden.org>

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

#ifndef TEATREE_AVAILABLE_SIMULATIONS_HPP
#define TEATREE_AVAILABLE_SIMULATIONS_HPP

#include "config.h"

#include "efield/plummer.hpp"
#include "mac/opening_angle.hpp"
#include "mac/min_dist.hpp"
#include "pusher/comp/21.hpp"
#include "simulation/open.hpp"

#include "boost/mpl/vector.hpp"

namespace teatree
{

/**
 * Simulation types available in this build of teatree.
 */
typedef boost::mpl::vector
<
  /*
   * Simple 2D simulation with open boundaries and multipole moments
   */
  simulation_open< double             // Double precision
                 , 2                  // 2D
                 , 2                  // Quadrupole moments
                 , mac_opening_angle  // BH mac
                 , efield_plummer     // Plummer potential
                 , pusher_comp_21     // Verlet method
                 >,
  simulation_open< double             // Double precision
                 , 3                  // 3D
                 , 0                  // Quadrupole moments
                 , mac_opening_angle  // BH mac
                 , efield_plummer     // Plummer potential
                 , pusher_comp_21     // Verlet method
                 >,
  simulation_open< double             // Double precision
                 , 3                  // 3D
                 , 1                  // Octupole moments
                 , mac_opening_angle  // BH mac
                 , efield_plummer     // Plummer potential
                 , pusher_comp_21     // Verlet method
                 >,
  simulation_open< double             // Double precision
                 , 3                  // 3D
                 , 2                  // Octupole moments
                 , mac_opening_angle  // BH mac
                 , efield_plummer     // Plummer potential
                 , pusher_comp_21     // Verlet method
                 >
> available_simulations;

}


#endif // TEATREE_AVAILABLE_SIMULATIONS_HPP
