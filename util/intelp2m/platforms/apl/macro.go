package apl

import (
	"fmt"
	"strconv"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/config"
	"review.coreboot.org/coreboot.git/util/intelp2m/fields"
)

const (
	PAD_CFG_DW0_RO_FIELDS = (0x1 << 27) | (0x1 << 24) | (0x3 << 21) | (0xf << 16) | 0xfc
	PAD_CFG_DW1_RO_FIELDS = 0xfffc00ff
)

const (
	PAD_CFG_DW0 = common.PAD_CFG_DW0
	PAD_CFG_DW1 = common.PAD_CFG_DW1
	MAX_DW_NUM  = common.MAX_DW_NUM
)

const (
	PULL_NONE    = 0x0  // 0 000: none
	PULL_DN_5K   = 0x2  // 0 010: 5k wpd (Only available on SMBus GPIOs)
	PULL_DN_20K  = 0x4  // 0 100: 20k wpd
	// PULL_NONE = 0x8  // 1 000: none
	PULL_UP_1K   = 0x9  // 1 001: 1k wpu (Only available on I2C GPIOs)
	PULL_UP_2K   = 0xb  // 1 011: 2k wpu (Only available on I2C GPIOs)
	PULL_UP_20K  = 0xc  // 1 100: 20k wpu
	PULL_UP_667  = 0xd  // 1 101: 1k & 2k wpu (Only available on I2C GPIOs)
	PULL_NATIVE  = 0xf  // 1 111: (optional) Native controller selected by Pad Mode
)

type PlatformSpecific struct {}

// RemmapRstSrc - remmap Pad Reset Source Config
// remmap is not required because it is the same as common.
func (PlatformSpecific) RemmapRstSrc() {}

// Adds The Pad Termination (TERM) parameter from DW1 to the macro as a new argument
// return: macro
func (PlatformSpecific) Pull() {
	macro := common.GetMacro()
	dw1 := macro.Register(PAD_CFG_DW1)
	var pull = map[uint8]string{
		PULL_NONE:   "NONE",
		PULL_DN_5K:  "DN_5K",
		PULL_DN_20K: "DN_20K",
		PULL_UP_1K:  "UP_1K",
		PULL_UP_2K:  "UP_2K",
		PULL_UP_20K: "UP_20K",
		PULL_UP_667: "UP_667",
		PULL_NATIVE: "NATIVE",

	}
	terminationFieldValue := dw1.GetTermination()
	str, valid := pull[terminationFieldValue]
	if !valid {
		str = strconv.Itoa(int(terminationFieldValue))
		fmt.Println("Error", macro.PadIdGet(), " invalid TERM value = ", str)
	}
	macro.Separator().Add(str)
}

// Generate macro to cause peripheral IRQ when configured in GPIO input mode
func ioApicRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	dw1 := macro.Register(PAD_CFG_DW1)
	if dw0.GetGPIOInputRouteIOxAPIC() == 0 {
		return false
	}
	macro.Add("_APIC")
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// e.g. H1_PCH_INT_ODL
		// PAD_CFG_GPI_APIC_IOS(GPIO_63, NONE, DEEP, LEVEL, INVERT, TxDRxE, DISPUPD),
		macro.Add("_IOS(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else {
		// PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv)
		macro.Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	}
	macro.Add("),")
	return true
}

// Generate macro to cause NMI when configured in GPIO input mode
func nmiRoute() bool {
	macro := common.GetMacro()
	if macro.Register(PAD_CFG_DW0).GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// e.g. PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	macro.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SCI when configured in GPIO input mode
func sciRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	dw1 := macro.Register(PAD_CFG_DW0)
	if dw0.GetGPIOInputRouteSCI() == 0 {
		return false
	}
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPI_SCI_IOS(GPIO_141, NONE, DEEP, EDGE_SINGLE, INVERT, IGNORE, DISPUPD),
		macro.Add("_SCI_IOS")
		macro.Add("(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else if dw0.GetRXLevelEdgeConfiguration() & 0x1 != 0 {
		// e.g. PAD_CFG_GPI_ACPI_SCI(GPP_G2, NONE, DEEP, YES),
		macro.Add("_ACPI_SCI").Add("(").Id().Pull().Rstsrc().Invert()
	} else {
		// e.g. PAD_CFG_GPI_SCI(GPP_B18, UP_20K, PLTRST, LEVEL, INVERT),
		macro.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert()
	}
	macro.Add("),")
	return true
}

