package main

import (
	"fmt"
	"os"
	"time"

	"review.coreboot.org/coreboot.git/util/intelp2m/cli"
	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/generator"
	"review.coreboot.org/coreboot.git/util/intelp2m/generator/header"
	"review.coreboot.org/coreboot.git/util/intelp2m/generator/printer"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

// Version is injected into main during project build
var Version string = "Unknown"

// main
func main() {
	p2m.Config.Version = Version

	cli.ParseOptions()

	if file, err := logs.Init(); err != nil {
		fmt.Printf("logs init error: %v\n", err)
		os.Exit(1)
	} else {
		defer file.Close()
	}

	year, month, day := time.Now().Date()
	hour, min, sec := time.Now().Clock()
	logs.Infof("%d-%d-%d %d:%d:%d", year, month, day, hour, min, sec)
	logs.Infof("============ start ============")

	entries, err := parser.Run()
	if err != nil {
		fmt.Print("failed to run parser")
		os.Exit(1)
	}

	lines, err := generator.Run(entries)
	if err != nil {
		fmt.Print("failed to run generator")
		os.Exit(1)
	}
	lines = header.Add(lines)

	if err := printer.Do(lines); err != nil {
		fmt.Print("printer error")
		os.Exit(1)
	}

	logs.Infof("========== completed ==========")
	os.Exit(0)
}
