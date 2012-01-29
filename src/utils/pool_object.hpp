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

#ifndef TEATREE_UTILS_POOL_OBJECT_HPP
#define TEATREE_UTILS_POOL_OBJECT_HPP

#include "utils/aligned_pool_allocator.hpp"

#include <boost/pool/pool.hpp>

namespace teatree {

/**
 * Base class overloading the new and delete operators so that they allocate
 *  from a boost::pool object.
 */
template<typename T,
         typename AllocT = aligned_pool_allocator>
class pool_object
{
public:
    static void* operator new(size_t /*size*/)
    { return pool_.malloc(); }

    static void operator delete(void *p)
    { pool_.free(p); }

private:
    static boost::pool<AllocT> pool_;
};

template<typename T, typename AllocT>
boost::pool<AllocT> pool_object<T,AllocT>::pool_(sizeof(T));

}

#endif // TEATREE_UTILS_POOL_OBJECT_HPP
