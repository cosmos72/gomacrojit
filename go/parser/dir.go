/*
 * Copyright (C) 2021 Massimiliano Ghilardi
 *
 *     This Source Code Form is subject to the terms of the Mozilla Public
 *     License, v. 2.0. If a copy of the MPL was not distributed with this
 *     file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 *
 * file.go
 *
 *  Created on: Mar 23, 2021
 *      Author: Massimiliano Ghilardi
 */

package parser

import (
	"github.com/cosmos72/onejit/go/ast"
	"github.com/cosmos72/onejit/go/config"
	"github.com/cosmos72/onejit/go/io"
	"github.com/cosmos72/onejit/go/token"
)

type FileOpener = func() (filename string, src io.ReadCloser)

// parse all files in a directory.
// repeatedly calls Parser.InitParseFile() until opener returns nil.
// does NOT clear accumulated errors
func (p *Parser) InitParseDir(fset *token.FileSet, opener FileOpener, mode Mode, lang config.Lang) *ast.Dir {
	dir := &ast.Dir{Atom: ast.Atom{Tok: token.DIR}, FileSet: fset}
	var files []*ast.File
	for {
		file := p.InitParseFile(fset, opener, mode, lang)
		if file == nil {
			break
		}
		files = append(files, file)
	}
	dir.Files = files
	return dir
}

// open and parse a single file in a directory.
// calls in sequence: opener(), Parser.Init(), Parser.ParseFile()
// does NOT clear accumulated errors
func (p *Parser) InitParseFile(fset *token.FileSet, opener FileOpener, mode Mode, lang config.Lang) *ast.File {
	filename, src := opener()
	if src == nil {
		return nil
	}
	defer src.Close()
	p.Init(fset.AddFile(filename), src, mode, lang)
	ret := p.ParseFile()
	return ret
}
