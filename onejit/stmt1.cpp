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
 * stmt1.cpp
 *
 *  Created on Jan 18, 2020
 *      Author Massimiliano Ghilardi
 */

#include "onejit/stmt1.hpp"
#include "onejit/code.hpp"

namespace onejit {

// ============================  Stmt1  ========================================

Stmt1 Stmt1::create(OpStmt1 op, const Node &child, Code *holder) {
  const NodeHeader header{STMT_1, Void, uint16_t(op)};
  CodeItem offset = holder->length();

  if (!holder->add(header) || !holder->add(child, offset)) {
    holder->truncate(offset);
    return Stmt1{};
  }
  return Stmt1{header, offset, holder};
}

std::ostream &operator<<(std::ostream &out, const Stmt1 &st1) {
  return out << '(' << st1.op() << ' ' << st1.child(0) << ')';
}

// ============================  Default  ======================================

Default Default::create(const Node &body, Code *holder) {
  return Stmt1::create(DEFAULT, body, holder).is<Default>();
}

} // namespace onejit
