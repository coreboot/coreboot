package jsl

import (
	"strings"

	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

const (
	DW0Mask = (0b1 << 27) | (0b1 << 18) | (0b00111111 << 11) | (0b00111111 << 2) | (0b1 << 1)
	DW1Mask = 0b11111101111111111100001111111111
)

// "GPP_A", "GPP_B", "GPP_C", "GPP_D", "GPP_E", "GPP_F", "GPP_G", "GPP_H", "GPP_R", "GPP_S",
// "GPP_T", "GPD", "HVMOS", "VGPIO5"
var GPPGroups = []string{"GPP_", "GPD", "VGPIO"}

var remapping = common.ResetSources{
	0b00: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
	0b01: bits.RstCfgDEEP << bits.DW0PadRstCfg,
	0b10: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
}

type BasePlatform struct {
	// based on the Cannon Lake platform
	cnl.BasePlatform
}

func InitBasePlatform(dw0, dw0mask uint32, dw1, dw1mask uint32) BasePlatform {
	return BasePlatform{cnl.InitBasePlatform(dw0, dw0mask, dw1, dw1mask)}
}

func GetPlatform(dw0, dw1 uint32) common.PlatformIf {
	p := InitBasePlatform(dw0, DW0Mask, dw1, DW1Mask)
	return &p
}

// Override BasePlatform.RemapResetSource()
func (p *BasePlatform) RemapResetSource(m *common.Macro) {
	if strings.Contains(m.GetPadId(), "GPP_F") ||
		strings.Contains(m.GetPadId(), "GPP_B") ||
		strings.Contains(m.GetPadId(), "GPP_A") ||
		strings.Contains(m.GetPadId(), "GPP_S") ||
		strings.Contains(m.GetPadId(), "GPP_R") {
		// See reset map for the Jasper Lake Community 0:
		// https://github.com/coreboot/coreboot/blob/master/src/soc/intel/jasperlake/gpio.c#L21
		// remmap is not required because it is the same as common.
		return
	}
	if err := p.UpdateResetSource(remapping); err != nil {
		logs.Errorf("remap reset source for %s: %v", m.GetPadId(), err)
	}
}
