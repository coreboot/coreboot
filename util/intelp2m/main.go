package main

import (
	"flag"
	"fmt"
	"os"

	"review.coreboot.org/coreboot.git/util/intelp2m/config"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

// generateOutputFile - generates include file
// parser            : parser data structure
func generateOutputFile(parser *parser.ParserData) (err error) {

	config.OutputGenFile.WriteString(`/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CFG_GPIO_H
#define CFG_GPIO_H

#include <gpio.h>

/* Pad configuration was generated automatically using intelp2m utility */
static const struct pad_config gpio_table[] = {
`)
	// Add the pads map
	parser.PadMapFprint()
	config.OutputGenFile.WriteString(`};

#endif /* CFG_GPIO_H */
`)
	return nil
}

// main
func main() {
	// Command line arguments
	inputFileName := flag.String("file",
		"inteltool.log",
		"the path to the inteltool log file\n")

	outputFileName := flag.String("o",
		"generate/gpio.h",
		"the path to the generated file with GPIO configuration\n")

	ignFlag := flag.Bool("ign",
		false,
		"exclude fields that should be ignored from advanced macros\n")

	nonCheckFlag := flag.Bool("n",
		false,
		"Generate macros without checking.\n" +
		"\tIn this case, some fields of the configuration registers\n" +
		"\tDW0 will be ignored.\n")

	infoLevels := []*bool {
		flag.Bool("i",    false, "Show pads function in the comments"),
		flag.Bool("ii",   false, "Show DW0/DW1 value in the comments"),
		flag.Bool("iii",  false, "Show ignored bit fields in the comments"),
		flag.Bool("iiii", false, "Show target PAD_CFG() macro in the comments"),
	}

	template := flag.Int("t", 0, "template type number\n"+
		"\t0 - inteltool.log (default)\n"+
		"\t1 - gpio.h\n"+
		"\t2 - your template\n\t")

	platform :=  flag.String("p", "snr", "set platform:\n"+
		"\tsnr - Sunrise PCH or Skylake/Kaby Lake SoC\n"+
		"\tlbg - Lewisburg PCH with Xeon SP\n"+
		"\tapl - Apollo Lake SoC\n"+
		"\tcnl - CannonLake-LP or Whiskeylake/Coffeelake/Cometlake-U SoC\n"+
		"\tadl - AlderLake PCH\n")

	fieldstyle :=  flag.String("fld", "none", "set fields macros style:\n"+
		"\tcb  - use coreboot style for bit fields macros\n"+
		"\tfsp - use fsp style\n"+
		"\traw - do not convert, print as is\n")

	flag.Parse()

	config.IgnoredFieldsFlagSet(*ignFlag)
	config.NonCheckingFlagSet(*nonCheckFlag)

	for level, flag := range infoLevels {
		if *flag {
			config.InfoLevelSet(level + 1)
			fmt.Printf("Info level: Use level %d!\n", level + 1)
			break
		}
	}

	if !config.TemplateSet(*template) {
		fmt.Printf("Error! Unknown template format of input file!\n")
		os.Exit(1)
	}

	if valid := config.PlatformSet(*platform); valid != 0 {
		fmt.Printf("Error: invalid platform -%s!\n", *platform)
		os.Exit(1)
	}

	fmt.Println("Log file:", *inputFileName)
	fmt.Println("Output generated file:", *outputFileName)

	inputRegDumpFile, err := os.Open(*inputFileName)
	if err != nil {
		fmt.Printf("Error: inteltool log file was not found!\n")
		os.Exit(1)
	}

	if config.FldStyleSet(*fieldstyle) != 0 {
		fmt.Printf("Error! Unknown bit fields style option -%s!\n", *fieldstyle)
		os.Exit(1)
	}

	// create dir for output files
	err = os.MkdirAll("generate", os.ModePerm)
	if err != nil {
		fmt.Printf("Error! Can not create a directory for the generated files!\n")
		os.Exit(1)
	}

	// create empty gpio.h file
	outputGenFile, err := os.Create(*outputFileName)
	if err != nil {
		fmt.Printf("Error: unable to generate GPIO config file!\n")
		os.Exit(1)
	}

	defer inputRegDumpFile.Close()
	defer outputGenFile.Close()

	config.OutputGenFile = outputGenFile
	config.InputRegDumpFile = inputRegDumpFile

	parser := parser.ParserData{}
	parser.Parse()

	// gpio.h
	err = generateOutputFile(&parser)
	if err != nil {
		fmt.Printf("Error! Can not create the file with GPIO configuration!\n")
		os.Exit(1)
	}
}
