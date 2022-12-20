package cnl

import (
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

const (
	DW0Mask uint32 = (0b1 << 27) | (0b1 << 24) | (0b11 << 21) | (0b1111 << 16) | 0b11111100
	DW1Mask uint32 = 0b11111101111111111100001111111111
)

type BasePlatform struct {
	// based on the Sunrise platform
	snr.BasePlatform
}

func InitBasePlatform(dw0, dw0mask uint32, dw1, dw1mask uint32) BasePlatform {
	return BasePlatform{snr.InitBasePlatform(dw0, dw0mask, dw1, dw1mask)}
}

func GetPlatform(dw0, dw1 uint32) common.PlatformIf {
	p := InitBasePlatform(dw0, DW0Mask, dw1, DW1Mask)
	return &p
}

// Override BasePlatform.RemapRstSrc()
func (p *BasePlatform) RemapRstSrc(m *common.Macro) {
	if strings.Contains(m.GetPadId(), "GPP_A") ||
		strings.Contains(m.GetPadId(), "GPP_B") ||
		strings.Contains(m.GetPadId(), "GPP_G") {
		// See reset map for the Cannonlake Groups the Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/cannonlake/gpio.c#L14
		// remmap is not required because it is the same as common.
		return
	}

	dw0 := p.GetRegisterDW0()
	remapping := map[uint32]uint32{
		0b00: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
		0b01: bits.RstCfgDEEP << bits.DW0PadRstCfg,
		0b10: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
	}
	source, valid := remapping[dw0.GetResetConfig()]
	if valid {
		dw0.Value &= 0x3fffffff
		dw0.Value |= source
	} else {
		logs.Errorf("%s: skip re-mapping: DW0 %s: invalid reset config source value 0b%b",
			m.GetPadId(), dw0, dw0.GetResetConfig())
	}
	mask := bits.DW0[bits.DW0PadRstCfg]
	dw0.CntrMaskFieldsClear(mask)
}

// Override BasePlatform.Pull()
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
	// PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv)
	m.Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// nmiRoute() generate macro to cause NMI when configured in GPIO input mode
func nmiRoute(p *BasePlatform, m *common.Macro) bool {
	if dw0 := p.GetRegisterDW0(); dw0.GetGPIOInputRouteNMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_NMI(GPIO_24, UP_20K, DEEP, LEVEL, INVERT),
	m.Add("_NMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// sciRoute() generate macro to cause SCI when configured in GPIO input mode
func sciRoute(p *BasePlatform, m *common.Macro) bool {
	if dw0 := p.GetRegisterDW0(); dw0.GetGPIOInputRouteSCI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SCI(pad, pull, rst, trig, inv)
	m.Add("_SCI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// smiRoute() generates macro to cause SMI when configured in GPIO input mode
func smiRoute(p *BasePlatform, m *common.Macro) bool {
	if dw0 := p.GetRegisterDW0(); dw0.GetGPIOInputRouteSMI() == 0 {
		return false
	}
	// PAD_CFG_GPI_SMI(pad, pull, rst, trig, inv)
	m.Add("_SMI").Add("(").Id().Pull().Rstsrc().Trig().Invert().Add("),")
	return true
}

// Override BasePlatform.AddGpiMacro()
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