// Generate macro to cause SMI when configured in GPIO input mode
func smiRoute() bool {
	macro := common.GetMacro()
	dw0 := macro.Register(PAD_CFG_DW0)
	dw1 := macro.Register(PAD_CFG_DW1)
	if dw0.GetGPIOInputRouteSMI() == 0 {
		return false
	}
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPI_SMI_IOS(GPIO_41, UP_20K, DEEP, EDGE_SINGLE, NONE, IGNORE, SAME),
		macro.Add("_SMI_IOS")
		macro.Add("(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else if dw0.GetRXLevelEdgeConfiguration() & 0x1 != 0 {
		// e.g. PAD_CFG_GPI_ACPI_SMI(GPP_I3, NONE, DEEP, YES),
		macro.Add("_ACPI_SMI").Add("(").Id().Pull().Rstsrc().Invert()
	} else {
		// e.g. PAD_CFG_GPI_SMI(GPP_E3, NONE, PLTRST, EDGE_SINGLE, NONE),
		macro.Add("_SMI").Add("(").Id().Pull().Rstsrc().Trig().Invert()
	}
	macro.Add("),")
	return true
}

// Generate macro for GPI port
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
		dw1 := macro.Register(PAD_CFG_DW1)
		isIOStandbyStateUsed := dw1.GetIOStandbyState() != 0
		isIOStandbyTerminationUsed := dw1.GetIOStandbyTermination() != 0
		if isIOStandbyStateUsed && !isIOStandbyTerminationUsed {
			macro.Add("_TRIG_IOSSTATE_OWN(")
			// PAD_CFG_GPI_TRIG_IOSSTATE_OWN(pad, pull, rst, trig, iosstate, own)
			macro.Id().Pull().Rstsrc().Trig().IOSstate().Own().Add("),")
		} else if isIOStandbyTerminationUsed {
			macro.Add("_TRIG_IOS_OWN(")
			// PAD_CFG_GPI_TRIG_IOS_OWN(pad, pull, rst, trig, iosstate, iosterm, own)
			macro.Id().Pull().Rstsrc().Trig().IOSstate().IOTerm().Own().Add("),")
		} else {
			// PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, own)
			macro.Add("_TRIG_OWN(").Id().Pull().Rstsrc().Trig().Own().Add("),")
		}
	case 1:
		// GPI with IRQ route
		if config.AreFieldsIgnored() {
			macro.SetPadOwnership(common.PAD_OWN_ACPI)
		}
	case 2:
		// PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)
		macro.Set("PAD_CFG_GPI_DUAL_ROUTE(").Id().Pull().Rstsrc().Trig().Invert()
		macro.Add(", " + ids[0] + ", " + ids[1] + "),")
		if config.AreFieldsIgnored() {
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
	dw0 :=  macro.Register(PAD_CFG_DW0)
	dw1 :=  macro.Register(PAD_CFG_DW1)
	term := dw1.GetTermination()

	macro.Set("PAD_CFG")
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_91, 0, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
		// PAD_CFG_GPO_IOSSTATE_IOSTERM(pad, val, rst, pull, iosstate, ioterm)
		macro.Add("_GPO_IOSSTATE_IOSTERM(").Id().Val().Rstsrc().Pull().IOSstate().IOTerm()
	} else {
		if term != 0 {
			// e.g. PAD_CFG_TERM_GPO(GPP_B23, 1, DN_20K, DEEP),
			// PAD_CFG_TERM_GPO(pad, val, pull, rst)
			macro.Add("_TERM")
		}
		macro.Add("_GPO(").Id().Val()
		if term != 0 {
			macro.Pull()
		}
		macro.Rstsrc()
	}
	macro.Add("),")

	if dw0.GetRXLevelEdgeConfiguration() != common.TRIG_OFF {
		// ignore if trig = OFF is not set
		dw0.CntrMaskFieldsClear(common.RxLevelEdgeConfigurationMask)
	}
}

