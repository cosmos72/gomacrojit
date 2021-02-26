/*
 * onejit - JIT compiler in C++
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
 * idset.hpp
 *
 *  Created on Jan 09, 2021
 *      Author Massimiliano Ghilardi
 */

#ifndef ONEJIT_IDSET_HPP
#define ONEJIT_IDSET_HPP

#include <onejit/id.hpp>
#include <onestl/bitset.hpp>

namespace onejit {

// set of Id. only supports Id >= Id::FIRST
class IdSet : private BitSet {
  using Base = BitSet;

  enum : size_t { FIRST = Id::FIRST };

public:
  constexpr IdSet() noexcept : Base{} {
  }
  explicit IdSet(size_t size) noexcept : Base{size} {
  }

  IdSet(const IdSet &other) = delete;
  IdSet(IdSet &&other) noexcept = default;

  ~IdSet() noexcept = default;

  IdSet &operator=(const IdSet &other) = delete;
  IdSet &operator=(IdSet &&other) noexcept = default;

  using Base::capacity;
  using Base::clear;
  using Base::empty;
  using Base::operator bool;
  using Base::size;
  using Base::truncate;

  // checked element access:
  // return true if Id is present, otherwise false
  constexpr bool operator[](Id id) const noexcept {
    return Base::operator[](id.val() - FIRST);
  }

  // add or remove Id. does nothing if Id is out of bounds.
  void set(Id id, bool value) noexcept {
    Base::set(id.val() - FIRST, value);
  }

  // resize idset, changing its size.
  // return false if out of memory.
  bool resize(Id highest) noexcept {
    size_t val = highest.val() + 1;
    if (val < FIRST) {
      val = FIRST;
    }
    return Base::resize(val - FIRST);
  }

  // increase idset capacity.
  // return false if out of memory.
  bool reserve(Id highest) noexcept {
    size_t val = highest.val() + 1;
    if (val < FIRST) {
      val = FIRST;
    }
    return Base::reserve(val - FIRST);
  }

  void swap(IdSet &other) noexcept {
    Base::swap(other);
  }
}; // class IdSet

} // namespace onejit

#endif // ONEJIT_ID_HPP
