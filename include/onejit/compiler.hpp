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
 * compiler.hpp
 *
 *  Created on Jan 22, 2021
 *      Author Massimiliano Ghilardi
 */

#ifndef ONEJIT_COMPILER_HPP
#define ONEJIT_COMPILER_HPP

#include <onejit/error.hpp>
#include <onejit/node/label.hpp>
#include <onejit/node/node.hpp>
#include <onejit/optimizer.hpp>
#include <onestl/vector.hpp>

namespace onejit {

////////////////////////////////////////////////////////////////////////////////

class Compiler {
public:
  enum Flag : uint8_t;

  Compiler() noexcept;
  Compiler(Compiler &&other) noexcept = default;
  Compiler &operator=(Compiler &&other) noexcept = default;

  ~Compiler() noexcept;

  explicit operator bool() const noexcept;

  constexpr Func *func() const noexcept {
    return func_;
  }

  // compile function
  Compiler &compile(Func &func, Optimizer::Flag flags = Optimizer::All) noexcept;

private:
  Node compile(Assign stmt, Flag flags) noexcept;
  Node compile(AssignCall stmt, Flag flags) noexcept;
  Expr compile(Binary expr, Flag flags) noexcept;
  Node compile(Block stmt, Flag flags) noexcept;
  Expr compile(Call expr, Flag flags) noexcept;
  Node compile(Cond stmt, Flag flags) noexcept;
  Expr compile(Expr expr, Flag flags) noexcept;
  Node compile(For stmt, Flag flags) noexcept;
  Node compile(If stmt, Flag flags) noexcept;
  Node compile(JumpIf stmt, Flag flags) noexcept;
  Expr compile(Mem expr, Flag flags) noexcept;
  Node compile(Node node, Flag flags) noexcept;
  Node compile(Return stmt, Flag flags) noexcept;
  Node compile(Stmt0 stmt, Flag flags) noexcept;
  Node compile(Stmt1 stmt, Flag flags) noexcept;
  Node compile(Stmt2 stmt, Flag flags) noexcept;
  Node compile(Stmt3 stmt, Flag flags) noexcept;
  Node compile(Stmt4 stmt, Flag flags) noexcept;
  Node compile(StmtN stmt, Flag flags) noexcept;
  Node compile(Switch stmt, Flag flags) noexcept;
  Expr compile(Unary expr, Flag flags) noexcept;
  Expr compile(Tuple expr, Flag flags) noexcept;

  // simplify x && y
  Expr simplify_land(Expr x, Expr y) noexcept;
  // simplify x || y
  Expr simplify_lor(Expr x, Expr y) noexcept;

private:
  // get current destination for "break"
  Label label_break() const noexcept;

  // get current destination for "continue"
  Label label_continue() const noexcept;

  // get current destination for "fallthrough"
  Label label_fallthrough() const noexcept;

  Compiler &enter_case(Label l_break, Label l_fallthrough) noexcept;
  Compiler &exit_case() noexcept;

  Compiler &enter_loop(Label l_break, Label l_continue) noexcept;
  Compiler &exit_loop() noexcept;

  // copy expression result to a new local variable.
  // if node is already a Var, does nothing and returns it
  Var to_var(const Node &node) noexcept;

  // copy node.child(start ... end-1) to new local variables,
  // and append such variables to vars.
  Compiler &to_vars(const Node &node, uint32_t start, uint32_t end, //
                    Vector<Expr> &vars) noexcept;

  // add an already compiled node to compiled list
  Compiler &add(const Node &node) noexcept;

  // compile a node, then add it to compiled list
  Compiler &compile_add(const Node &node, Flag flags) noexcept {
    return add(compile(node, flags));
  }

  // store compiled code into function.compiled()
  // invoked by compile(Func)
  Compiler &finish() noexcept;

  // add a compile error
  Compiler &error(const Node &where, Chars msg) noexcept;

  // add an out-of-memory error
  Compiler &out_of_memory(const Node &where) noexcept;

private:
  Optimizer optimizer_;
  Func *func_;

  Vector<Label> break_;       // stack of 'break' destination labels
  Vector<Label> continue_;    // stack of 'continue' destination labels
  Vector<Label> fallthrough_; // stack of 'fallthrough' destination labels
  Vector<Node> node_;
  Vector<Error> error_;
  bool good_; // good_ = false means out-of-memory
};

} // namespace onejit

#endif // ONEJIT_COMPILER_HPP
