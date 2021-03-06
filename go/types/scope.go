/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * scope.go
 *
 *  Created on: Apr 05, 2021
 *      Author: Massimiliano Ghilardi
 */

package types

import (
	"github.com/cosmos72/onejit/go/config"
	"github.com/cosmos72/onejit/go/sort"
)

type (
	Class uint8

	Object struct {
		cls  Class
		name string
		typ  *Complete
		// constant.Value for constants, *Package for imports,
		// Label for global variables and functions/methods
		// Generic for generic types and generic functions/methods
		value interface{}
		// auxiliary data describing object's declaration.
		// currently used by go/typecheck to store *typecheck.Decl
		decl interface{}
	}

	Scope struct {
		parent *Scope
		m      map[string]*Object
	}

	Package struct {
		name    string
		pkgPath string
		scope   Scope
	}

	// map path -> *Package of all known packages
	Packages map[string]*Package
)

const (
	InvalidObj Class = iota
	BuiltinObj
	ConstObj
	FuncObj
	ImportObj
	TypeObj
	VarObj
	GenericFuncObj
	GenericTypeObj
)

// --------------------------- Class -------------------------------------------

func (cl Class) String() string {
	var str string
	switch cl {
	case BuiltinObj:
		str = "builtin"
	case ConstObj:
		str = "const"
	case FuncObj:
		str = "func"
	case ImportObj:
		str = "import"
	case TypeObj:
		str = "type"
	case VarObj:
		str = "var"
	case GenericFuncObj:
		str = "genericfunc"
	case GenericTypeObj:
		str = "generictype"
	default:
		str = "invalid"
	}
	return str
}

// --------------------------- Object ------------------------------------------

func NewObject(cls Class, name string, typ *Complete) *Object {
	obj := Object{cls, name, typ, nil, nil}
	obj.checkValid()
	return &obj
}

func (obj *Object) Class() Class {
	return obj.cls
}

func (obj *Object) Decl() interface{} {
	return obj.decl
}

func (obj *Object) Name() string {
	return obj.name
}

func (obj *Object) Type() *Complete {
	return obj.typ
}

/**
 * Value returns the value associated to given *Object, or nil if no value was set.
 * If non nil, it is expected to be:
 *   constant.Value for constants
 *   *Package for imports,
 *   Label for global variables and functions/methods
 *   Generic for generic types and generic functions/methods
 */
func (obj *Object) Value() interface{} {
	return obj.value
}

func (obj *Object) SetDecl(decl interface{}) *Object {
	obj.decl = decl
	return obj
}

func (obj *Object) SetType(t *Complete) {
	if t == nil {
		panic("Object.SetType(nil)")
	} else if obj.typ != nil {
		panic("Object.SetType: type is already set")
	}
	obj.checkValidType(t)
	obj.typ = t
}

func (obj *Object) SetValue(value interface{}) *Object {
	obj.value = value
	return obj
}

func (obj *Object) String() string {
	return obj.cls.String() + " " + obj.name
}

func (obj *Object) checkValid() {
	if obj == nil {
		panic("Object is nil")
	} else if len(obj.name) == 0 {
		panic("Object has empty name")
	} else {
		obj.checkValidType(obj.typ)
	}
}

func (obj *Object) checkValidType(t *Complete) {
	if t == nil {
		return
	}
	switch obj.Class() {
	case BuiltinObj:
		if !isBuiltinType(t) {
			panic("type of BuiltinObj must be created with NewBuiltin")
		}
		return
	case ConstObj:
		if !t.kind.IsBasic() {
			panic("type of ConstObj must be one of the basic types")
		}
	case FuncObj:
		if t.kind != FuncKind {
			panic("type of FuncObj must be a function")
		} else if len(t.Name()) != 0 {
			panic("type of FuncObj must be unnamed")
		}
	case ImportObj:
		if t != nil {
			panic("type of ImportObj must be nil")
		}
		return
	case TypeObj:
		// nothing to do
	case VarObj:
		if t.kind.IsUntyped() {
			panic("type of VarObj cannot be untyped.*")
		}
	default:
		panic("Object has invalid class")
	}
	if isBuiltinType(t) {
		panic("type of " + obj.cls.String() + " cannot be builtin")
	}
}

func isBuiltinType(t *Complete) bool {
	return t.typ == nil && t.extra != nil
}

