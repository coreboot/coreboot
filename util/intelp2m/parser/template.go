package parser

import (
	"fmt"
	"strings"
	"unicode"
)

const INTSEL_MASK uint32 = 0xffffff00

type template func(string, *string, *string, *uint32, *uint32) int

// extractPadFuncFromComment
// line   : string from file with pad config map
// return : pad function string
func extractPadFuncFromComment(line string) string {
	if !strings.Contains(line, "/*") && !strings.Contains(line, "*/") {
		return ""
	}

	fields := strings.Fields(line)
	for i, field := range fields {
		if field == "/*" && len(fields) >= i+2 {
			return fields[i+1]
		}
	}
	return ""
}

// tokenCheck
func tokenCheck(c rune) bool {
	return c != '_' && c != '#' && !unicode.IsLetter(c) && !unicode.IsNumber(c)
}

// UseTemplate
// line      : string from file with pad config map
// *function : the string that means the pad function
// *id       : pad id string
// *dw0      : DW0 register value
// *dw1      : DW1 register value
// return
//   error status
func UseTemplate(line string, function *string, id *string, dw0 *uint32, dw1 *uint32) int {
	var val uint64
	// 0x0520: 0x0000003c44000600 GPP_B12  SLP_S0#
	// 0x0438: 0xffffffffffffffff GPP_C7   RESERVED
	if fields := strings.FieldsFunc(line, tokenCheck); len(fields) >= 4 {
		fmt.Sscanf(fields[1], "0x%x", &val)
		*dw0 = uint32(val & 0xffffffff)
		*dw1 = uint32(val >> 32)
		*id = fields[2]
		*function = fields[3]
		// Sometimes the configuration file contains compound functions such as
		// SUSWARN#/SUSPWRDNACK. Since the template does not take this into account,
		// need to collect all parts of the pad function back into a single word
		for i := 4; i < len(fields); i++ {
			*function += "/" + fields[i]
		}
		// clear RO Interrupt Select (INTSEL)
		*dw1 &= INTSEL_MASK
	}
	return 0
}

// registerInfoTemplate
// line    : (in)  string from file with pad config map
// *name   : (out) register name
// *offset : (out) offset name
// *value  : (out) register value
// return
//   error status
func registerInfoTemplate(line string, name *string, offset *uint32, value *uint32) int {
	// 0x0088: 0x00ffffff (HOSTSW_OWN_GPP_F)
	// 0x0100: 0x00000000 (GPI_IS_GPP_A)
	if fields := strings.FieldsFunc(line, tokenCheck); len(fields) == 3 {
		*name = fields[2]
		fmt.Sscanf(fields[1], "0x%x", value)
		fmt.Sscanf(fields[0], "0x%x", offset)
		return 0
	}
	return -1
}
