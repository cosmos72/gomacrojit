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
 * node.cpp
 *
 *  Created on Jan 09, 2020
 *      Author Massimiliano Ghilardi
 */

#include "onejit/node.hpp"
#include "onejit/check.hpp"
#include "onejit/code.hpp"
#include "onejit/const.hpp"
#include "onejit/var.hpp"

namespace onejit {

Node Node::child(uint16_t i) const {
  check(i, <, children());
  const CodeItem item = code_->uint32(size_t(i) * sizeof(Offset) + data_ + sizeof(NodeHeader));

  NodeHeader header;
  uint32_t offset = 0;

  if (item <= FALLTHROUGH) {
    header = NodeHeader(Type(item), Void, 0);
  } else if ((item & 1) == 1) {
    // direct Const
    return Node(Const::from_direct(item));
  } else if ((item & 3) != 0) {
    // direct Var
    return Node(Var::from_direct(item));
  } else {
    // indirect Node
    offset = data_ + item;
    header = NodeHeader(code_->uint32(offset));
  }
  return Node{header, offset, code_};
}

} // namespace onejit
