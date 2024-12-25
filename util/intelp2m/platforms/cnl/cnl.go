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

var GPPGroups = []string{
	"GPP_", "GPD", // "GPP_A", "GPP_B", "GPP_G", "GPP_D", "GPP_F", "GPP_H", "GPP_C", "GPP_E"
	"I2S1_TXD", "CNV_BTEN", "CNV_GNEN", "CNV_WFEN", "CNV_WCEN", "vCNV_GNSS_HOST_WAKE_B",
	"vSD3_CD_B", "CNV_BT_HOST_WAKE_B", "CNV_BT_IF_SELECT", "vCNV_BT_UART_TXD",
	"vCNV_BT_UART_RXD", "vCNV_BT_UART_CTS_B", "vCNV_BT_UART_RTS_B", "vCNV_MFUART1_TXD",
	"vCNV_MFUART1_RXD", "vCNV_MFUART1_CTS_B", "vCNV_MFUART1_RTS_B", "vCNV_GNSS_UART_TXD",
	"vCNV_GNSS_UART_RXD", "vCNV_GNSS_UART_CTS_B", "vCNV_GNSS_UART_RTS_B", "vUART0_TXD",
	"vUART0_RXD", "vUART0_CTS_B", "vUART0_RTS_B", "vISH_UART0_TXD", "vISH_UART0_RXD",
	"vISH_UART0_CTS_B", "vISH_UART0_RTS_B", "vISH_UART1_TXD", "vISH_UART1_RXD",
	"vISH_UART1_CTS_B", "vISH_UART1_RTS_B", "vCNV_BT_I2S_BCLK", "vCNV_BT_I2S_WS_SYNC",
	"vCNV_BT_I2S_SDO", "vCNV_BT_I2S_SDI", "vSSP2_SCLK", "vSSP2_SFRM", "vSSP2_TXD",
	"vSSP2_RXD", "SLP_LAN_B", "SLP_SUS_B", "WAKE_B", "DRAM_RESET_B", "SPI0_IO_2",
	"SPI0_IO_3", "SPI0_MISO", "SPI0_MOSI", "SPI0_CS2_B", "SPI0_CS0_B", "SPI0_CS1_B",
	"SPI0_CLK", "SPI0_CLK_LOOPBK", "HDACPU_SDI", "HDACPU_SDO", "HDACPU_SCLK", "PM_SYNC",
	"PECI", "CPUPWRGD", "THRMTRIP_B", "PLTRST_CPU_B", "PM_DOWN", "TRIGGER_IN",
	"TRIGGER_OUT", "PCH_TDO", "PCH_JTAGX", "PROC_PRDY_B", "PROC_PREQ_B", "CPU_TRST_B",
	"PCH_TDI", "PCH_TMS", "PCH_TCK", "ITP_PMODE", "SYS_PWROK", "SYS_RESET_B", "CL_RST_B",
	"HDA_BCLK", "HDA_RST_B", "HDA_SYNC", "HDA_SDO", "HDA_SDI0", "HDA_SDI1", "I2S1_SFRM",
}

var remapping = common.ResetSources{
	0b00: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
	0b01: bits.RstCfgDEEP << bits.DW0PadRstCfg,
	0b10: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
}

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

// Override BasePlatform.RemapResetSource()
func (p *BasePlatform) RemapResetSource(m *common.Macro) {
	if strings.Contains(m.GetPadId(), "GPP_A") ||
		strings.Contains(m.GetPadId(), "GPP_B") ||
		strings.Contains(m.GetPadId(), "GPP_G") {
		// See reset map for the Cannonlake Groups the Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/cannonlake/gpio.c#L14
		// remmap is not required because it is the same as common.
		return
	}
	if err := p.UpdateResetSource(remapping); err != nil {
		logs.Errorf("remap reset source for %s: %v", m.GetPadId(), err)
	}
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
