package ehl

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
)

const (
	DW0Mask = (0b1 << 27) | (0b1111 << 14) | 0b11111100
	DW1Mask = 0b11111111111111000000000011111111
)

var GPPGroups = []string{"GPP_", "GPD_", "GPIO_", "VGPIO_"}

type BasePlatform struct {
	// based on the Apollo Lake SoC
	apl.BasePlatform
}

func InitBasePlatform(dw0Val, dw0Mask uint32, dw1Val, dw1Mask uint32) BasePlatform {
	return BasePlatform{apl.InitBasePlatform(dw0Val, dw0Mask, dw1Val, dw1Mask)}
}

func GetPlatform(dw0, dw1 uint32) common.PlatformIf {
	p := InitBasePlatform(dw0, DW0Mask, dw1, DW1Mask)
	return &p
}

// Override BasePlatform.Pull()
func (p *BasePlatform) Pull(m *common.Macro) {
	dw1 := p.GetRegisterDW1()
	var pull = map[uint32]string{
		0b0000: "NONE",
		0b0001: "DN_1K",
		0b0010: "DN_5K",
		0b0100: "DN_20K",
		0b1000: "UP_1K_5K_20K",
		0b1001: "UP_1K",
		0b1010: "UP_5K",
		0b1011: "UP_1K_5K",
		0b1100: "UP_20K",
		0b1101: "UP_1K_20K",
		0b1110: "UP_5K_20K",
		0b1111: "NATIVE",
	}
	term, valid := pull[dw1.GetTermination()]
	if !valid {
		term = "ERROR"
		logs.Errorf("%s: DW1 %s: invalid termination value 0b%b",
			dw1, m.GetPadId(), dw1.GetTermination())
	}
	m.Separator().Add(term)
}
