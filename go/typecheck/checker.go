/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * checker.go
 *
 *  Created on: Apr 06, 2021
 *      Author: Massimiliano Ghilardi
 */

package typecheck

import (
	"github.com/cosmos72/onejit/go/ast"
	"github.com/cosmos72/onejit/go/strings"
	"github.com/cosmos72/onejit/go/token"
	"github.com/cosmos72/onejit/go/types"
)

type (
	Checker struct {
		scope *types.Scope
		// names and corresponding *types.Object declared in source being typechecked
		defs      *types.Scope
		globals   globaldecls // names declared in source being typechecked
		redefined globaldecls // names declared more than once in source being typechecked
		typemap   TypeMap
		knownpkgs types.Packages // list of known packages
	}
)

var typeAlias ast.Node = &ast.Atom{Tok: token.ASSIGN}

func (c *Checker) Init(scope *types.Scope, knownpkgs types.Packages) {
	c.scope = scope
	c.defs = nil
	c.globals = nil
	c.redefined = nil
	c.typemap = nil
	c.knownpkgs = knownpkgs
}

func (c *Checker) CheckGlobals(source ...ast.Node) {
	c.collectGlobals(source...)
}

func (c *Checker) collectGlobals(nodes ...ast.Node) {
	for _, node := range nodes {
		if node == nil {
			continue
		}
		switch op := node.Op(); op {
		case token.FILE, token.IMPORTS, token.DECLS:
			for i, n := 0, node.Len(); i < n; i++ {
				c.collectGlobals(node.At(i))
			}
		case token.FUNC:
			c.collectFuncDecl(node)
		case token.IMPORT:
			for i, n := 0, node.Len(); i < n; i++ {
				c.collectImportSpec(node.At(i))
			}
		case token.TYPE:
			for i, n := 0, node.Len(); i < n; i++ {
				c.collectTypeSpec(node.At(i))
			}
		case token.VAR, token.CONST:
			for i, n := 0, node.Len(); i < n; i++ {
				c.collectValueSpec(op, node.At(i))
			}
		}
	}
}

func (c *Checker) collectFuncDecl(decl ast.Node) {
	if recv := decl.At(0); recv != nil {
		return // it's a method, not a function
	}
	name := decl.At(1).(*ast.Atom)
	typ := decl.At(2)
	body := decl.At(3)
	c.addLocal(types.FuncObj, name.Lit, typ, body)
}

func (c *Checker) collectImportSpec(spec ast.Node) {
	op := spec.Op()
	if op != token.IMPORT_SPEC {
		panic("invalid import declaration: " + spec.String())
	}
	name := spec.At(0)
	path := spec.At(1).(*ast.Atom)
	// also skip initial and final '"'
	namestr, pathstr := "", strings.Unescape(path.Lit[1:len(path.Lit)-1])
	if name != nil {
		namestr = name.(*ast.Atom).Lit // identifier
	} else if pkg := c.knownpkgs[pathstr]; pkg != nil {
		namestr = pkg.Name()
	} else {
		namestr = strings.Basename(pathstr) // approximate!
	}
	c.addLocal(types.ImportObj, namestr, nil, path)
}

func (c *Checker) collectTypeSpec(spec ast.Node) {
	op := spec.Op()
	if op != token.ASSIGN && op != token.DEFINE {
		panic("invalid type declaration: " + spec.String())
	}
	name := spec.At(0).(*ast.Atom)
	typ := spec.At(1)
	var init ast.Node
	if op == token.ASSIGN {
		init = typeAlias
	}
	c.addLocal(types.TypeObj, name.Lit, typ, init)
}

func (c *Checker) collectValueSpec(op token.Token, spec ast.Node) {
	if spec.Op() != token.VALUE_SPEC {
		panic("invalid " + op.String() + " declaration: " + spec.String())
	}
	names := spec.At(0)
	n := names.Len()
	typ := spec.At(1)
	init := spec.At(2) // init.Op() == token.EXPRS
	oneInitializerPerName := false
	if init != nil {
		switch ninit := init.Len(); ninit {
		case n:
			oneInitializerPerName = true
		case 1:
			init = init.At(0)
		case 0:
			init = nil
		default:
			panic("invalid " + op.String() + " declaration, found " +
				strings.Uint64ToString(uint64(ninit)) + " initializers" +
				", expecting 0, 1 or " + strings.Uint64ToString(uint64(n)) +
				": " + spec.String())
		}
	}

	cls := token2class(op)
	for i := 0; i < n; i++ {
		atom := names.At(i).(*ast.Atom)
		init_i := init
		if oneInitializerPerName {
			init_i = init.At(i)
		}
		c.addLocal(cls, atom.Lit, typ, init_i)
	}
}

func (c *Checker) addLocal(cls types.Class, name string, typ ast.Node, init ast.Node) {
	l := globaldecl{cls, typ, init}
	if c.globals == nil {
		c.globals = make(globaldecls)
	} else if _, ok := c.globals[name]; ok {
		// redefined symbol
		if c.redefined == nil {
			c.redefined = make(globaldecls)
		}
		c.redefined[name] = l
	}
	c.globals[name] = l
}

func (c *Checker) addDef(def *types.Object) {
	if c.defs == nil {
		c.defs = types.NewScope(c.scope)
	}
	c.defs.Insert(def)
}