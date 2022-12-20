package apl

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

const (
	DW0Mask uint32 = (0b1 << 27) | (0b1 << 24) | (0b11 << 21) | (0b1111 << 16) | 0b11111100
	DW1Mask uint32 = 0b11111111111111000000000011111111
)

type BasePlatform struct {
	common.BasePlatform
}

func InitBasePlatform(dw0, dw0mask uint32, dw1, dw1mask uint32) BasePlatform {
	return BasePlatform{common.InitBasePlatform(dw0, dw0mask, dw1, dw1mask)}
}

func GetPlatform(dw0, dw1 uint32) common.PlatformIf {
	p := InitBasePlatform(dw0, DW0Mask, dw1, DW1Mask)
	return &p
}

// RemapRstSrc() remaps Pad Reset Source Config
func (p *BasePlatform) RemapRstSrc(m *common.Macro) {}

// Pull() adds The Pad Termination (TERM) parameter from PAD_CFG_DW1 to the macro
func (p *BasePlatform) Pull(m *common.Macro) {
	const (
		PULL_NONE   = 0b0000 // 0 000: none
		PULL_DN_5K  = 0b0010 // 0 010: 5k wpd (Only available on SMBus GPIOs)
		PULL_DN_20K = 0b0100 // 0 100: 20k wpd
		// PULL_NONE = 0b1000  // 1 000: none
		PULL_UP_1K  = 0b1001 // 1 001: 1k wpu (Only available on I2C GPIOs)
		PULL_UP_2K  = 0b1011 // 1 011: 2k wpu (Only available on I2C GPIOs)
		PULL_UP_20K = 0b1100 // 1 100: 20k wpu
		PULL_UP_667 = 0b1101 // 1 101: 1k & 2k wpu (Only available on I2C GPIOs)
		PULL_NATIVE = 0b1111 // 1 111: (optional) Native controller selected by Pad Mode
	)

	var pull = map[uint32]string{
		PULL_NONE:   "NONE",
		PULL_DN_5K:  "DN_5K",
		PULL_DN_20K: "DN_20K",
		PULL_UP_1K:  "UP_1K",
		PULL_UP_2K:  "UP_2K",
		PULL_UP_20K: "UP_20K",
		PULL_UP_667: "UP_667",
		PULL_NATIVE: "NATIVE",
	}
	dw1 := p.GetRegisterDW1()
	term, exist := pull[dw1.GetTermination()]
	if !exist {
		term = "INVALID"
		logs.Errorf("%s: DW1 %s: invalid termination value 0b%b",
			dw1, m.GetPadId(), dw1.GetTermination())
	}
	m.Separator().Add(term)
}

// Generate macro to cause peripheral IRQ when configured in GPIO input mode
func ioApicRoute(p *BasePlatform, m *common.Macro) bool {
	if dw0 := p.GetRegisterDW0(); dw0.GetGPIOInputRouteIOxAPIC() == 0 {
		return false
	}
	m.Add("_APIC")
	dw1 := p.GetRegisterDW1()
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// e.g. H1_PCH_INT_ODL
		// PAD_CFG_GPI_APIC_IOS(GPIO_63, NONE, DEEP, LEVEL, INVERT, TxDRxE, DISPUPD),
		m.Add("_IOS(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else {
		// PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv)
		m.Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	}
	m.Add("),")
	return true
}

