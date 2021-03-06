/*
 * onejit - JIT compiler in C++
 *
 * Copyright (C) 2018-2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * header.hpp
 *
 *  Created on Jan 12, 2021
 *      Author Massimiliano Ghilardi
 */

#ifndef ONEJIT_IR_HEADER_HPP
#define ONEJIT_IR_HEADER_HPP

#include <onejit/endian.hpp>
#include <onejit/fwd.hpp>
#include <onejit/kind.hpp>
#include <onejit/opstmt.hpp>
#include <onejit/type.hpp>

namespace onejit {
namespace ir {

////////////////////////////////////////////////////////////////////////////////
// first CodeItem contained in Binary, Unary, Stmt*
class Header {

public:
  /**
   * construct an invalid Header.
   * exists only to allow placing Header in containers
   * and similar uses that require a default constructor.
   *
   * to create a valid Header, use one of the other constructors.
   */
  constexpr Header() noexcept //
      : Header{STMT_0, Bad, BAD} {
  }

  constexpr explicit Header(CodeItem item) noexcept //
      : val_{((item >> 4) & 0x0F) | (item & ~0xFF)} {
  }

  constexpr Header(Type type, Kind kind, uint16_t op) noexcept
      : val_{(type & 0x0F) | uint32_t(kind.val()) << 8 | uint32_t(op) << 16} {
  }

  constexpr Type type() const noexcept {
    return Type(val_ & 0x0F);
  }

  constexpr Kind kind() const noexcept {
    return Kind{uint8_t(val_ >> 8)};
  }

  constexpr uint16_t op() const noexcept {
    return uint16_t(val_ >> 16);
  }

  constexpr CodeItem item() const noexcept {
    return 0xE | (val_ & 0x0F) << 4 | (val_ & ~0xFF);
  }

  constexpr explicit operator bool() const noexcept {
    return kind() != Bad;
  }

  constexpr bool operator!() const noexcept {
    return kind() == Bad;
  }

  // true if this and other Header have the same type, kind and op.
  constexpr bool operator==(const Header &other) const noexcept {
    return val_ == other.val_;
  }

  constexpr bool operator!=(const Header &other) const noexcept {
    return val_ != other.val_;
  }

  constexpr bool operator<(const Header &other) const noexcept {
    return order() < other.order();
  }

private:
  constexpr uint32_t order() const noexcept {
    return uint32_t(type()) << 24 | uint32_t(kind().val()) << 16 | op();
  }

  union {
    CodeItem val_;
#ifdef ONEJIT_LITTLE_ENDIAN
    // only for debug purposes. works only on little-endian machines
    struct {
      Type type_;
      eKind ekind_;
      uint16_t op_;
    };
#endif
  };
};

} // namespace ir
} // namespace onejit

#endif // ONEJIT_IR_HEADER_HPP
