package cnl

import (
	"strings"
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/config"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

const (
	PAD_CFG_DW0_RO_FIELDS = (0x1 << 27) | (0x1 << 24) | (0x3 << 21) | (0xf << 16) | 0xfc
	PAD_CFG_DW1_RO_FIELDS = 0xfdffc3ff
)

const (
	PAD_CFG_DW0 = common.PAD_CFG_DW0
	PAD_CFG_DW1 = common.PAD_CFG_DW1
	MAX_DW_NUM  = common.MAX_DW_NUM
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
	if config.TemplateGet() != config.TempInteltool {
		// Use reset source remapping only if the input file is inteltool.log dump
		return
	}
	if strings.Contains(macro.PadIdGet(), "GPP_A") ||
			strings.Contains(macro.PadIdGet(), "GPP_B") ||
			strings.Contains(macro.PadIdGet(), "GPP_G") {
		// See reset map for the Cannonlake Groups the Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/cannonlake/gpio.c#L14
		// remmap is not required because it is the same as common.
		return
	}

	dw0 := macro.Register(PAD_CFG_DW0)
	var remapping = map[uint8]uint32{
		0: common.RST_RSMRST << common.PadRstCfgShift,
		1: common.RST_DEEP   << common.PadRstCfgShift,
		2: common.RST_PLTRST << common.PadRstCfgShift,
	}
	resetsrc, valid := remapping[dw0.GetResetConfig()]
	if valid {
		// dw0.SetResetConfig(resetsrc)
		ResetConfigFieldVal := (dw0.ValueGet() & 0x3fffffff) | remapping[dw0.GetResetConfig()]
		dw0.ValueSet(ResetConfigFieldVal)
	} else {
		fmt.Println("Invalid Pad Reset Config [ 0x", resetsrc ," ] for ", macro.PadIdGet())
	}
	dw0.CntrMaskFieldsClear(common.PadRstCfgMask)
}

// Adds The Pad Termination (TERM) parameter from PAD_CFG_DW1 to the macro
// as a new argument
func (PlatformSpecific) Pull() {
	macro := common.GetMacro()
	dw1 := macro.Register(PAD_CFG_DW1)
	var pull = map[uint8]string{
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
	dw0 := macro.Register(PAD_CFG_DW0)
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
	if macro.Register(PAD_CFG_DW0).GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	macro.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SCI when configured in GPIO input mode
func sciRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	if dw0.GetGPIOInputRouteSCI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SCI(pad, pull, rst, trig, inv)
	macro.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SMI when configured in GPIO input mode
func smiRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	if dw0.GetGPIOInputRouteSMI() == 0 {
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
	for routeid, isRoute := range map[string]func() (bool) {
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
		if config.AreFieldsIgnored() {
			// Set Host Software Ownership to ACPI mode
			macro.SetPadOwnership(common.PAD_OWN_ACPI)
		}

	case 2:
		// PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)
		macro.Set("PAD_CFG_GPI_DUAL_ROUTE(").Id().Pull().Rstsrc().Trig().Invert()
		macro.Add(", " + ids[0] + ", " + ids[1] + "),")
		if config.AreFieldsIgnored() {
			// Set Host Software Ownership to ACPI mode
			macro.SetPadOwnership(common.PAD_OWN_ACPI)
		}
	default:
		// Clear the control mask so that the check fails and "Advanced" macro is
		// generated
		macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
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
// dw0 : DW0 config register value
// dw1 : DW1 config register value
// return: string of macro
//         error
func (PlatformSpecific) GenMacro(id string, dw0 uint32, dw1 uint32, ownership uint8) string {
	macro := common.GetInstanceMacro(PlatformSpecific{InheritanceMacro : snr.PlatformSpecific{}},
		fields.InterfaceGet())
	macro.Clear()
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.PadIdSet(id).SetPadOwnership(ownership)
	macro.Register(PAD_CFG_DW0).ValueSet(dw0).ReadOnlyFieldsSet(PAD_CFG_DW0_RO_FIELDS)
	macro.Register(PAD_CFG_DW1).ValueSet(dw1).ReadOnlyFieldsSet(PAD_CFG_DW1_RO_FIELDS)
	return macro.Generate()
}
