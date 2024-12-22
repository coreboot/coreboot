package common

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/register/bits"
)

type ResetSources map[uint32]uint32

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

// UpdateResetSource() updates the Pad Reset configuration fields in the DW0 register according
// to the ResetSources{} map. Redefine this function for the corresponding platform if it has a
// difference in logic or register bits.
func (p *BasePlatform) UpdateResetSource(remapping ResetSources) error {
	dw0 := p.GetRegisterDW0()
	mask := bits.DW0[bits.DW0PadRstCfg]
	source, valid := remapping[dw0.GetResetConfig()]
	if !valid {
		dw0.CntrMaskFieldsClear(mask)
		return fmt.Errorf("invalid reset config source value 0b%b", dw0.GetResetConfig())
	}
	dw0.Value &= ^mask
	dw0.Value |= source
	dw0.CntrMaskFieldsClear(mask)
	return nil
}
