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
 * op0.hpp
 *
 *  Created on Jan 09, 2020
 *      Author Massimiliano Ghilardi
 */

#include "onejit/op0.hpp"

#include <ostream>

namespace onejit {

static const Chars op0string[] = {
    "?",
    "break",
    "continue",
    "fallthrough",
};

const Chars &to_string(Op0 op) {
  uint8_t i = 0;
  switch (op) {
  case BREAK:
    i = 1;
    break;
  case CONTINUE:
    i = 2;
    break;
  case FALLTHROUGH:
    i = 3;
    break;
  }
  return op0string[i];
}

std::ostream &operator<<(std::ostream &out, Op0 op) {
  return out << to_string(op);
}

} // namespace onejit