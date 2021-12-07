package main

import (
	"embed"
	"errors"
	"flag"
	"fmt"
	"html/template"
	"io/fs"
	"os"
	"path/filepath"
)

//go:embed templates
var templates embed.FS

var data = TemplateData{
	Categories: []Category{
		"laptop",
		"server",
		"desktop",
		"half",
		"mini",
		"settop",
		"eval",
		"sbc",
		"emulation",
		"misc",
		"unclass",
	},
	CategoryNiceNames: map[Category]string{
		"desktop":   "Desktops / Workstations",
		"server":    "Servers",
		"laptop":    "Laptops",
		"half":      "Embedded / PC/104 / Half-size boards",
		"mini":      "Mini-ITX / Micro-ITX / Nano-ITX",
		"settop":    "Set-top-boxes / Thin clients",
		"eval":      "Devel/Eval Boards",
		"sbc":       "Single-Board computer",
		"emulation": "Emulation",
		"misc":      "Miscellaneous",
		"unclass":   "Unclassified",
	},
	BoardsByCategory: map[Category][]Board{},
}

var (
	cbdirFS fs.FS
	cbdir   string
	bsdirFS fs.FS
)

func main() {
	var cbDir, bsDir string
	flag.StringVar(&cbDir, "coreboot-dir", filepath.Join("..", "coreboot.git"), "coreboot.git checkout")
	flag.StringVar(&bsDir, "board-status-dir", filepath.Join("..", "board-status.git"), "board-status.git checkout")
	flag.Parse()

	tpls, err := template.ParseFS(templates, filepath.Join("templates", "*"))
	if err != nil {
		fmt.Fprintf(os.Stderr, "Parsing templates failed: %v\n", err)
		os.Exit(1)
	}

	if _, err := os.Stat(cbDir); errors.Is(err, os.ErrNotExist) {
		fmt.Fprintf(os.Stderr, "coreboot root %s does not exist\n", cbDir)
		os.Exit(1)
	}

	if _, err := os.Stat(bsDir); errors.Is(err, os.ErrNotExist) {
		fmt.Fprintf(os.Stderr, "board-status dir %s does not exist\n", bsDir)
		os.Exit(1)
	}

	cbdirFS = os.DirFS(cbDir)
	cbdir = cbDir
	bsdirFS = os.DirFS(bsDir)

	dirs := make(chan NamedFS)
	go fetchLogs(dirs)
	collectLogs(dirs)

	dirs = make(chan NamedFS)
	go fetchBoards(dirs)
	collectBoards(dirs)

	err = tpls.ExecuteTemplate(os.Stdout, "board-status.html", data)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Executing template failed: %v\n", err)
		os.Exit(1)
	}
}
