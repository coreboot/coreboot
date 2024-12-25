package platforms

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/adl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/ebg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/jsl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/lbg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/mtl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/tgl"
)

type Constructor func(dw0, dw1 uint32) common.PlatformIf

var ErrUnregisteredPlatform = fmt.Errorf("unregistered platform type %d", p2m.Config.Platform)

var platformConstructorMap = map[p2m.PlatformType]Constructor{
	p2m.Alder:      adl.GetPlatform,
	p2m.Apollo:     apl.GetPlatform,
	p2m.Cannon:     cnl.GetPlatform,
	p2m.Sunrise:    snr.GetPlatform,
	p2m.Tiger:      tgl.GetPlatform,
	p2m.Jasper:     jsl.GetPlatform,
	p2m.Meteor:     mtl.GetPlatform,
	p2m.Emmitsburg: ebg.GetPlatform,
	p2m.Lewisburg:  lbg.GetPlatform,
}

var GppMap = map[p2m.PlatformType][]string{
	p2m.Alder:      adl.GPPGroups,
	p2m.Apollo:     apl.GPPGroups,
	p2m.Cannon:     cnl.GPPGroups,
	p2m.Sunrise:    snr.GPPGroups,
	p2m.Tiger:      tgl.GPPGroups,
	p2m.Jasper:     jsl.GPPGroups,
	p2m.Meteor:     mtl.GPPGroups,
	p2m.Emmitsburg: ebg.GPPGroups,
	p2m.Lewisburg:  lbg.GPPGroups,
}

func GetConstructor() (Constructor, error) {
	constructor, exist := platformConstructorMap[p2m.Config.Platform]
	if !exist {
		return nil, ErrUnregisteredPlatform
	}
	return constructor, nil
}

func GetGPPGroups() ([]string, error) {
	groups, exist := GppMap[p2m.Config.Platform]
	if !exist {
		return nil, ErrUnregisteredPlatform
	}
	return groups, nil
}
