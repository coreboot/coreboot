package jsl

import (
	"fmt"
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/fields"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

const (
	PAD_CFG_DW0_RO_FIELDS = (0x1 << 27) | (0x1 << 18) | (0x3f << 11) | (0x3f << 2) | (0x1 << 1)
	PAD_CFG_DW1_RO_FIELDS = 0xdfffc3ff
)

type InheritanceMacro interface {
	Pull()
	GpiMacroAdd()
	GpoMacroAdd()
	NativeFunctionMacroAdd()
	NoConnMacroAdd()
}

type PlatformSpecific struct {
	InheritanceMacro
}

// RemmapRstSrc - remmap Pad Reset Source Config
func (PlatformSpecific) RemmapRstSrc() {
	macro := common.GetMacro()
	if strings.Contains(macro.PadIdGet(), "GPP_F") ||
		strings.Contains(macro.PadIdGet(), "GPP_B") ||
		strings.Contains(macro.PadIdGet(), "GPP_A") ||
		strings.Contains(macro.PadIdGet(), "GPP_S") ||
		strings.Contains(macro.PadIdGet(), "GPP_R") {
		// See reset map for the Jasper Lake Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/jasperlake/gpio.c#L21
		// remmap is not required because it is the same as common.
		return
	}

	dw0 := macro.GetRegisterDW0()
	var remapping = map[uint32]uint32{
		0: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
		1: bits.RstCfgDEEP << bits.DW0PadRstCfg,
		2: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
	}
	resetsrc, valid := remapping[dw0.GetResetConfig()]
	if valid {
		// dw0.SetResetConfig(resetsrc)
		ResetConfigFieldVal := (dw0.Value & 0x3fffffff) | remapping[dw0.GetResetConfig()]
		dw0.Value = ResetConfigFieldVal
	} else {
		fmt.Println("Invalid Pad Reset Config [ 0x", resetsrc, " ] for ", macro.PadIdGet())
	}
	mask := bits.DW0[bits.DW0PadRstCfg]
	dw0.CntrMaskFieldsClear(mask)
}

// Adds The Pad Termination (TERM) parameter from PAD_CFG_DW1 to the macro
// as a new argument
func (platform PlatformSpecific) Pull() {
	platform.InheritanceMacro.Pull()
}

// Adds PAD_CFG_GPI macro with arguments
func (platform PlatformSpecific) GpiMacroAdd() {
	platform.InheritanceMacro.GpiMacroAdd()
}

// Adds PAD_CFG_GPO macro with arguments
func (platform PlatformSpecific) GpoMacroAdd() {
	platform.InheritanceMacro.GpoMacroAdd()
}

// Adds PAD_CFG_NF macro with arguments
func (platform PlatformSpecific) NativeFunctionMacroAdd() {
	platform.InheritanceMacro.NativeFunctionMacroAdd()
}

// Adds PAD_NC macro
func (platform PlatformSpecific) NoConnMacroAdd() {
	platform.InheritanceMacro.NoConnMacroAdd()
}

// GenMacro - generate pad macro
// dw0 : DW0 config register value
// dw1 : DW1 config register value
// return: string of macro
func (PlatformSpecific) GenMacro(id string, dw0Val, dw1Val uint32, ownership uint8) string {
	macro := common.GetInstanceMacro(
		PlatformSpecific{
			InheritanceMacro: cnl.PlatformSpecific{
				InheritanceMacro: snr.PlatformSpecific{},
			},
		},
		fields.InterfaceGet(),
	)
	macro.Clear()

	macro.Clear()
	dw0 := macro.GetRegisterDW0()
	dw0.CntrMaskFieldsClear(bits.All32)

	dw1 := macro.GetRegisterDW1()
	dw1.CntrMaskFieldsClear(bits.All32)

	dw0.Value = dw0Val
	dw1.Value = dw1Val
	dw0.ReadOnly = PAD_CFG_DW0_RO_FIELDS
	dw1.ReadOnly = PAD_CFG_DW1_RO_FIELDS

	macro.PadIdSet(id).SetPadOwnership(ownership)
	return macro.Generate()
}