// Generate macro to cause NMI when configured in GPIO input mode
func nmiRoute(p *BasePlatform, m *common.Macro) bool {
	if dw0 := p.GetRegisterDW0(); dw0.GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// e.g. PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	m.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Generate macro to cause SCI when configured in GPIO input mode
func sciRoute(p *BasePlatform, m *common.Macro) bool {
	dw0 := p.GetRegisterDW0()
	if dw0.GetGPIOInputRouteSCI() == 0 {
		return false
	}

	dw1 := p.GetRegisterDW1()
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPI_SCI_IOS(GPIO_141, NONE, DEEP, EDGE_SINGLE, INVERT, IGNORE, DISPUPD),
		m.Add("_SCI_IOS")
		m.Add("(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else if dw0.GetRXLevelEdgeConfiguration()&0x1 != 0 {
		// e.g. PAD_CFG_GPI_ACPI_SCI(GPP_G2, NONE, DEEP, YES),
		m.Add("_ACPI_SCI").Add("(").Id().Pull().Rstsrc().Invert()
	} else {
		// e.g. PAD_CFG_GPI_SCI(GPP_B18, UP_20K, PLTRST, LEVEL, INVERT),
		m.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert()
	}
	m.Add("),")
	return true
}

// Generate macro to cause SMI when configured in GPIO input mode
func smiRoute(p *BasePlatform, m *common.Macro) bool {
	dw0 := p.GetRegisterDW0()
	if dw0.GetGPIOInputRouteSMI() == 0 {
		return false
	}
	dw1 := p.GetRegisterDW1()
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPI_SMI_IOS(GPIO_41, UP_20K, DEEP, EDGE_SINGLE, NONE, IGNORE, SAME),
		m.Add("_SMI_IOS")
		m.Add("(").Id().Pull().Rstsrc().Trig().Invert().IOSstate().IOTerm()
	} else if dw0.GetRXLevelEdgeConfiguration()&0x1 != 0 {
		// e.g. PAD_CFG_GPI_ACPI_SMI(GPP_I3, NONE, DEEP, YES),
		m.Add("_ACPI_SMI").Add("(").Id().Pull().Rstsrc().Invert()
	} else {
		// e.g. PAD_CFG_GPI_SMI(GPP_E3, NONE, PLTRST, EDGE_SINGLE, NONE),
		m.Add("_SMI").Add("(").Id().Pull().Rstsrc().Trig().Invert()
	}
	m.Add("),")
	return true
}

// AddGpiMacro() adds PAD_CFG_GPI macro with arguments
func (p *BasePlatform) AddGpiMacro(m *common.Macro) {
	var ids []string

	m.Set("PAD_CFG_GPI")
	for routeid, isRoute := range map[string]func(*BasePlatform, *common.Macro) bool{
		"IOAPIC": ioApicRoute,
		"SCI":    sciRoute,
		"SMI":    smiRoute,
		"NMI":    nmiRoute,
	} {
		if isRoute(p, m) {
			ids = append(ids, routeid)
		}
	}

	switch config, argc := p2m.Config, len(ids); argc {
	case 0:
		dw1 := p.GetRegisterDW1()
		isIOStandbyStateUsed := dw1.GetIOStandbyState() != 0
		isIOStandbyTerminationUsed := dw1.GetIOStandbyTermination() != 0
		if isIOStandbyStateUsed && !isIOStandbyTerminationUsed {
			m.Add("_TRIG_IOSSTATE_OWN(")
			// PAD_CFG_GPI_TRIG_IOSSTATE_OWN(pad, pull, rst, trig, iosstate, own)
			m.Id().Pull().Rstsrc().Trig().IOSstate().Own().Add("),")
		} else if isIOStandbyTerminationUsed {
			m.Add("_TRIG_IOS_OWN(")
			// PAD_CFG_GPI_TRIG_IOS_OWN(pad, pull, rst, trig, iosstate, iosterm, own)
			m.Id().Pull().Rstsrc().Trig().IOSstate().IOTerm().Own().Add("),")
		} else {
			// PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, own)
			m.Add("_TRIG_OWN(").Id().Pull().Rstsrc().Trig().Own().Add("),")
		}
	case 1:
		// GPI with IRQ route
		if config.IgnoredFields {
			m.SetOwnershipAcpi()
		}
	case 2:
		// PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)
		m.Set("PAD_CFG_GPI_DUAL_ROUTE(").Id().Pull().Rstsrc().Trig().Invert()
		m.Add(", " + ids[0] + ", " + ids[1] + "),")
		if config.IgnoredFields {
			m.SetOwnershipAcpi()
		}
	default:
		// Clear the control mask so that the check fails and "Advanced" macro is
		// generated
		dw0 := p.GetRegisterDW0()
		dw0.CntrMaskFieldsClear(bits.All32)
	}
}

