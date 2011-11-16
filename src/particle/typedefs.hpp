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

#ifndef TEATREE_PARTICLE_TYPEDEFS_HPP
#define TEATREE_PARTICLE_TYPEDEFS_HPP

namespace teatree
{

/**
 *
 */
#define TEATREE_PARTICLE_GENERATE_TYPEDEFS(ParticleT)         \
    typedef ParticleT particle_type;                          \
    typedef typename particle_type::scalar_type scalar_type;  \
    typedef typename particle_type::vector_type vector_type;  \
    typedef typename particle_type::array_type  array_type

/**
 *
 */
#define TEATREE_PSEUDO_PARTICLE_GENERATE_TYPEDEFS(PParticleT)                 \
    typedef PParticleT pseudo_particle_type;                                  \
    typedef typename PParticleT::particle_moments_type particle_moments_type; \
    TEATREE_PARTICLE_GENERATE_TYPEDEFS(typename PParticleT::particle_type)

}

#endif // TEATREE_PARTICLE_TYPEDEFS_HPP
