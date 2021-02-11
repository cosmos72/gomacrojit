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
 * test.hpp
 *
 *  Created on Jan 08, 2021
 *      Author Massimiliano Ghilardi
 */

#include <onejit/compiler.hpp>
#include <onejit/func.hpp>
#include <onejit/optimizer.hpp>

#include "test_disasm.hpp"

namespace onejit {

class Test : public TestDisasm {
public:
  Test();
  ~Test();

  void run();

private:
  FuncType ftype();
  void dump_and_clear_code();

  // called by run()
  void kind();
  void const_expr() const;
  void simple_expr();
  void nested_expr();
  void x64_expr();
  void eval_expr();
  void eval_expr_kind(Kind kind);
  void func_fib();
  void func_loop();
  void func_switch1();
  void func_switch2();
  void func_cond();
  void optimize_expr();
  void optimize_expr_kind(Kind kind);

  void compile(Func &func);

  Code holder;
  Func func;
  Compiler comp;
  Optimizer opt;
};

} // namespace onejit
