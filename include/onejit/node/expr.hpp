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
 * expr.hpp
 *
 *  Created on Jan 16, 2021
 *      Author Massimiliano Ghilardi
 */

#ifndef ONEJIT_NODE_EXPR_HPP
#define ONEJIT_NODE_EXPR_HPP

#include <onejit/node/node.hpp>

namespace onejit {

// base class of Binary, Const, Label, Mem, Tuple, Unary, Var
class Expr : public Node {
  using Base = Node;

  friend class Node;

public:
  /**
   * construct an invalid Expr.
   * exists only to allow placing Expr in containers
   * and similar uses that require a default constructor.
   *
   * to construct a valid Expr, use one of the subclasses constructors
   */
  constexpr Expr() noexcept : Base{} {
  }

protected:
  // downcast Node to Expr
  constexpr explicit Expr(const Node &node) noexcept : Base{node} {
  }

  // downcast helper
  static constexpr bool is_allowed_type(Type t) noexcept {
    return t >= VAR && t <= CONST;
  }
};

} // namespace onejit

#endif // ONEJIT_NODE_EXPR_HPP