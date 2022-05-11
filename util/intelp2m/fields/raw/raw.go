package raw

import (
	"fmt"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
)

type FieldMacros struct {}

func (FieldMacros) DecodeDW0() {
	macro := common.GetMacro()
	// Do not decode, print as is.
	macro.Add(fmt.Sprintf("0x%0.8x", macro.Register(common.PAD_CFG_DW0).ValueGet()))
}

func (FieldMacros) DecodeDW1() {
	macro := common.GetMacro()
	// Do not decode, print as is.
	macro.Add(fmt.Sprintf("0x%0.8x", macro.Register(common.PAD_CFG_DW1).ValueGet()))
}

// GenerateString - generates the entire string of bitfield macros.
func (bitfields FieldMacros) GenerateString() {
	macro := common.GetMacro()
	macro.Add("_PAD_CFG_STRUCT(").Id().Add(", ")
	bitfields.DecodeDW0()
	macro.Add(", ")
	bitfields.DecodeDW1()
	macro.Add("),")
}
