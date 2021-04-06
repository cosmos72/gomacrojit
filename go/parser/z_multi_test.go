/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * z_multi_test.go
 *
 *  Created on: Mar 28, 2021
 *      Author: Massimiliano Ghilardi
 */

package parser

import (
	"go/build"
	"io"
	"testing"

	"github.com/cosmos72/onejit/go/scanner"
	"github.com/cosmos72/onejit/go/testutil"
	"github.com/cosmos72/onejit/go/token"
)

func TestBuiltinFunctions(t *testing.T) {
	p := &Parser{}
	source := "func append(slice []Type, elems ...Type) []Type\nfunc copy(dst, src []Type) int"
	parseString(t, p, source)
}

func TestParseGoRootFiles(t *testing.T) {
	// t.SkipNow()
	p := Parser{}
	visit := func(t *testing.T, in io.Reader, filename string) {
		p.Init(token.NewFile(filename, 0), in, Default)
		p.ParseFile()
		testutil.CompareErrors(t, filename, errorList{p.Errors()}, nil)
	}
	testutil.VisitDirRecurse(t, visit, build.Default.GOROOT)
}

func parseString(t *testing.T, p *Parser, source string) {
	p.InitString(source, Default)
	filename := "<string>"
	for {
		node := p.Parse()
		if node == nil {
			continue
		} else if tok := node.Op(); tok == token.EOF {
			break
		} else if tok == token.ILLEGAL {
			t.Errorf("parse file %q returned %v", filename, node)
		}
	}
	testutil.CompareErrors(t, filename, errorList{p.Errors()}, nil)
}

type errorList struct {
	errors *[]*scanner.Error
}

func (list errorList) Len() int {
	return len(*list.errors)
}

func (list errorList) String(i int) string {
	return (*list.errors)[i].Msg
}

func (list errorList) Error(i int) error {
	return (*list.errors)[i]
}
