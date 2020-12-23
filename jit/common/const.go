/*
 * gomacrojit - JIT compiler in Go
 *
 * Copyright (C) 2019-2020 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * const.go
 *
 *  Created on Jan 23, 2019
 *      Author Massimiliano Ghilardi
 */

package common

type Const struct {
	kind Kind
	re   int64
	imag int64
}

var (
	False = NewConst(Bool, 0)
	True  = NewConst(Bool, 1)
)

// implement Expr interface
func (c Const) expr() {}

func (c Const) RegId() RegId {
	return NoRegId
}

func (c Const) Kind() Kind {
	return c.kind
}

func (c Const) IsConst() bool {
	return true
}

func (c Const) Size() Size {
	return c.kind.Size()
}

func (c Const) Val() int64 {
	return c.re
}

func (c Const) Int() int64 {
	return c.re
}

func (c Const) Uint() uint64 {
	return uint64(c.re)
}

// convert Const to a different kind
func (c Const) Cast(to Kind) Const {
	val := c.re
	// sign-extend or zero-extend to 64 bits
	switch c.kind {
	case Bool:
		if val != 0 {
			// non-zero means true => convert to 1
			val = 1
		}
	case Int:
		val = int64(int(val))
	case Int8:
		val = int64(int8(val))
	case Int16:
		val = int64(int16(val))
	case Int32:
		val = int64(int32(val))
	case Int64:
		// nothing to do
	case Uint:
		val = int64(uint(val))
	case Uint8:
		val = int64(uint8(val))
	case Uint16:
		val = int64(uint16(val))
	case Uint32:
		val = int64(uint32(val))
	case Uint64:
		val = int64(uint64(val)) // should be a nop
	case Uintptr:
		val = int64(uintptr(val))
	default:
		Errorf("Const.Cast: unsupported constant kind: %v", c.kind)
	}
	// let caller truncate val as needed
	return Const{kind: to, re: val}
}

func NewConst(kind Kind, val int64) Const {
	return Const{kind: kind, re: val}
}

func ConstInt(val int) Const {
	return Const{kind: Int, re: int64(val)}
}

func ConstInt8(val int8) Const {
	return Const{kind: Int8, re: int64(val)}
}

func ConstInt16(val int16) Const {
	return Const{kind: Int16, re: int64(val)}
}

func ConstInt32(val int32) Const {
	return Const{kind: Int32, re: int64(val)}
}

func ConstInt64(val int64) Const {
	return Const{kind: Int64, re: val}
}

func ConstUint(val uint) Const {
	return Const{kind: Uint, re: int64(val)}
}

func ConstUint8(val uint8) Const {
	return Const{kind: Uint8, re: int64(val)}
}

func ConstUint16(val uint16) Const {
	return Const{kind: Uint16, re: int64(val)}
}

func ConstUint32(val uint32) Const {
	return Const{kind: Uint32, re: int64(val)}
}

func ConstUint64(val uint64) Const {
	return Const{kind: Uint64, re: int64(val)}
}

func ConstUintptr(val uintptr) Const {
	return Const{kind: Uintptr, re: int64(val)}
}
