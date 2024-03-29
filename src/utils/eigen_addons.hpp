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

/*
 * Eigen extensions.  These are injected into the array and matrix base
 * by config.h.  Given m = [x, y, z]; m.xxz() = [x, x, z].
 */

#define PER2(a,b) Derived a##b() const \
                  { return Derived(this->a(), this->b()); }
PER2(x,x) PER2(x,y)
          PER2(y,y)
#undef PER2

ConstReverseReturnType yx() const { return reverse(); }

#define PER3(a,b,c) Derived a##b##c() const \
                    { return Derived(this->a(), this->b(), this->c()); }
PER3(x,x,x) PER3(x,x,y) PER3(x,x,z)
PER3(x,y,x) PER3(x,y,y) PER3(x,y,z)
PER3(x,z,x) PER3(x,z,y) PER3(x,z,z)
PER3(y,x,x) PER3(y,x,y) PER3(y,x,z)
PER3(y,y,x) PER3(y,y,y) PER3(y,y,z)
PER3(y,z,x) PER3(y,z,y) PER3(y,z,z)
PER3(z,x,x) PER3(z,x,y) PER3(z,x,z)
PER3(z,y,x) PER3(z,y,y) PER3(z,y,z)
PER3(z,z,x) PER3(z,z,y) PER3(z,z,z)
#undef PER3

/**
 * boost::serialization support; this is only intended for fixed-size
 *  Eigen types.
 */
template<typename ArchiveT>
void serialize(ArchiveT& ar, unsigned int /*version*/)
{
    for (int i = 0; i < this->rows(); ++i)
        for (int j = 0; j < this->cols(); ++j)
            ar & (*this)(i, j);
}
