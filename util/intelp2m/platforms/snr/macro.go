package snr

import (
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

const (
	DW0Mask uint32 = (0b1 << 27) | (0b1 << 24) | (0b11 << 21) | (0b1111 << 16) | 0b11111100
	DW1Mask uint32 = 0b11111101111111111100001111111111
)

// "GPP_A", "GPP_B", "GPP_F", "GPP_C", "GPP_D", "GPP_E", "GPD", "GPP_I", "GPP_J", "GPP_G",
// "GPP_H", "GPP_L"
var GPPGroups = []string{"GPP_", "GPD"}

var remapping = common.ResetSources{
	0b00: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
	0b01: bits.RstCfgDEEP << bits.DW0PadRstCfg,
	0b10: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
}

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

// RemmapRstSrc() remaps Pad Reset Source Config
func (p *BasePlatform) RemapResetSource(m *common.Macro) {
	if strings.Contains(m.GetPadId(), "GPD") {
		// See reset map for the Sunrise GPD Group in the Community 2:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/skylake/gpio.c#L15
		// remmap is not required because it is the same as common.
		return
	}
	if err := p.UpdateResetSource(remapping); err != nil {
		logs.Errorf("remap reset source for %s: %v", m.GetPadId(), err)
	}
}

// Pull() adds The Pad Termination (TERM) parameter from PAD_CFG_DW1 to the macro
func (p *BasePlatform) Pull(m *common.Macro) {
	dw1 := p.GetRegisterDW1()
	var pull = map[uint32]string{
		0b0000: "NONE",
		0b0010: "DN_5K",
		0b0100: "DN_20K",
		0b1001: "UP_1K",
		0b1010: "UP_5K",
		0b1011: "UP_2K",
		0b1100: "UP_20K",
		0b1101: "UP_667",
		0b1111: "NATIVE",
	}
	term, valid := pull[dw1.GetTermination()]
	if !valid {
		term = "INVALID"
		logs.Errorf("%s: DW1 %s: invalid termination value 0b%b",
			dw1, m.GetPadId(), dw1.GetTermination())
	}
	m.Separator().Add(term)
}

// ioApicRoute() generate macro to cause peripheral IRQ when configured in GPIO input mode
func ioApicRoute(p *BasePlatform, m *common.Macro) bool {
	dw0 := p.GetRegisterDW0()
	if dw0.GetGPIOInputRouteIOxAPIC() == 0 {
		return false
	}

	m.Add("_APIC")
	if dw0.GetRXLevelEdgeConfiguration() == bits.TrigLEVEL {
		if dw0.GetRxInvert() != 0 {
			// PAD_CFG_GPI_APIC_LOW(pad, pull, rst)
			m.Add("_LOW")
		} else {
			// PAD_CFG_GPI_APIC_HIGH(pad, pull, rst)
			m.Add("_HIGH")
		}
		m.Add("(").Id().Pull().Rstsrc().Add("),")
		return true
	}

	// e.g. PAD_CFG_GPI_APIC_IOS(pad, pull, rst, trig, inv, iosstate, iosterm)
	m.Add("_IOS(").Id().Pull().Rstsrc().Trig().Invert().Add(", TxLASTRxE, SAME),")
	return true
}

// nmiRoute() generate macro to cause NMI when configured in GPIO input mode
func nmiRoute(p *BasePlatform, m *common.Macro) bool {
	if p.GetRegisterDW0().GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	m.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// sciRoute() generate macro to cause SCI when configured in GPIO input mode
func sciRoute(p *BasePlatform, m *common.Macro) bool {
	dw0 := p.GetRegisterDW0()
	if dw0.GetGPIOInputRouteSCI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SCI(GPP_B18, UP_20K, PLTRST, LEVEL, INVERT),
	m.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// smiRoute() generates macro to cause SMI when configured in GPIO input mode
func smiRoute(p *BasePlatform, m *common.Macro) bool {
	dw0 := p.GetRegisterDW0()
	if dw0.GetGPIOInputRouteSMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SMI(GPP_E7, NONE, DEEP, LEVEL, NONE),
	m.Add("_SMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
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

	switch argc := len(ids); argc {
	case 0:
		// e.g. PAD_CFG_GPI_TRIG_OWN(pad, pull, rst, trig, own)
		m.Add("_TRIG_OWN").Add("(").Id().Pull().Rstsrc().Trig().Own().Add("),")
	case 1:
		// GPI with IRQ route
		if p2m.Config.IgnoredFields {
			// Set Host Software Ownership to ACPI mode
			m.SetOwnershipAcpi()
		}

	case 2:
		// PAD_CFG_GPI_DUAL_ROUTE(pad, pull, rst, trig, inv, route1, route2)
		m.Set("PAD_CFG_GPI_DUAL_ROUTE(").Id().Pull().Rstsrc().Trig().Invert()
		m.Add(", " + ids[0] + ", " + ids[1] + "),")
		if p2m.Config.IgnoredFields {
			// Set Host Software Ownership to ACPI mode
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
	// #define PAD_CFG_GPO(pad, val, rst)                \
	//    _PAD_CFG_STRUCT(pad,                           \
	//      PAD_FUNC(GPIO) | PAD_RESET(rst) |            \
	//      PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | !!val, \
	//      PAD_PULL(NONE) | PAD_IOSSTATE(TxLASTRxE))
	if dw0 := p.GetRegisterDW0(); dw0.GetRXLevelEdgeConfiguration() != bits.TrigOFF {
		mask := bits.DW0[bits.DW0RxLevelEdgeConfiguration]
		dw0.CntrMaskFieldsClear(mask)
	}
	m.Set("PAD_CFG")
	if m.IsOwnershipDriver() {
		// PAD_CFG_GPO_GPIO_DRIVER(pad, val, rst, pull)
		m.Add("_GPO_GPIO_DRIVER").Add("(").Id().Val().Rstsrc().Pull().Add("),")
		return
	}
	dw1 := p.GetRegisterDW1()
	term := dw1.GetTermination()
	if term != 0 {
		// e.g. PAD_CFG_TERM_GPO(GPP_B23, 1, DN_20K, DEEP),
		m.Add("_TERM")
	}
	m.Add("_GPO").Add("(").Id().Val()
	if term != 0 {
		m.Pull()
	}
	m.Rstsrc().Add("),")
}

// AddNativeFunctionMacro() adds PAD_CFG_NF macro with arguments
func (p *BasePlatform) AddNativeFunctionMacro(m *common.Macro) {
	// e.g. PAD_CFG_NF(GPP_D23, NONE, DEEP, NF1)
	m.Set("PAD_CFG_NF")
	if p.GetRegisterDW1().GetPadTol() != 0 {
		m.Add("_1V8")
	}
	m.Add("(").Id().Pull().Rstsrc().Padfn().Add("),")
}

// AddNoConnMacro() adds PAD_NC macro
func (p *BasePlatform) AddNoConnMacro(m *common.Macro) {
	// #define PAD_NC(pad, pull)
	// _PAD_CFG_STRUCT(pad,
	//     PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE),
	//     PAD_PULL(pull) | PAD_IOSSTATE(TxDRxE)),
	dw0 := p.GetRegisterDW0()

	// Some fields of the configuration registers are hidden inside the macros,
	// we should check them to update the corresponding bits in the control mask.
	if dw0.GetRXLevelEdgeConfiguration() != bits.TrigOFF {
		mask := bits.DW0[bits.DW0RxLevelEdgeConfiguration]
		dw0.CntrMaskFieldsClear(mask)
	}
	if dw0.GetResetConfig() != 1 { // 1 = RST_DEEP
		mask := bits.DW0[bits.DW0PadRstCfg]
		dw0.CntrMaskFieldsClear(mask)
	}
	m.Set("PAD_NC").Add("(").Id().Pull().Add("),")
}
