package platforms

import (
	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/logs"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/adl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/cnl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/lbg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

type SpecificIf interface {
	GenMacro(id string, dw0 uint32, dw1 uint32, ownership uint8) string
	KeywordCheck(line string) bool
}

func GetSpecificInterface() SpecificIf {
	platforms := map[p2m.PlatformType]SpecificIf{
		p2m.Alder:   adl.PlatformSpecific{},
		p2m.Apollo:  apl.PlatformSpecific{},
		p2m.Sunrise: snr.PlatformSpecific{},
		p2m.Cannon: cnl.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
			InheritanceMacro:    snr.PlatformSpecific{},
		},
		p2m.Lewisburg: lbg.PlatformSpecific{
			InheritanceTemplate: snr.PlatformSpecific{},
			InheritanceMacro:    snr.PlatformSpecific{},
		},
	}
	platform, exist := platforms[p2m.Config.Platform]
	if !exist {
		logs.Errorf("unknown platform type %d", int(p2m.Config.Platform))
		return nil
	}
	return platform
}