// --------------------------- Scope -------------------------------------------

func NewScope(parent *Scope) *Scope {
	return &Scope{parent: parent}
}

// insert obj into Scope s. overwrites any existing object with the same name
func (s *Scope) Insert(obj *Object) {
	obj.checkValid()
	if s.m == nil {
		s.m = make(map[string]*Object)
	}
	s.m[obj.name] = obj
}

func (s *Scope) Len() int {
	return len(s.m)
}

// search object by name in specified Scope.
func (s *Scope) Lookup(name string) *Object {
	if s == nil {
		return nil
	}
	return s.m[name]
}

// search object by name in specified Scope and all its parent scopes.
func (s *Scope) LookupParent(name string) (*Scope, *Object) {
	for ; s != nil; s = s.parent {
		if obj := s.m[name]; obj != nil {
			return s, obj
		}
	}
	return nil, nil
}

// return the sorted names of all objects in Scope
func (s *Scope) Names() []string {
	if s == nil {
		return nil
	}
	names := make([]string, len(s.m))
	i := 0
	for name := range s.m {
		names[i] = name
		i++
	}
	if len(names) > 1 {
		sort.Strings(names)
	}
	return names
}

func (s *Scope) Parent() *Scope {
	return s.parent
}

// --------------------------- Package -------------------------------------------

func NewPackage(name string, pkgPath string) *Package {
	return &Package{
		name:    name,
		pkgPath: pkgPath,
		scope:   Scope{parent: Universe()},
	}
}

func (pkg *Package) Name() string {
	return pkg.name
}

func (pkg *Package) PkgPath() string {
	return pkg.pkgPath
}

func (pkg *Package) Scope() *Scope {
	return &pkg.scope
}

// --------------------------- Universe -------------------------------------------

var (
	universeLinux386 = makeUniverse(basicTypesLinux386)
	universeOther32  = makeUniverse(basicTypesOther32)
	universe64       = makeUniverse(basicTypes64)
)

// return the top-level scope, containing all predeclared objects of Go.
// result depends on config.Target()
func Universe() *Scope {
	var universe *Scope
	os, arc := config.Target()
	if arc.Bits() > 32 {
		universe = universe64
	} else if os == config.Linux && arc == config.I386 {
		universe = universeLinux386
	} else {
		universe = universeOther32
	}
	return universe
}

func makeUniverse(basic []*Complete) *Scope {
	s := Scope{}
	for _, b := range basic[:Complex128+1] {
		if b != nil {
			s.Insert(&Object{TypeObj, b.Name(), b, nil, nil})
		}
	}
	errorType := NewNamed("error", "")
	errorType.SetUnderlying(NewInterface(nil, Method{
		Name: "Error",
		Type: NewFunc(nil, []Type{basic[String].Type()}, false),
	}))

	objs := []Object{
		// each BuiltinObj has a unique type
		NewBuiltinObj("append", 2, 1, true),
		NewBuiltinObj("cap", 1, 0, false),
		NewBuiltinObj("close", 1, 0, false),
		NewBuiltinObj("complex", 2, 1, false),
		NewBuiltinObj("copy", 2, 1, false),
		NewBuiltinObj("delete", 2, 0, false),
		NewBuiltinObj("imag", 1, 1, false),
		NewBuiltinObj("len", 1, 1, false),
		NewBuiltinObj("make", 2, 1, true),
		NewBuiltinObj("new", 1, 1, true),
		NewBuiltinObj("panic", 1, 0, false),
		NewBuiltinObj("print", 1, 0, true),
		NewBuiltinObj("println", 1, 0, true),
		NewBuiltinObj("real", 1, 1, false),
		NewBuiltinObj("recover", 0, 1, false),

		{ConstObj, "false", basic[UntypedBool], nil, nil},
		{ConstObj, "iota", basic[UntypedInt], nil, nil},
		{ConstObj, "nil", basic[UntypedNil], nil, nil},
		{ConstObj, "true", basic[UntypedBool], nil, nil},

		{TypeObj, "byte", basic[Uint8], nil, nil},
		{TypeObj, "rune", basic[Int32], nil, nil},
		{TypeObj, "string", basic[String], nil, nil},
		{TypeObj, "error", errorType.common(), nil, nil},
	}
	for i := range objs {
		s.Insert(&objs[i])
	}
	return &s
}
