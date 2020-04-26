package main

import "flag"
import "fmt"
import "os"

import "./parser"
import "./config"

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

	infoLevel1 := flag.Bool("i",
		false,
		"\n\tInfo Level 1: adds DW0/DW1 value to the comments:\n" +
		"\t/* GPIO_173 - SDCARD_D0 */\n")

	infoLevel2 := flag.Bool("ii",
		false,
		"Info Level 2: adds original macro to the comments:\n" +
		"\t/* GPIO_173 - SDCARD_D0 (DW0: 0x44000400, DW1: 0x00021000) */\n")

	infoLevel3 := flag.Bool("iii",
		false,
		"Info Level 3: adds information about bit fields that (need to be ignored)\n" +
		"\twere ignored to generate a macro:\n" +
		"\t/* GPIO_173 - SDCARD_D0 (DW0: 0x44000400, DW1: 0x00021000) */\n" +
		"\t/* PAD_CFG_NF_IOSSTATE(GPIO_173, DN_20K, DEEP, NF1, HIZCRx1), */\n")

	infoLevel4 := flag.Bool("iiii",
		false,
		"Info Level 4: show decoded DW0/DW1 register:\n" +
		"\t/* DW0: PAD_TRIG(DEEP) | PAD_BUF(TX_RX_DISABLE) - IGNORED */\n")

	template := flag.Int("t", 0, "template type number\n"+
		"\t0 - inteltool.log (default)\n"+
		"\t1 - gpio.h\n"+
		"\t2 - your template\n\t")

	platform :=  flag.String("p", "snr", "set platform:\n"+
		"\tsnr - Sunrise PCH or Skylake/Kaby Lake SoC\n"+
		"\tlbg - Lewisburg PCH with Xeon SP\n"+
		"\tapl - Apollo Lake SoC\n")

	filedstyle :=  flag.String("fld", "none", "set fileds macros style:\n"+
		"\tcb  - use coreboot style for bit fields macros\n"+
		"\tfsp - use fsp style\n"+
		"\traw - do not convert, print as is\n")

	flag.Parse()

	config.IgnoredFieldsFlagSet(*ignFlag)
	config.NonCheckingFlagSet(*nonCheckFlag)

	if *infoLevel1 {
		config.InfoLevelSet(1)
	} else if *infoLevel2 {
		config.InfoLevelSet(2)
	} else if *infoLevel3 {
		config.InfoLevelSet(3)
	} else if *infoLevel4 {
		config.InfoLevelSet(4)
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

	if config.FldStyleSet(*filedstyle) != 0 {
		fmt.Printf("Error! Unknown bit fields style option -%s!\n", *filedstyle)
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
