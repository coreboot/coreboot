package p2m

import (
	"fmt"
)

type PlatformType int
type FieldType int

const (
	Sunrise PlatformType = iota
	Lewisburg
	Apollo
	Cannon
	Alder
	Jasper
	Tiger
	Meteor
	Emmitsburg
)

const (
	NoFlds  FieldType = iota
	CbFlds            // coreboot style
	FspFlds           // FSP/edk2 style
	RawFlds           // raw DW0/1 values
)

var platforms = map[string]PlatformType{
	"snr": Sunrise,
	"lbg": Lewisburg,
	"apl": Apollo,
	"cnl": Cannon,
	"tgl": Tiger,
	"adl": Alder,
	"jsl": Jasper,
	"mtl": Meteor,
	"ebg": Emmitsburg,
}

var fields = map[string]FieldType{
	"none": NoFlds,
	"cb":   CbFlds,
	"fsp":  FspFlds,
	"raw":  RawFlds,
}

type Settings struct {
	Version       string
	Platform      PlatformType
	Field         FieldType
	InputPath     string
	OutputPath    string
	LogsPath      string
	IgnoredFields bool
	AutoCheck     bool
	GenLevel      int
}

var defaultConfig = Settings{
	Version:       "unknown",
	Platform:      Sunrise,
	Field:         NoFlds,
	IgnoredFields: false,
	AutoCheck:     true,
	GenLevel:      0,
}

var Config = defaultConfig

func SettingsReset() {
	Config = defaultConfig
}

func SetPlatformType(format string) error {
	if _, exist := platforms[format]; !exist {
		return fmt.Errorf("unknown platform type %s", format)
	}
	Config.Platform = platforms[format]
	return nil
}

func SetFieldType(format string) error {
	if _, exist := fields[format]; !exist {
		return fmt.Errorf("unknown field type %s", format)
	}
	Config.Field = fields[format]
	return nil
}
