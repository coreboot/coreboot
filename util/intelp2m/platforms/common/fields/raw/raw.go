package raw

import "review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"

type FieldCollection struct{}

// DecodeDW0() decodes DW0 register value and adds it to the macro string
func (FieldCollection) DecodeDW0(macro *common.Macro) *common.Macro {
	dw0 := macro.Platform.GetRegisterDW0()
	return macro.Add(dw0.String())
}

// DecodeDW1() decodes DW1 register value and adds it to the macro string
func (FieldCollection) DecodeDW1(macro *common.Macro) *common.Macro {
	dw1 := macro.Platform.GetRegisterDW1()
	return macro.Add(dw1.String())
}

// GenerateMacro() generates the field macro collection and adds it to the macro string
func (f FieldCollection) GenerateMacro(macro *common.Macro) *common.Macro {
	macro.Add("_PAD_CFG_STRUCT(").Id().Add(", ")
	f.DecodeDW0(macro).Add(", ")
	return f.DecodeDW1(macro).Add("),")
}
