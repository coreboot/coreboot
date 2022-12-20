package platforms

import (
	"fmt"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
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

type KeywordAction func(line string) bool

type Constructor func(dw0, dw1 uint32) common.PlatformIf

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

func GetKeywordChekingAction() KeywordAction {
	actions := map[p2m.PlatformType]KeywordAction{
		p2m.Alder:      adl.CheckKeyword,
		p2m.Apollo:     apl.CheckKeyword,
		p2m.Cannon:     cnl.CheckKeyword,
		p2m.Sunrise:    snr.CheckKeyword,
		p2m.Tiger:      tgl.CheckKeyword,
		p2m.Jasper:     jsl.CheckKeyword,
		p2m.Meteor:     mtl.CheckKeyword,
		p2m.Emmitsburg: ebg.CheckKeyword,
		p2m.Lewisburg:  lbg.CheckKeyword,
	}
	action, exist := actions[p2m.Config.Platform]
	if !exist {
		logs.Errorf("unregistered platform type %d", p2m.Config.Platform)
		return nil
	}
	return action
}

func GetConstructor() (Constructor, error) {
	constructor, exist := platformConstructorMap[p2m.Config.Platform]
	if !exist {
		return nil, fmt.Errorf("unregistered platform type %d", p2m.Config.Platform)
	}
	return constructor, nil
}
