package template

import (
	"fmt"
	"strings"
	"unicode"

	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

func token(c rune) bool {
	return c != '_' && c != '#' && !unicode.IsLetter(c) && !unicode.IsNumber(c)
}

// Apply
// line     : string from file with pad config map
// return
// function : the string that means the pad function
// id       : pad id string
// dw0      : DW0 register value
// dw1      : DW1 register value
// err      : error
func Apply(line string) (function string, id string, dw0 uint32, dw1 uint32, err error) {
	// 0x0520: 0x0000003c44000600 GPP_B12  SLP_S0#
	// 0x0438: 0xffffffffffffffff GPP_C7   RESERVED
	slices := strings.FieldsFunc(line, token)
	number := len(slices)
	if number >= 4 {
		var val uint64
		fmt.Sscanf(slices[1], "0x%x", &val)
		dw0 = uint32(val & 0xffffffff)
		dw1 = uint32(val >> 32)
		id = slices[2]
		function = slices[3]
		// Sometimes the configuration file contains compound functions such as
		// SUSWARN#/SUSPWRDNACK. Since the template does not take this into account,
		// need to collect all parts of the pad function back into a single word
		for i := 4; i < len(slices); i++ {
			function += "/" + slices[i]
		}
		// clear RO Interrupt Select (INTSEL)
		dw1 &^= bits.DW1[bits.DW1InterruptSelect]
		return function, id, dw0, dw1, nil
	}
	logs.Errorf("template: more than %d elements are needed", number)
	return "", "", 0, 0, fmt.Errorf("template error")
}