// AddGpoMacro() adds PAD_CFG_GPO macro with arguments
func (p *BasePlatform) AddGpoMacro(m *common.Macro) {
	m.Set("PAD_CFG")
	dw1 := p.GetRegisterDW1()
	if dw1.GetIOStandbyState() != 0 || dw1.GetIOStandbyTermination() != 0 {
		// PAD_CFG_GPO_IOSSTATE_IOSTERM(GPIO_91, 0, DEEP, NONE, Tx0RxDCRx0, DISPUPD),
		// PAD_CFG_GPO_IOSSTATE_IOSTERM(pad, val, rst, pull, iosstate, ioterm)
		m.Add("_GPO_IOSSTATE_IOSTERM(").Id().Val().Rstsrc().Pull().IOSstate().IOTerm()
	} else {
		term := dw1.GetTermination()
		if term != 0 {
			// e.g. PAD_CFG_TERM_GPO(GPP_B23, 1, DN_20K, DEEP),
			// PAD_CFG_TERM_GPO(pad, val, pull, rst)
			m.Add("_TERM")
		}
		m.Add("_GPO(").Id().Val()
		if term != 0 {
			m.Pull()
		}
		m.Rstsrc()
	}
	m.Add("),")

	if dw0 := p.GetRegisterDW0(); dw0.GetRXLevelEdgeConfiguration() != bits.TrigOFF {
		// ignore if trig = OFF is not set
		dw0.CntrMaskFieldsClear(bits.DW0[bits.DW0RxLevelEdgeConfiguration])
	}
}

// AddNativeFunctionMacro() adds PAD_CFG_NF macro with arguments
func (p *BasePlatform) AddNativeFunctionMacro(m *common.Macro) {
	m.Set("PAD_CFG_NF")
	dw1 := p.GetRegisterDW1()
	isIOStandbyStateUsed := dw1.GetIOStandbyState() != 0
	isIOStandbyTerminationUsed := dw1.GetIOStandbyTermination() != 0
	if !isIOStandbyTerminationUsed && isIOStandbyStateUsed {
		if dw1.GetIOStandbyState() == bits.IOStateStandbyIgnore {
			// PAD_CFG_NF_IOSTANDBY_IGNORE(PMU_SLP_S0_B, NONE, DEEP, NF1),
			m.Add("_IOSTANDBY_IGNORE(").Id().Pull().Rstsrc().Padfn()
		} else {
			// PAD_CFG_NF_IOSSTATE(GPIO_22, UP_20K, DEEP, NF2, TxDRxE),
			m.Add("_IOSSTATE(").Id().Pull().Rstsrc().Padfn().IOSstate()
		}
	} else if isIOStandbyTerminationUsed {
		// PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_103, NATIVE, DEEP, NF1, MASK, SAME),
		m.Add("_IOSSTATE_IOSTERM(").Id().Pull().Rstsrc().Padfn().IOSstate().IOTerm()
	} else {
		// e.g. PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1)
		m.Add("(").Id().Pull().Rstsrc().Padfn()
	}
	m.Add("),")

	if dw0 := p.GetRegisterDW0(); dw0.GetGPIORxTxDisableStatus() != 0 {
		// Since the bufbis parameter will be ignored for NF, we should clear
		// the corresponding bits in the control mask.
		dw0.CntrMaskFieldsClear(bits.DW0[bits.DW0RxTxBufDisable])
	}
}

// AddNoConnMacro() adds PAD_NC macro
func (p *BasePlatform) AddNoConnMacro(m *common.Macro) {
	if dw1 := p.GetRegisterDW1(); dw1.GetIOStandbyState() == bits.IOStateTxDRxE {
		dw0 := p.GetRegisterDW0()
		// See comments in sunrise/m.go : AddNoConnMacro()
		if dw0.GetRXLevelEdgeConfiguration() != bits.TrigOFF {
			dw0.CntrMaskFieldsClear(bits.DW0[bits.DW0RxLevelEdgeConfiguration])
		}
		if dw0.GetResetConfig() != 1 { // 1 = RST_DEEP
			dw0.CntrMaskFieldsClear(bits.DW0[bits.DW0PadRstCfg])
		}
		// PAD_NC(OSC_CLK_OUT_1, DN_20K)
		m.Set("PAD_NC").Add("(").Id().Pull().Add("),")
		return
	}
	// PAD_CFG_GPIO_HI_Z(GPIO_81, UP_20K, DEEP, HIZCRx0, DISPUPD),
	if m.Set("PAD_CFG_GPIO_"); m.IsOwnershipDriver() {
		// PAD_CFG_GPIO_DRIVER_HI_Z(GPIO_55, UP_20K, DEEP, HIZCRx1, ENPU),
		m.Add("DRIVER_")
	}
	m.Add("HI_Z(").Id().Pull().Rstsrc().IOSstate().IOTerm().Add("),")
}
