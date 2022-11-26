package raw

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
)

type FieldMacros struct{}

func (FieldMacros) DecodeDW0() {
	macro := common.GetMacro()
	dw0 := macro.GetRegisterDW0()
	macro.Add(dw0.String())
}

func (FieldMacros) DecodeDW1() {
	macro := common.GetMacro()
	dw1 := macro.GetRegisterDW1()
	macro.Add(dw1.String())
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
