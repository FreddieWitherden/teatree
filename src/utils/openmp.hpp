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

#ifndef TEATREE_UTILS_OPENMP_HPP
#define TEATREE_UTILS_OPENMP_HPP

#include "config.h"

#ifdef TEATREE_ENABLE_OPENMP
# include <omp.h>
#else

// Stub out some of the OpenMP runtime
typedef enum omp_sched_t
{
    omp_sched_static  = 1,
    omp_sched_dynamic = 2,
    omp_sched_guided  = 3,
    omp_sched_auto    = 4
} omp_sched_t;

static inline void omp_set_num_threads(int) {}

static inline int omp_get_num_threads() { return 1; }
static inline int omp_get_max_threads() { return 1; }
static inline int omp_get_thread_num()  { return 1; }
static inline int omp_get_num_procs()   { return 1; }

static inline int omp_in_parallel()     { return 0; }

#endif

#endif // TEATREE_UTILS_OPENMP_HPP
