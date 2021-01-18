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
 * unaryexpr.cpp
 *
 *  Created on Jan 16, 2020
 *      Author Massimiliano Ghilardi
 */

#include "onejit/unaryexpr.hpp"
#include "onejit/code.hpp"

namespace onejit {

UnaryExpr UnaryExpr::create(Kind kind, Op1 op, const Expr &child, Code *holder) {
  const NodeHeader header{UNARY, kind, uint16_t(op)};
  CodeItem offset = holder->length();

  if (!holder->add(header) || !holder->add(child, offset)) {
    holder->truncate(offset);
    return UnaryExpr{};
  }
  return UnaryExpr{header, offset, holder};
}

std::ostream &operator<<(std::ostream &out, const UnaryExpr &ue) {
  return out << '(' << ue.op() << ' ' << ue.child(0) << ')';
}

} // namespace onejit