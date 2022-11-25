package main

import (
	"flag"
	"fmt"
	"os"
	"path/filepath"

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

var (
	// Version is injected into main during project build
	Version string = "Unknown"
)

// printVersion - print the utility version in the console
func printVersion() {
	fmt.Printf("[ intelp2m ] Version: %s\n", Version)
}

// main
func main() {
	// Command line arguments
	inputFilePath := flag.String("file", "inteltool.log",
		"the path to the inteltool log file\n")

	outputFilePath := flag.String("o",
		"generate/gpio.h",
		"the path to the generated file with GPIO configuration\n")

	ignored := flag.Bool("ign", false,
		"exclude fields that should be ignored from advanced macros\n")

	unchecking := flag.Bool("n", false,
		"Generate macros without checking.\n"+
			"\tIn this case, some fields of the configuration registers\n"+
			"\tDW0 will be ignored.\n")

	levels := []*bool{
		flag.Bool("i", false, "Show pads function in the comments"),
		flag.Bool("ii", false, "Show DW0/DW1 value in the comments"),
		flag.Bool("iii", false, "Show ignored bit fields in the comments"),
		flag.Bool("iiii", false, "Show target PAD_CFG() macro in the comments"),
	}

	platform := flag.String("p", "snr", "set platform:\n"+
		"\tsnr - Sunrise PCH or Skylake/Kaby Lake SoC\n"+
		"\tlbg - Lewisburg PCH with Xeon SP\n"+
		"\tapl - Apollo Lake SoC\n"+
		"\tcnl - CannonLake-LP or Whiskeylake/Coffeelake/Cometlake-U SoC\n"+
		"\ttgl - TigerLake-H SoC\n"+
		"\tadl - AlderLake PCH\n"+
		"\tjsl - Jasper Lake SoC\n"+
		"\tmtl - MeteorLake SoC\n"+
		"\tebg - Emmitsburg PCH with Xeon SP\n")

	field := flag.String("fld", "none", "set fields macros style:\n"+
		"\tcb  - use coreboot style for bit fields macros\n"+
		"\tfsp - use fsp style\n"+
		"\traw - do not convert, print as is\n")

	flag.Parse()
	printVersion()

	// settings
	p2m.Config.Version = Version
	p2m.Config.IgnoredFields = *ignored
	p2m.Config.AutoCheck = !(*unchecking)
	for level, set := range levels {
		if *set {
			p2m.Config.GenLevel = level + 1
			fmt.Printf("Info level: Use level %d!\n", level+1)
			break
		}
	}

	if err := p2m.SetPlatformType(*platform); err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	if err := p2m.SetFieldType(*field); err != nil {
		fmt.Printf("Error: %v\n", err)
		os.Exit(1)
	}

	if file, err := os.Open(*inputFilePath); err != nil {
		fmt.Printf("input file error: %v", err)
		os.Exit(1)
	} else {
		p2m.Config.InputFile = file
	}
	defer p2m.Config.InputFile.Close()

	if err := os.MkdirAll(filepath.Dir(*outputFilePath), os.ModePerm); err != nil {
		fmt.Printf("failed to create output directory: %v", err)
		os.Exit(1)
	}
	if file, err := os.Create(*outputFilePath); err != nil {
		fmt.Printf("failed to create output file: %v", err)
		os.Exit(1)
	} else {
		p2m.Config.OutputFile = file
	}
	defer p2m.Config.OutputFile.Close()

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
