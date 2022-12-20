package common

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register"
)

type BasePlatform struct {
	dw0 register.DW0
	dw1 register.DW1
}

func InitBasePlatform(dw0, dw0Mask uint32, dw1, dw1Mask uint32) BasePlatform {
	return BasePlatform{
		register.SetVal[register.DW0](dw0, dw0Mask),
		register.SetVal[register.DW1](dw1, dw1Mask),
	}
}

func (p *BasePlatform) GetRegisterDW0() *register.DW0 {
	return &p.dw0
}

func (p *BasePlatform) GetRegisterDW1() *register.DW1 {
	return &p.dw1
}
