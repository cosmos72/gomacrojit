/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * kind.go
 *
 *  Created on: Mar 31, 2021
 *      Author: Massimiliano Ghilardi
 */

package types

type Kind uint8

const (
	Invalid Kind = iota

	Bool
	Int
	Int8
	Int16
	Int32
	Int64
	Uint
	Uint8
	Uint16
	Uint32
	Uint64
	Uintptr
	Float32
	Float64
	Complex64
	Complex128
	Array
	Chan
	Func
	Interface
	Map
	Ptr
	Slice
	String
	Struct
	UnsafePointer

	// types for untyped values
	UntypedBool
	UntypedInt
	UntypedRune
	UntypedFloat
	UntypedComplex
	UntypedString
	UntypedNil

	// aliases
	Byte = Uint8
	Rune = Int32
)

var kinds = [...]string{
	Invalid:        "invalid",
	Bool:           "bool",
	Int:            "int",
	Int8:           "int8",
	Int16:          "int16",
	Int32:          "int32",
	Int64:          "int64",
	Uint:           "uint",
	Uint8:          "uint8",
	Uint16:         "uint16",
	Uint32:         "uint32",
	Uint64:         "uint64",
	Uintptr:        "uintptr",
	Float32:        "float32",
	Float64:        "float64",
	Complex64:      "complex64",
	Complex128:     "complex128",
	Array:          "array",
	Chan:           "chan",
	Func:           "func",
	Interface:      "interface",
	Map:            "map",
	Ptr:            "ptr",
	Slice:          "slice",
	String:         "string",
	Struct:         "struct",
	UnsafePointer:  "unsafe.Pointer",
	UntypedBool:    "untyped.Bool",
	UntypedInt:     "untyped.Int",
	UntypedRune:    "untyped.Rune",
	UntypedFloat:   "untyped.Float",
	UntypedComplex: "untyped.Complex",
	UntypedString:  "untyped.String",
	UntypedNil:     "untyped.Nil",
}

func (k Kind) String() string {
	if int(k) >= len(kinds) {
		k = Invalid
	}
	return kinds[k]
}
