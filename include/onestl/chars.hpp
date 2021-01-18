/*
 * onestl - Tiny STL C++ library
 *
 * Copyright (C) 2018-2021 Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * chars.hpp
 *
 *  Created on Jan 09, 2020
 *      Author Massimiliano Ghilardi
 */
#ifndef ONESTL_CHARS_HPP
#define ONESTL_CHARS_HPP

#include <onestl/view.hpp>

#include <ostream>

namespace onestl {

/** read-only view of char[] */
class Chars : public View<char> {
private:
  typedef char T;
  typedef View<char> Base;

public:
  constexpr Chars() : Base() {
  }
  template <size_t N> constexpr Chars(const T (&addr)[N]) : Base(addr, N - 1) {
  }
  constexpr Chars(const T *addr, size_t n) : Base(addr, n) {
  }
  constexpr Chars(const Base &other) : Base(other) {
  }

  Chars view(size_t start, size_t end) const {
    return Chars(Base::view(start, end));
  }
};

inline std::ostream &operator<<(std::ostream &out, Chars chars) {
  return out.write(chars.data(), chars.size());
}

} // namespace onestl

#endif /* ONESTL_CHARS_HPP */