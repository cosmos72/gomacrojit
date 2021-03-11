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
 * allocator.hpp
 *
 *  Created on Feb 26, 2021
 *      Author Massimiliano Ghilardi
 */

#ifndef ONEJIT_REG_ALLOCATOR_HPP
#define ONEJIT_REG_ALLOCATOR_HPP

#include <onejit/fwd.hpp>
#include <onejit/reg/fwd.hpp>
#include <onestl/array.hpp>
#include <onestl/graph.hpp>

namespace onejit {
namespace reg {

// register allocator. uses register interference graph and Chaitin algorithm.
class Allocator {

public:
  Allocator() noexcept;
  explicit Allocator(Size num_regs) noexcept;

  Allocator(Allocator &&) = default;
  Allocator(const Allocator &) = delete;

  ~Allocator() noexcept;

  Allocator &operator=(Allocator &&) = default;
  Allocator &operator=(const Allocator &) = delete;

  // reset Allocator and reinitialize it
  // for a (possibly) different number of registers.
  // return false if out of memory.
  bool reset(Size num_regs) noexcept;

  Graph &graph() {
    return g_;
  }

  constexpr const Graph &graph() const {
    return g_;
  }

  // enable hints and store preferred Reg->Color into them.
  // Note: hints are disabled in newly-constructed instances,
  // and reset() disables them too.
  void add_hint(Reg reg, Color color) noexcept;

  // choose a color for each Reg present in graph()
  void allocate_regs(Color num_colors) noexcept;

  // return colors chosen by allocate_regs()
  // spilled Regs will have color >= num_colors
  constexpr View<Color> get_colors() const noexcept {
    return colors_;
  }

  // return a bitset with size() == num_regs.
  // can be resized and used as buffer by user code,
  // as long as its size is again num_regs when allocate_regs() is called.
  BitSet &get_bitset() noexcept {
    return avail_colors_;
  }

private:
  constexpr Size size() const noexcept {
    return g_.size();
  }

  // called by allocate_regs()
  void init() noexcept;

  // find a register in g_ with degree less than specified degree
  Reg find_degree_less_than(Degree degree) const noexcept;

  // pick a register in g_ to be spilled
  Reg pick() const noexcept;

  // pop registers from stack_ and color them
  // in the lowest color not used by some neighbor
  void assign_colors(Color num_colors) noexcept;

  // try to find an alternate color for Reg that satisfies hints
  Color try_satisfy_hints(Reg reg) noexcept;

  Graph g_;  // index is reg
  Graph g2_; // index is reg
  Array<Reg> stack_;
  Array<Color> hints_;  // index is reg
  Array<Color> colors_; // index is reg
  BitSet avail_colors_;

}; // class Allocator

} // namespace reg
} // namespace onejit

#endif // ONEJIT_REG_ALLOCATOR_HPP