package cnl

import (
	"fmt"
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

const (
	PAD_CFG_DW0_RO_FIELDS = (0x1 << 27) | (0x1 << 24) | (0x3 << 21) | (0xf << 16) | 0xfc
	PAD_CFG_DW1_RO_FIELDS = 0xfdffc3ff
)

type InheritanceMacro interface {
	GpoMacroAdd()
	NativeFunctionMacroAdd()
	NoConnMacroAdd()
}

type PlatformSpecific struct {
	InheritanceMacro
	InheritanceTemplate
}

// RemmapRstSrc - remmap Pad Reset Source Config
func (PlatformSpecific) RemmapRstSrc() {
	macro := common.GetMacro()
	if strings.Contains(macro.PadIdGet(), "GPP_A") ||
		strings.Contains(macro.PadIdGet(), "GPP_B") ||
		strings.Contains(macro.PadIdGet(), "GPP_G") {
		// See reset map for the Cannonlake Groups the Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/cannonlake/gpio.c#L14
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
func (PlatformSpecific) Pull() {
	macro := common.GetMacro()
	dw1 := macro.GetRegisterDW1()
	var pull = map[uint32]string{
		0x0: "NONE",
		0x2: "DN_5K",
		0x4: "DN_20K",
		0x9: "UP_1K",
		0xa: "UP_5K",
		0xb: "UP_2K",
		0xc: "UP_20K",
		0xd: "UP_667",
		0xf: "NATIVE",
	}
	str, valid := pull[dw1.GetTermination()]
	if !valid {
		str = "INVALID"
		fmt.Println("Error",
			macro.PadIdGet(),
			" invalid TERM value = ",
			int(dw1.GetTermination()))
	}
	macro.Separator().Add(str)
}

// Generate macro to cause peripheral IRQ when configured in GPIO input mode
func ioApicRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.GetRegisterDW0()
	if dw0.GetGPIOInputRouteIOxAPIC() == 0 {
		return false
	}

	macro.Add("_APIC")
	// PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv)
	macro.Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause NMI when configured in GPIO input mode
func nmiRoute() bool {
	macro := common.GetMacro()
	if macro.GetRegisterDW0().GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	macro.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SCI when configured in GPIO input mode
func sciRoute() bool {
	macro := common.GetMacro()
	if macro.GetRegisterDW0().GetGPIOInputRouteSCI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SCI(pad, pull, rst, trig, inv)
	macro.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SMI when configured in GPIO input mode
func smiRoute() bool {
	macro := common.GetMacro()

	if macro.GetRegisterDW0().GetGPIOInputRouteSMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SMI(pad, pull, rst, trig, inv)
	macro.Add("_SMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Adds PAD_CFG_GPI macro with arguments
func (PlatformSpecific) GpiMacroAdd() {
	macro := common.GetMacro()
	var ids []string
	macro.Set("PAD_CFG_GPI")
	for routeid, isRoute := range map[string]func() bool{
		"IOAPIC": ioApicRoute,
		"SCI":    sciRoute,
		"SMI":    smiRoute,
		"NMI":    nmiRoute,
	} {
		if isRoute() {
			ids = append(ids, routeid)
		}
	}

	switch argc := len(ids); argc {
	case 0:
		// e.g. PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, own)
		macro.Add("_TRIG_OWN").Add("(").Id().Pull().Rstsrc().Trig().Own().Add("),")
	case 1:
		// GPI with IRQ route
		if p2m.Config.IgnoredFields {
			// Set Host Software Ownership to ACPI mode
			macro.SetPadOwnership(common.PAD_OWN_ACPI)
		}

	case 2:
		// PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)
		macro.Set("PAD_CFG_GPI_DUAL_ROUTE(").Id().Pull().Rstsrc().Trig().Invert()
		macro.Add(", " + ids[0] + ", " + ids[1] + "),")
		if p2m.Config.IgnoredFields {
			// Set Host Software Ownership to ACPI mode
			macro.SetPadOwnership(common.PAD_OWN_ACPI)
		}
	default:
		// Clear the control mask so that the check fails and "Advanced" macro is
		// generated
		macro.GetRegisterDW0().CntrMaskFieldsClear(bits.All32)
	}
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
// dw0Val : DW0 config register value
// dw1Val : DW1 config register value
// return: string of macro
func (PlatformSpecific) GenMacro(id string, dw0Val uint32, dw1Val uint32, ownership uint8) string {
	macro := common.GetInstanceMacro(
		PlatformSpecific{
			InheritanceMacro: snr.PlatformSpecific{},
		},
		fields.InterfaceGet(),
	)
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
