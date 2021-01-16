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
 * varexpr.cpp
 *
 *  Created on Jan 13, 2020
 *      Author Massimiliano Ghilardi
 */

#include "onejit/varexpr.hpp"

namespace onejit {

Var VarExpr::var() const {
  if (is_direct()) {
    return Var{kind(), VarId{offset_or_direct()}};
  } else {
    return Var::from_indirect(at(0));
  }
}

VarExpr VarExpr::create(Var var, Code *holder) {
  CodeItem offset_or_direct = holder->offset();
  const bool is_direct = var.is_direct();

  if (is_direct || holder->add(var.indirect())) {
    // must match Var::operator Node()
    NodeHeader header = NodeHeader{VAR, var.kind(), 0};
    if (is_direct) {
      // must match Var::operator Node()
      offset_or_direct = var.id().val();
      holder = nullptr;
    }
    return VarExpr{header, offset_or_direct, holder};
  }
  return VarExpr{};
}

void VarExpr::add_to(Code *holder) const {
  holder->add(is_direct() //
                  ? Var{kind(), VarId{offset_or_direct()}}.direct()
                  : offset_or_direct());
}

std::ostream &operator<<(std::ostream &out, VarExpr v) {
  return out << v.var();
}

} // namespace onejit
