package parser

import (
	"bufio"
	"errors"
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/adl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/ebg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/jsl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/lbg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/mtl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/tgl"
)

// PlatformSpecific - platform-specific interface
type PlatformSpecific interface {
	GenMacro(id string, dw0 uint32, dw1 uint32, ownership uint8) string
	KeywordCheck(line string) bool
}

// padInfo - information about pad
// id        : pad id string
// offset    : the offset of the register address relative to the base
// function  : the string that means the pad function
// dw0       : DW0 register value
// dw1       : DW1 register value
// ownership : host software ownership
type padInfo struct {
	id        string
	offset    uint16
	function  string
	dw0       uint32
	dw1       uint32
	ownership uint8
}

// ParserData - global data
// line       : string from the configuration file
// padmap     : pad info map
// RawFmt     : flag for generating pads config file with DW0/1 reg raw values
// Template   : structure template type of ConfigFile
type ParserData struct {
	platform  PlatformSpecific
	line      string
	padmap    []padInfo
	ownership map[string]uint32
}

// padInfoExtract - adds a new entry to pad info map
// return error status
func (parser *ParserData) padInfoExtract() int {
	var function, id string
	var dw0, dw1 uint32
	if rc := UseTemplate(parser.line, &function, &id, &dw0, &dw1); rc != 0 {
		return rc
	}
	pad := padInfo{id: id,
		function:  function,
		dw0:       dw0,
		dw1:       dw1,
		ownership: 0}
	parser.padmap = append(parser.padmap, pad)
	return 0

}

// communityGroupExtract
func (parser *ParserData) communityGroupExtract() {
	pad := padInfo{function: parser.line}
	parser.padmap = append(parser.padmap, pad)
}

// PlatformSpecificInterfaceSet - specific interface for the platform selected
// in the configuration
func (parser *ParserData) PlatformSpecificInterfaceSet() {
	platform := map[p2m.PlatformType]PlatformSpecific{
		p2m.Sunrise: snr.PlatformSpecific{},
		// See platforms/lbg/macro.go
		p2m.Lewisburg: lbg.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
		},
		p2m.Apollo: apl.PlatformSpecific{},
		p2m.Cannon: cnl.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
		},
		p2m.Tiger:  tgl.PlatformSpecific{},
		p2m.Alder:  adl.PlatformSpecific{},
		p2m.Jasper: jsl.PlatformSpecific{},
		p2m.Meteor: mtl.PlatformSpecific{},
		// See platforms/ebg/macro.go
		p2m.Emmitsburg: ebg.PlatformSpecific{},
	}
	parser.platform = platform[p2m.Config.Platform]
}

// Parse pads groupe information in the inteltool log file
// ConfigFile : name of inteltool log file
func (parser *ParserData) Parse() {
	// Read all lines from inteltool log file
	fmt.Println("Parse IntelTool Log File...")

	// determine the platform type and set the interface for it
	parser.PlatformSpecificInterfaceSet()

	// map of thepad ownership registers for the GPIO controller
	parser.ownership = make(map[string]uint32)

	file := p2m.Config.InputFile
	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		parser.line = scanner.Text()
		isIncluded, _ := common.KeywordsCheck(parser.line, "GPIO Community", "GPIO Group")
		if isIncluded {
			parser.communityGroupExtract()
		} else if parser.platform.KeywordCheck(parser.line) {
			if parser.padInfoExtract() != 0 {
				fmt.Println("...error!")
			}
		}
	}
	fmt.Println("...done!")
}

type PrinterIf interface {
	Linef(lvl int, format string, args ...interface{})
	Line(lvl int, str string)
}

type Generator struct {
	Data      *ParserData // information from the parser
	PrinterIf             // interface for printing
}

// Run - generate a new gpio file based on the information from the parser
func (g Generator) Run() error {
	if g.PrinterIf == nil && g.Data == nil {
		return errors.New("Generator: Incorrect initialization")
	}
	for _, pad := range g.Data.padmap {
		switch pad.dw0 {
		case 0x00000000:
			// titleFprint - print GPIO group title to file
			// /* ------- GPIO Group GPP_L ------- */
			g.Linef(0, "\n\t/* %s */\n", pad.function)
		case 0xffffffff:
			// reservedFprint - print reserved GPIO to file as comment
			// /* GPP_H17 - RESERVED */
			g.Line(2, "\n")
			// small comment about reserved port
			g.Linef(0, "\t/* %s - %s */\n", pad.id, pad.function)
		default:
			// padInfoMacroFprint - print information about current pad to file using
			// special macros:
			// PAD_CFG_NF(GPP_F1, 20K_PU, PLTRST, NF1), /* SATAXPCIE4 */
			platform := g.Data.platform
			macro := platform.GenMacro(pad.id, pad.dw0, pad.dw1, pad.ownership)
			g.Linef(2, "\n\t/* %s - %s */\n\t/* DW0: 0x%0.8x, DW1: 0x%0.8x */\n",
				pad.id, pad.function, pad.dw0, pad.dw1)
			g.Linef(0, "\t%s", macro)
			if p2m.Config.GenLevel == 1 {
				g.Linef(1, "\t/* %s */", pad.function)
			}
			g.Line(0, "\n")
		}
	}
	return nil
}
