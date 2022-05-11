package parser

import (
	"bufio"
	"fmt"
	"strings"
	"strconv"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/lbg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/adl"
	"review.coreboot.org/coreboot.git/util/intelp2m/config"
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

// generate - wrapper for Fprintf(). Writes text to the file specified
// in config.OutputGenFile
func (info *padInfo) generate(lvl int, line string, a ...interface{}) {
	if config.InfoLevelGet() >= lvl {
		fmt.Fprintf(config.OutputGenFile, line, a...)
	}
}

// titleFprint - print GPIO group title to file
// /* ------- GPIO Group GPP_L ------- */
func (info *padInfo) titleFprint() {
	info.generate(0, "\n\t/* %s */\n", info.function)
}

// reservedFprint - print reserved GPIO to file as comment
// /* GPP_H17 - RESERVED */
func (info *padInfo) reservedFprint() {
	info.generate(2, "\n")
	// small comment about reserved port
	info.generate(0, "\t/* %s - %s */\n", info.id, info.function)
}

// padInfoMacroFprint - print information about current pad to file using
// special macros:
// PAD_CFG_NF(GPP_F1, 20K_PU, PLTRST, NF1), /* SATAXPCIE4 */
// gpio  : gpio.c file descriptor
// macro : string of the generated macro
func (info *padInfo) padInfoMacroFprint(macro string) {
	info.generate(2,
		"\n\t/* %s - %s */\n\t/* DW0: 0x%0.8x, DW1: 0x%0.8x */\n",
		info.id,
		info.function,
		info.dw0,
		info.dw1)
	info.generate(0, "\t%s", macro)
	if config.InfoLevelGet() == 1 {
		info.generate(1, "\t/* %s */", info.function)
	}
	info.generate(0, "\n")
}

// ParserData - global data
// line       : string from the configuration file
// padmap     : pad info map
// RawFmt     : flag for generating pads config file with DW0/1 reg raw values
// Template   : structure template type of ConfigFile
type ParserData struct {
	platform   PlatformSpecific
	line       string
	padmap     []padInfo
	ownership  map[string]uint32
}

// hostOwnershipGet - get the host software ownership value for the corresponding
// pad ID
// id : pad ID string
// return the host software ownership form the parser struct
func (parser *ParserData) hostOwnershipGet(id string) uint8 {
	var ownership uint8 = 0
	status, group := parser.platform.GroupNameExtract(id)
	if config.TemplateGet() == config.TempInteltool && status {
		numder, _ := strconv.Atoi(strings.TrimLeft(id, group))
		if (parser.ownership[group] & (1 << uint8(numder))) != 0 {
			ownership = 1
		}
	}
	return ownership
}

// padInfoExtract - adds a new entry to pad info map
// return error status
func (parser *ParserData) padInfoExtract() int {
	var function, id string
	var dw0, dw1 uint32
	var template = map[int]template{
		config.TempInteltool: useInteltoolLogTemplate,
		config.TempGpioh    : useGpioHTemplate,
		config.TempSpec     : useYourTemplate,
	}
	if template[config.TemplateGet()](parser.line, &function, &id, &dw0, &dw1) == 0 {
		pad := padInfo{id: id,
			function: function,
			dw0: dw0,
			dw1: dw1,
			ownership: parser.hostOwnershipGet(id)}
		parser.padmap = append(parser.padmap, pad)
		return 0
	}
	fmt.Printf("This template (%d) does not match!\n", config.TemplateGet())
	return -1
}

// communityGroupExtract
func (parser *ParserData) communityGroupExtract() {
	pad := padInfo{function: parser.line}
	parser.padmap = append(parser.padmap, pad)
}

// PlatformSpecificInterfaceSet - specific interface for the platform selected
// in the configuration
func (parser *ParserData) PlatformSpecificInterfaceSet() {
	var platform = map[uint8]PlatformSpecific {
		config.SunriseType   : snr.PlatformSpecific{},
		// See platforms/lbg/macro.go
		config.LewisburgType : lbg.PlatformSpecific{
			InheritanceTemplate : snr.PlatformSpecific{},
		},
		config.ApolloType    : apl.PlatformSpecific{},
		config.CannonType    : cnl.PlatformSpecific{
			InheritanceTemplate : snr.PlatformSpecific{},
		},
		config.AlderType    : adl.PlatformSpecific{},
	}
	parser.platform = platform[config.PlatformGet()]
}

// PadMapFprint - print pad info map to file
func (parser *ParserData) PadMapFprint() {
	for _, pad := range parser.padmap {
		switch pad.dw0 {
		case 0:
			pad.titleFprint()
		case 0xffffffff:
			pad.reservedFprint()
		default:
			str := parser.platform.GenMacro(pad.id, pad.dw0, pad.dw1, pad.ownership)
			pad.padInfoMacroFprint(str)
		}
	}
}

// Register - read specific platform registers (32 bits)
// line         : string from file with pad config map
// nameTemplate : register name femplate to filter parsed lines
// return
//     valid  : true if the dump of the register in intertool.log is set in accordance
//              with the template
//     name   : full register name
//     offset : register offset relative to the base address
//     value  : register value
func (parser *ParserData) Register(nameTemplate string) (
		valid bool, name string, offset uint32, value uint32) {
	if strings.Contains(parser.line, nameTemplate) &&
		config.TemplateGet() == config.TempInteltool {
		if registerInfoTemplate(parser.line, &name, &offset, &value) == 0 {
			fmt.Printf("\n\t/* %s : 0x%x : 0x%x */\n", name, offset, value)
			return true, name, offset, value
		}
	}
	return false, "ERROR", 0, 0
}

// padOwnershipExtract - extract Host Software Pad Ownership from inteltool dump
//                       return true if success
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
//                           information from the inteltool log was successfully parsed.
func (parser *ParserData) padConfigurationExtract() bool {
	// Only for Sunrise or CannonLake, and only for inteltool.log file template
	if config.TemplateGet() != config.TempInteltool || config.IsPlatformApollo() {
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
		isIncluded, _ := common.KeywordsCheck(parser.line, "GPIO Community", "GPIO Group");
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
