/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, g. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * generic.go
 *
 *  Created on: Mar 23, 2021
 *      Author: Massimiliano Ghilardi
 */

package ast

import (
	"github.com/cosmos72/onejit/go/io"
	"github.com/cosmos72/onejit/go/strings"
	"github.com/cosmos72/onejit/go/token"
)

type GenericType struct {
	Atom   // always token.GENERIC
	Params *List
	Type   Node
}

func (g *GenericType) Len() int {
	return 2
}

func (g *GenericType) At(i int) (child Node) {
	// cannot use choose2() here: assigning a nil pointer to an interface
	// creates a "half-nil" interface
	switch i {
	case 0:
		if node := g.Params; node != nil {
			child = node
		}
	case 1:
		child = g.Type
	default:
		outOfRange()
	}
	return child
}

func (g *GenericType) End() token.Pos {
	if g.Type != nil {
		return g.Type.End()
	} else if g.Params != nil {
		return g.Params.End()
	} else {
		return g.Atom.End()
	}
}

func (g *GenericType) String() string {
	if g == nil {
		return "nil"
	}
	var buf strings.Builder
	g.WriteTo(&buf)
	return buf.String()
}

func (g *GenericType) WriteTo(out io.StringWriter) {
	if g == nil {
		out.WriteString("nil")
	} else {
		writeListTo(out, g)
	}
}
