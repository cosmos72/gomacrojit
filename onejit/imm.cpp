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
 * imm.cpp
 *
 *  Created on Jan 13, 2021
 *      Author Massimiliano Ghilardi
 */

#include <onejit/code.hpp>
#include <onejit/imm.hpp>
#include <onestl/chars.hpp>

namespace onejit {

Imm Imm::parse_indirect(Kind kind, Offset offset, const Code *holder) noexcept {
  uint64_t bits;
  if (kind.bits().val() == 64) {
    bits = holder->uint64(offset);
  } else {
    bits = holder->uint32(offset);
  }
  return Imm{kind, bits};
}

Code &Imm::write_indirect(Code *holder) const noexcept {
  if (kind().bits().val() == 64) {
    return holder->add_uint64(uint64());
  } else {
    return holder->add_uint32(uint32());
  }
}

} // namespace onejit