// Adds PAD_CFG_NF macro with arguments
func (PlatformSpecific) NativeFunctionMacroAdd() {
	macro := common.GetMacro()
	dw1 := macro.Register(PAD_CFG_DW1)
	isIOStandbyStateUsed := dw1.GetIOStandbyState() != 0
	isIOStandbyTerminationUsed := dw1.GetIOStandbyTermination() != 0

	macro.Set("PAD_CFG_NF")
	if !isIOStandbyTerminationUsed && isIOStandbyStateUsed {
		if dw1.GetIOStandbyState() == common.StandbyIgnore {
			// PAD_CFG_NF_IOSTANDBY_IGNORE(PMU_SLP_S0_B, NONE, DEEP, NF1),
			macro.Add("_IOSTANDBY_IGNORE(").Id().Pull().Rstsrc().Padfn()
		} else {
			// PAD_CFG_NF_IOSSTATE(GPIO_22, UP_20K, DEEP, NF2, TxDRxE),
			macro.Add("_IOSSTATE(").Id().Pull().Rstsrc().Padfn().IOSstate()
		}
	} else if isIOStandbyTerminationUsed {
		// PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_103, NATIVE, DEEP, NF1, MASK, SAME),
		macro.Add("_IOSSTATE_IOSTERM(").Id().Pull().Rstsrc().Padfn().IOSstate().IOTerm()
	} else {
		// e.g. PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1)
		macro.Add("(").Id().Pull().Rstsrc().Padfn()
	}
	macro.Add("),")

	if dw0 := macro.Register(PAD_CFG_DW0); dw0.GetGPIORxTxDisableStatus() != 0 {
		// Since the bufbis parameter will be ignored for NF, we should clear
		// the corresponding bits in the control mask.
		dw0.CntrMaskFieldsClear(common.RxTxBufDisableMask)
	}
}

// Adds PAD_NC macro
func (PlatformSpecific) NoConnMacroAdd() {
	macro := common.GetMacro()
	dw1 := macro.Register(PAD_CFG_DW1)

	if dw1.GetIOStandbyState() == common.TxDRxE {
		dw0 := macro.Register(PAD_CFG_DW0)

		// See comments in sunrise/macro.go : NoConnMacroAdd()
		if dw0.GetRXLevelEdgeConfiguration() != common.TRIG_OFF {
			dw0.CntrMaskFieldsClear(common.RxLevelEdgeConfigurationMask)
		}
		if dw0.GetResetConfig() != 1 { // 1 = RST_DEEP
			dw0.CntrMaskFieldsClear(common.PadRstCfgMask)
		}

		// PAD_NC(OSC_CLK_OUT_1, DN_20K)
		macro.Set("PAD_NC").Add("(").Id().Pull().Add("),")
		return
	}
	// PAD_CFG_GPIO_HI_Z(GPIO_81, UP_20K, DEEP, HIZCRx0, DISPUPD),
	macro.Set("PAD_CFG_GPIO_")
	if macro.IsOwnershipDriver() {
		// PAD_CFG_GPIO_DRIVER_HI_Z(GPIO_55, UP_20K, DEEP, HIZCRx1, ENPU),
		macro.Add("DRIVER_")
	}
	macro.Add("HI_Z(").Id().Pull().Rstsrc().IOSstate().IOTerm().Add("),")
}

// GenMacro - generate pad macro
// dw0 : DW0 config register value
// dw1 : DW1 config register value
// return: string of macro
//         error
func (PlatformSpecific) GenMacro(id string, dw0 uint32, dw1 uint32, ownership uint8) string {
	macro := common.GetInstanceMacro(PlatformSpecific{}, fields.InterfaceGet())
	// use platform-specific interface in Macro struct
	macro.PadIdSet(id).SetPadOwnership(ownership)
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.Register(PAD_CFG_DW0).CntrMaskFieldsClear(common.AllFields)
	macro.Register(PAD_CFG_DW0).ValueSet(dw0).ReadOnlyFieldsSet(PAD_CFG_DW0_RO_FIELDS)
	macro.Register(PAD_CFG_DW1).ValueSet(dw1).ReadOnlyFieldsSet(PAD_CFG_DW1_RO_FIELDS)
	return macro.Generate()
}
