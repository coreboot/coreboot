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

// Override the base platform method
func (p *BasePlatform) RemapRstSrc(m *common.Macro) {
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

	remapping := map[uint32]uint32{
		0b00: bits.RstCfgRSMRST << bits.DW0PadRstCfg,
		0b01: bits.RstCfgDEEP << bits.DW0PadRstCfg,
		0b10: bits.RstCfgPLTRST << bits.DW0PadRstCfg,
	}
	dw0 := p.GetRegisterDW0()
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
