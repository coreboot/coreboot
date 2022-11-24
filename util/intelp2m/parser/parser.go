package parser

import (
	"bufio"
	"errors"
	"fmt"
	"strconv"
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/config"
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
	GroupNameExtract(line string) (bool, string)
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

// hostOwnershipGet - get the host software ownership value for the corresponding
// pad ID
// id : pad ID string
// return the host software ownership form the parser struct
func (parser *ParserData) hostOwnershipGet(id string) uint8 {
	var ownership uint8 = 0
	_, group := parser.platform.GroupNameExtract(id)
	numder, _ := strconv.Atoi(strings.TrimLeft(id, group))
	if (parser.ownership[group] & (1 << uint8(numder))) != 0 {
		ownership = 1
	}
	return ownership
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
		ownership: parser.hostOwnershipGet(id)}
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
	var platform = map[uint8]PlatformSpecific{
		config.SunriseType: snr.PlatformSpecific{},
		// See platforms/lbg/macro.go
		config.LewisburgType: lbg.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
		},
		config.ApolloType: apl.PlatformSpecific{},
		config.CannonType: cnl.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
		},
		config.TigerType:  tgl.PlatformSpecific{},
		config.AlderType:  adl.PlatformSpecific{},
		config.JasperType: jsl.PlatformSpecific{},
		config.MeteorType: mtl.PlatformSpecific{},
		// See platforms/ebg/macro.go
		config.EmmitsburgType: ebg.PlatformSpecific{
			InheritanceTemplate: cnl.PlatformSpecific{
				InheritanceTemplate: snr.PlatformSpecific{},
			},
		},
	}
	parser.platform = platform[config.PlatformGet()]
}

// Register - read specific platform registers (32 bits)
// line         : string from file with pad config map
// nameTemplate : register name femplate to filter parsed lines
// return
//
//	valid  : true if the dump of the register in intertool.log is set in accordance
//	         with the template
//	name   : full register name
//	offset : register offset relative to the base address
//	value  : register value
func (parser *ParserData) Register(nameTemplate string) (
	valid bool,
	name string,
	offset uint32,
	value uint32,
) {
	if strings.Contains(parser.line, nameTemplate) {
		if registerInfoTemplate(parser.line, &name, &offset, &value) == 0 {
			fmt.Printf("\n\t/* %s : 0x%x : 0x%x */\n", name, offset, value)
			return true, name, offset, value
		}
	}
	return false, "ERROR", 0, 0
}

// padOwnershipExtract - extract Host Software Pad Ownership from inteltool dump
//
//	return true if success
func (parser *ParserData) padOwnershipExtract() bool {
	var group string
	status, name, offset, value := parser.Register("HOSTSW_OWN_GPP_")
	if status {
		_, group = parser.platform.GroupNameExtract(parser.line)
		parser.ownership[group] = value
		fmt.Printf("\n\t/* padOwnershipExtract: [offset 0x%x] %s = 0x%x */\n",
			offset, name, parser.ownership[group])
	}
	return status
}

// padConfigurationExtract - reads GPIO configuration registers and returns true if the
//
//	information from the inteltool log was successfully parsed.
func (parser *ParserData) padConfigurationExtract() bool {
	// Only for Sunrise or CannonLake, and only for inteltool.log file template
	if config.IsPlatformApollo() {
		return false
	}
	return parser.padOwnershipExtract()
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

	scanner := bufio.NewScanner(config.InputRegDumpFile)
	for scanner.Scan() {
		parser.line = scanner.Text()
		isIncluded, _ := common.KeywordsCheck(parser.line, "GPIO Community", "GPIO Group")
		if isIncluded {
			parser.communityGroupExtract()
		} else if !parser.padConfigurationExtract() && parser.platform.KeywordCheck(parser.line) {
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
			if config.InfoLevelGet() == 1 {
				g.Linef(1, "\t/* %s */", pad.function)
			}
			g.Line(0, "\n")
		}
	}
	return nil
}
