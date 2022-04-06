package config

import "os"

const (
	TempInteltool  int  = 0
	TempGpioh      int  = 1
	TempSpec       int  = 2
)

var template int = 0

func TemplateSet(temp int) bool {
	if temp > TempSpec {
		return false
	} else {
		template = temp
		return true
	}
}

func TemplateGet() int {
	return template
}

const (
	SunriseType   uint8  = 0
	LewisburgType uint8  = 1
	ApolloType    uint8  = 2
	CannonType    uint8  = 3
	AlderType    uint8  = 4
)

var key uint8 = SunriseType

var platform = map[string]uint8{
	"snr": SunriseType,
	"lbg": LewisburgType,
	"apl": ApolloType,
	"cnl": CannonType,
	"adl": AlderType}
func PlatformSet(name string) int {
	if platformType, valid := platform[name]; valid {
		key = platformType
		return 0
	}
	return -1
}
func PlatformGet() uint8 {
	return key
}
func IsPlatform(platformType uint8) bool {
	return platformType == key
}
func IsPlatformApollo() bool {
	return IsPlatform(ApolloType)
}
func IsPlatformSunrise() bool {
	return IsPlatform(SunriseType)
}
func IsPlatformLewisburg() bool {
	return IsPlatform(LewisburgType)
}
func IsPlatformCannonLake() bool {
	return IsPlatform(CannonType)
}
func IsPlatformAlderLakeH() bool {
	return IsPlatform(AlderType)
}

var InputRegDumpFile *os.File = nil
var OutputGenFile *os.File = nil

var ignoredFieldsFormat bool = false
func IgnoredFieldsFlagSet(flag bool) {
	ignoredFieldsFormat = flag
}
func AreFieldsIgnored() bool {
	return ignoredFieldsFormat
}

var nonCheckingFlag bool = false
func NonCheckingFlagSet(flag bool) {
	nonCheckingFlag = flag
}
func IsNonCheckingFlagUsed() bool {
	return nonCheckingFlag
}


var infolevel int = 0
func InfoLevelSet(lvl int) {
	infolevel = lvl
}
func InfoLevelGet() int {
	return infolevel
}

var fldstyle uint8 = CbFlds
const (
	NoFlds  uint8  = 0
	CbFlds  uint8  = 1 // coreboot style
	FspFlds uint8  = 2 // FSP/edk2 style
	RawFlds uint8  = 3 // raw DW0/1 values
)
var fldstylemap = map[string]uint8{
	"none" : NoFlds,
	"cb"   : CbFlds,
	"fsp"  : FspFlds,
	"raw"  : RawFlds}
func FldStyleSet(name string) int {
	if style, valid := fldstylemap[name]; valid {
		fldstyle = style
		return 0
	}
	return -1
}
func FldStyleGet() uint8 {
	return fldstyle
}
func IsFieldsMacroUsed() bool {
	return FldStyleGet() != NoFlds
}
func IsCbStyleMacro() bool {
	return FldStyleGet() == CbFlds
}
func IsFspStyleMacro() bool {
	return FldStyleGet() == FspFlds
}
func IsRawFields() bool {
	return FldStyleGet() == RawFlds
}
