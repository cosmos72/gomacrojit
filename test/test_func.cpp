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
 * test_func.cpp
 *
 *  Created on Jan 08, 2021
 *      Author Massimiliano Ghilardi
 */

#include "test.hpp"

#include <onejit/func.hpp>
#include <onejit/node.hpp>

namespace onejit {

void Test::func_fib() {
  Kind kind = Uint64;
  Func &f = func.reset(&holder, Name{&holder, "fib"}, FuncType{&holder, {kind}, {kind}});
  Var n = f.param(0);
  Const one = One(kind);
  Const two = Two(kind);

  /**
   * jit equivalent of C/C++ source code
   *
   * uint64_t fib(uint64_n) {
   *   if (n > 2) {
   *     return f(n-1) + f(n-2);
   *   } else {
   *     return 1;
   *   }
   * }
   */

  f.set_body(                                                  //
      If{f, Binary{f, GTR, n, two},                            //
         Return{f,                                             //
                Binary{f,                                      //
                       ADD,                                    //
                       Call{f, f, {Binary{f, SUB, n, one}}},   //
                       Call{f, f, {Binary{f, SUB, n, two}}}}}, //
         Return{f, one}});                                     //

  Chars expected = "(if (> var1000_ul 2)\n\
    (return (+ (call label_0 (- var1000_ul 1)) (call label_0 (- var1000_ul 2))))\n\
    (return 1))";
  TEST(to_string(f.get_body()), ==, expected);

  compile(f);

  expected = "(block\n\
    (asm_cmp var1000_ul 2)\n\
    (asm_jbe label_1)\n\
    (= var1002_ul (- var1000_ul 1))\n\
    (= var1003_ul (call label_0 var1002_ul))\n\
    (= var1004_ul (- var1000_ul 2))\n\
    (= var1005_ul (call label_0 var1004_ul))\n\
    (= var1001_ul (+ var1003_ul var1005_ul))\n\
    (return var1001_ul)\n\
    (goto label_2)\n\
    label_1\n\
    (= var1001_ul 1)\n\
    (return var1001_ul)\n\
    label_2)";
  TEST(to_string(f.get_compiled()), ==, expected);

  // dump_and_clear_code();
  holder.clear();
}

void Test::func_loop() {
  Kind kind = Uint64;
  Func &f = func.reset(&holder, Name{&holder, "loop"}, FuncType{&holder, {kind}, {kind}});
  Var n = f.param(0);
  Var total = f.result(0);
  Var i{f, kind};
  Const zero = Zero(kind);

  /**
   * jit equivalent of C/C++ source code
   *
   * uint64_t loop(uint64_n) {
   *   uint64_t total = 0, i;
   *   for (i = 0; i < n; i++) {
   *     total += i;
   *   }
   *   return total;
   * }
   */

  f.set_body( //
      Block{f,
            {Assign{f, ASSIGN, total, zero},
             For{
                 f,                              //
                 Assign{f, ASSIGN, i, zero},     // init
                 Binary{f, LSS, i, n},           // test
                 Inc{f, i},                      // post
                 Assign{f, ADD_ASSIGN, total, i} // body
             },
             Return{f, total}}});

  Chars expected = "(block\n\
    (= var1001_ul 0)\n\
    (for (= var1002_ul 0) (< var1002_ul var1000_ul) (++ var1002_ul)\n\
        (+= var1001_ul var1002_ul))\n\
    (return var1001_ul))";
  TEST(to_string(f.get_body()), ==, expected);

  compile(f);

  expected = "(block\n\
    (= var1001_ul 0)\n\
    (= var1002_ul 0)\n\
    (goto label_2)\n\
    label_1\n\
    (+= var1001_ul var1002_ul)\n\
    (++ var1002_ul)\n\
    label_2\n\
    (asm_cmp var1002_ul var1000_ul)\n\
    (asm_jb label_1)\n\
    label_3\n\
    (return var1001_ul))";
  TEST(to_string(f.get_compiled()), ==, expected);

  // dump_and_clear_code();
  holder.clear();
}

void Test::func_switch1() {
  Kind kind = Uint64;
  Func &f = func.reset(&holder, Name{&holder, "fswitch1"}, FuncType{&holder, {kind}, {kind}});
  Var n = f.param(0);
  Var ret = f.result(0);
  Const zero = Zero(kind);
  Const one = One(kind);
  Const two = Two(kind);

  /**
   * jit equivalent of C/C++ source code
   *
   * uint64_t fswitch1(uint64_n) {
   *   uint64_t ret;
   *   switch (n) {
   *     case 0:
   *       ret = 1;
   *       break;
   *     case 1:
   *       ret = 2;
   *       break;
   *     default:
   *       ret = n + 1;
   *       break;
   *   }
   *   return ret;
   * }
   */

  f.set_body( //
      Block{f,
            {Switch{f,
                    n,
                    {Case{f, zero, Assign{f, ASSIGN, ret, one}}, //
                     Case{f, one, Assign{f, ASSIGN, ret, two}},  //
                     Default{f, Assign{f, ASSIGN, ret, Binary{f, ADD, n, one}}}}},
             Return{f, ret}}});

  Chars expected = "(block\n\
    (switch\n\
        var1000_ul\n\
        (case 0 (= var1001_ul 1))\n\
        (case 1 (= var1001_ul 2))\n\
        (default (= var1001_ul (+ var1000_ul 1))))\n\
    (return var1001_ul))";
  TEST(to_string(f.get_body()), ==, expected);

  compile(f);

  expected = "(block\n\
    (asm_cmp var1000_ul 0)\n\
    (asm_jne label_2)\n\
    (= var1001_ul 1)\n\
    (goto label_1)\n\
    label_2\n\
    (asm_cmp var1000_ul 1)\n\
    (asm_jne label_4)\n\
    label_3\n\
    (= var1001_ul 2)\n\
    (goto label_1)\n\
    label_4\n\
    label_5\n\
    (= var1001_ul (+ var1000_ul 1))\n\
    label_1\n\
    (return var1001_ul))";
  TEST(to_string(f.get_compiled()), ==, expected);

  // dump_and_clear_code();
  holder.clear();
}

void Test::func_switch2() {
  Kind kind = Uint64;
  Func &f = func.reset(&holder, Name{&holder, "fswitch2"}, FuncType{&holder, {kind}, {kind}});
  Var n = f.param(0);
  Var ret = f.result(0);
  Const zero = Zero(kind);
  Const one = One(kind);
  Const two = Two(kind);

  /**
   * jit equivalent of C/C++ source code
   *
   * uint64_t fswitch2(uint64_n) {
   *   uint64_t ret;
   *   switch (n) {
   *     case 0:
   *       ret = 1;
   *       break;
   *     default:
   *       ret = n + 1;
   *       break;
   *     case 1:
   *       ret = 2;
   *       break;
   *   }
   *   return ret;
   * }
   */

  f.set_body( //
      Block{f,
            {Switch{f,
                    n,
                    {Case{f, zero, Assign{f, ASSIGN, ret, one}},                 //
                     Default{f, Assign{f, ASSIGN, ret, Binary{f, ADD, n, one}}}, //
                     Case{f, one, Assign{f, ASSIGN, ret, two}}}},                //
             Return{f, ret}}});

  Chars expected = "(block\n\
    (switch\n\
        var1000_ul\n\
        (case 0 (= var1001_ul 1))\n\
        (default (= var1001_ul (+ var1000_ul 1)))\n\
        (case 1 (= var1001_ul 2)))\n\
    (return var1001_ul))";
  TEST(to_string(f.get_body()), ==, expected);

  compile(f);

  expected = "(block\n\
    (asm_cmp var1000_ul 0)\n\
    (asm_jne label_2)\n\
    (= var1001_ul 1)\n\
    (goto label_1)\n\
    label_2\n\
    (goto label_4)\n\
    label_3\n\
    (= var1001_ul (+ var1000_ul 1))\n\
    (goto label_1)\n\
    label_4\n\
    (asm_cmp var1000_ul 1)\n\
    (asm_jne label_3)\n\
    label_5\n\
    (= var1001_ul 2)\n\
    label_1\n\
    (return var1001_ul))";
  TEST(to_string(f.get_compiled()), ==, expected);

  // dump_and_clear_code();
  holder.clear();
}

void Test::func_cond() {
  Kind kind = Uint64;
  Func &f = func.reset(&holder, Name{&holder, "fcond"}, FuncType{&holder, {kind}, {kind}});
  Var n = f.param(0);
  Var ret = f.result(0);
  Const zero = Zero(kind);
  Const one = One(kind);
  Const two = Two(kind);

  /**
   * jit equivalent of C/C++ source code
   *
   * uint64_t fswitch(uint64_n) {
   *   uint64_t ret;
   *   if (n == 0) {
   *     ret = 1;
   *   } else if (n == 1) {
   *     ret = 2;
   *   } else if (true) {
   *     ret = n + 1;
   *   }
   *   return ret;
   * }
   */

  f.set_body( //
      Block{f,
            {Cond{f,
                  {Binary{f, EQL, n, zero}, Assign{f, ASSIGN, ret, one},       //
                   Binary{f, EQL, n, one}, Assign{f, ASSIGN, ret, two},        //
                   TrueExpr, Assign{f, ASSIGN, ret, Binary{f, ADD, n, one}}}}, //
             Return{f, ret}}});

  Chars expected = "(block\n\
    (cond\n\
        (== var1000_ul 0)\n\
        (= var1001_ul 1)\n\
        (== var1000_ul 1)\n\
        (= var1001_ul 2)\n\
        true\n\
        (= var1001_ul (+ var1000_ul 1)))\n\
    (return var1001_ul))";
  TEST(to_string(f.get_body()), ==, expected);

  compile(f);

  expected = "(block\n\
    (asm_cmp var1000_ul 0)\n\
    (asm_jne label_2)\n\
    (= var1001_ul 1)\n\
    (goto label_1)\n\
    label_2\n\
    (asm_cmp var1000_ul 1)\n\
    (asm_jne label_3)\n\
    (= var1001_ul 2)\n\
    (goto label_1)\n\
    label_3\n\
    (asm_cmp true false)\n\
    (asm_je label_1)\n\
    (= var1001_ul (+ var1000_ul 1))\n\
    label_1\n\
    (return var1001_ul))";
  TEST(to_string(f.get_compiled()), ==, expected);

  // dump_and_clear_code();
  holder.clear();
}

} // namespace onejit