package main

import (
	"fmt"
	"os"
	"path/filepath"

	"review.coreboot.org/coreboot.git/util/intelp2m/cli"
	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

type Printer struct{}

func (Printer) Linef(lvl int, format string, args ...interface{}) {
	if p2m.Config.GenLevel >= lvl {
		fmt.Fprintf(p2m.Config.OutputFile, format, args...)
	}
}

func (Printer) Line(lvl int, str string) {
	if p2m.Config.GenLevel >= lvl {
		fmt.Fprint(p2m.Config.OutputFile, str)
	}
}

// Version is injected into main during project build
var Version string = "Unknown"

// main
func main() {
	p2m.Config.Version = Version

	cli.ParseOptions()

	if file, err := os.Open(p2m.Config.InputPath); err != nil {
		fmt.Printf("input file error: %v\n", err)
		os.Exit(1)
	} else {
		p2m.Config.InputFile = file
		defer file.Close()
	}

	if err := os.MkdirAll(filepath.Dir(p2m.Config.OutputPath), os.ModePerm); err != nil {
		fmt.Printf("failed to create output directory: %v\n", err)
		os.Exit(1)
	}
	if file, err := os.Create(p2m.Config.OutputPath); err != nil {
		fmt.Printf("failed to create output file: %v\n", err)
		os.Exit(1)
	} else {
		p2m.Config.OutputFile = file
		defer file.Close()
	}

	prs := parser.ParserData{}
	prs.Parse()

	generator := parser.Generator{
		PrinterIf: Printer{},
		Data:      &prs,
	}
	header := fmt.Sprintf(`/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CFG_GPIO_H
#define CFG_GPIO_H

#include <gpio.h>

/* Pad configuration was generated automatically using intelp2m %s */
static const struct pad_config gpio_table[] = {`, Version)
	p2m.Config.OutputFile.WriteString(header + "\n")
	// Add the pads map

	if err := generator.Run(); err != nil {
		fmt.Printf("Error: %v", err)
		os.Exit(1)
	}
	p2m.Config.OutputFile.WriteString(`};

#endif /* CFG_GPIO_H */
`)
	os.Exit(0)
}
