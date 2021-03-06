/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * map.go
 *
 *  Created on: Apr 01, 2021
 *      Author: Massimiliano Ghilardi
 */

package types

import "github.com/cosmos72/onejit/go/io"

type Map struct {
	mapTag struct{} // occupies zero bytes
	rtype  Complete
	extra  extra
}

// *Map implements Type

func (t *Map) String() string {
	_ = t.mapTag
	var b builder
	t.WriteTo(&b, fullPkgPath)
	return b.String()
}

func (t *Map) Underlying() Type {
	return t
}

func (t *Map) common() *Complete {
	return &t.rtype
}

func (t *Map) complete() {
	if t.rtype.hash == unknownHash {
		t.rtype.hash = computeMapHash(t.Key(), t.Elem())
	}
}

func (t *Map) WriteTo(dst io.StringWriter, flag verbose) {
	if flag == shortPkgName {
		dst.WriteString(t.rtype.str)
		return
	}
	writeMapTo(dst, t.Key(), t.Elem(), flag)
}

// *Map specific methods

func (t *Map) Key() Type {
	return t.extra.types[0]
}

func (t *Map) Elem() Type {
	return t.rtype.elem
}

type mapKey struct {
	keyElem [2]Type
}

var mapMap = map[mapKey]*Map{}

// create a new Map type
func NewMap(key Type, elem Type) *Map {
	if key.common().flags&flagNotComparable != 0 {
		panic("NewMap: map key type " + key.String() + " is not comparable")
	}
	k := mapKey{[2]Type{key, elem}}
	t := mapMap[k]
	if t != nil {
		return t
	}
	size := sizeOfPtr()
	t = &Map{
		rtype: Complete{
			size:  size,
			align: uint16(size),
			flags: (key.common().flags & elem.common().flags & flagComplete) | flagNotComparable,
			kind:  MapKind,
			elem:  elem,
			hash:  computeMapHash(key, elem),
			str:   makeMapString(key, elem, shortPkgName),
		},
		extra: extra{
			types: k.keyElem[0:1],
		},
	}
	t.rtype.typ = t
	t.rtype.extra = &t.extra
	mapMap[k] = t
	return t
}

func computeMapHash(key Type, elem Type) hash {
	khash := key.common().hash
	ehash := key.common().hash
	if khash == unknownHash || ehash == unknownHash {
		return unknownHash
	}
	return khash.String("map").Hash(ehash)
}

func makeMapString(key Type, elem Type, flag verbose) string {
	var b builder
	writeMapTo(&b, key, elem, flag)
	return b.String()
}

func writeMapTo(dst io.StringWriter, key Type, elem Type, flag verbose) {
	dst.WriteString("map[")
	key.WriteTo(dst, flag)
	dst.WriteString("]")
	elem.WriteTo(dst, flag)
}
