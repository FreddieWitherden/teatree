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

#ifndef TEATREE_UTILS_ALIGNED_POOL_ALLOCATOR_HPP
#define TEATREE_UTILS_ALIGNED_POOL_ALLOCATOR_HPP

#include <cstddef>
#include <Eigen/Core>

namespace teatree {

/**
 * An aligned allocator for use with boost::pool which ensures that all
 * memory allocated is aligned on 128-bit boundaries.  This is required
 * whenever a pool is being used to allocate an object containing an
 * Eigen structure.
 */
struct aligned_pool_allocator
{
    typedef std::size_t size_type;
    typedef std::ptrdiff_t difference_type;

    static char* malloc(size_type nbytes)
    { return static_cast<char*>(Eigen::internal::aligned_malloc(nbytes)); }

    static void free(char* ptr)
    { Eigen::internal::aligned_free(ptr); }
};

}

#endif // TEATREE_UTILS_ALIGNED_POOL_ALLOCATOR_HPP
