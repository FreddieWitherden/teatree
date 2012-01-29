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

#ifndef TEATREE_UTILS_SIGNALS_MAX_COMBINER_HPP
#define TEATREE_UTILS_SIGNALS_MAX_COMBINER_HPP

namespace teatree
{

template<typename T>
struct signals_maximum_combiner
{
    typedef T result_type;

    template<typename InputIteratorT>
    T operator()(InputIteratorT first, InputIteratorT last) const
    {
        if (first == last) return T();

        T max = *first;
        while (++first != last)
          if (max < *first) max = *first;

        return max;
    }
};

}

#endif // TEATREE_UTILS_SIGNALS_MAX_COMBINER_HPP
