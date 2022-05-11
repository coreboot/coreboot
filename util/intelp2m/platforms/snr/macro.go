package snr

import (
	"strings"
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/config"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields"
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

type PlatformSpecific struct {}

// RemmapRstSrc - remmap Pad Reset Source Config
func (PlatformSpecific) RemmapRstSrc() {
	macro := common.GetMacro()
	if config.TemplateGet() != config.TempInteltool {
		// Use reset source remapping only if the input file is inteltool.log dump
		return
	}
	if strings.Contains(macro.PadIdGet(), "GPD") {
		// See reset map for the Sunrise GPD Group in the Community 2:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/skylake/gpio.c#L15
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
	if dw0.GetRXLevelEdgeConfiguration() == common.TRIG_LEVEL {
		if dw0.GetRxInvert() != 0 {
			// PAD_CFG_GPI_APIC_LOW(pad, pull, rst)
			macro.Add("_LOW")
		} else {
			// PAD_CFG_GPI_APIC_HIGH(pad, pull, rst)
			macro.Add("_HIGH")
		}
		macro.Add("(").Id().Pull().Rstsrc().Add("),")
		return true
	}

	// e.g. PAD_CFG_GPI_APIC_IOS(pad, pull, rst, trig, inv, iosstate, iosterm)
	macro.Add("_IOS(").Id().Pull().Rstsrc().Trig().Invert().Add(", TxLASTRxE, SAME),")
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
	// PAD_CFG_GPI_SCI(GPP_B18, UP_20K, PLTRST, LEVEL, INVERT),
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
	// PAD_CFG_GPI_SMI(GPP_E7, NONE, DEEP, LEVEL, NONE),
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
func (PlatformSpecific) GpoMacroAdd() {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	term := macro.Register(PAD_CFG_DW1).GetTermination()

	// #define PAD_CFG_GPO(pad, val, rst)                \
	//    _PAD_CFG_STRUCT(pad,                           \
	//      PAD_FUNC(GPIO) | PAD_RESET(rst) |            \
	//      PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val, \
	//      PAD_PULL(NONE) | PAD_IOSSTATE(TxLASTRxE))
	if dw0.GetRXLevelEdgeConfiguration() != common.TRIG_OFF {
		dw0.CntrMaskFieldsClear(common.RxLevelEdgeConfigurationMask)
	}
	macro.Set("PAD_CFG")
	if macro.IsOwnershipDriver() {
		// PAD_CFG_GPO_GPIO_DRIVER(pad, val, rst, pull)
		macro.Add("_GPO_GPIO_DRIVER").Add("(").Id().Val().Rstsrc().Pull().Add("),")
		return
	}
	if term != 0 {
		// e.g. PAD_CFG_TERM_GPO(GPP_B23, 1, DN_20K, DEEP),
		macro.Add("_TERM")
	}
	macro.Add("_GPO").Add("(").Id().Val()
	if term != 0 {
		macro.Pull()
	}
	macro.Rstsrc().Add("),")
}

// Adds PAD_CFG_NF macro with arguments
func (PlatformSpecific) NativeFunctionMacroAdd() {
	macro := common.GetMacro()
	// e.g. PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1)
	macro.Set("PAD_CFG_NF")
	if macro.Register(PAD_CFG_DW1).GetPadTol() != 0 {
		macro.Add("_1V8")
	}
	macro.Add("(").Id().Pull().Rstsrc().Padfn().Add("),")
}

// Adds PAD_NC macro
func (PlatformSpecific) NoConnMacroAdd() {
	macro := common.GetMacro()
	// #define PAD_NC(pad, pull)
	// _PAD_CFG_STRUCT(pad,
	//     PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE),
	//     PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE)),
	dw0 := macro.Register(PAD_CFG_DW0)

	// Some fields of the configuration registers are hidden inside the macros,
	// we should check them to update the corresponding bits in the control mask.
	if dw0.GetRXLevelEdgeConfiguration() != common.TRIG_OFF {
		dw0.CntrMaskFieldsClear(common.RxLevelEdgeConfigurationMask)
	}
	if dw0.GetResetConfig() != 1 { // 1 = RST_DEEP
		dw0.CntrMaskFieldsClear(common.PadRstCfgMask)
	}

	macro.Set("PAD_NC").Add("(").Id().Pull().Add("),")
}

// GenMacro - generate pad macro
// dw0 : DW0 config register value
// dw1 : DW1 config register value
// return: string of macro
//         error
func (PlatformSpecific) GenMacro(id string, dw0 uint32, dw1 uint32, ownership uint8) string {
	macro := common.GetInstanceMacro(PlatformSpecific{}, fields.InterfaceGet())
	macro.Clear()
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.PadIdSet(id).SetPadOwnership(ownership)
	macro.Register(PAD_CFG_DW0).ValueSet(dw0).ReadOnlyFieldsSet(PAD_CFG_DW0_RO_FIELDS)
	macro.Register(PAD_CFG_DW1).ValueSet(dw1).ReadOnlyFieldsSet(PAD_CFG_DW1_RO_FIELDS)
	return macro.Generate()
}
