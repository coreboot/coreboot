/* SPDX-License-Identifier: GPL-2.0-only */
package decoder

import (
	"fmt"
	"math/bits"
)

type Fields[T uint32 | string] struct {
	PortConnectivity   T
	Location           T
	DefaultDevice      T
	ConnectionType     T
	Color              T
	Misc               T
	DefaultAssociation T
	Sequence           T
}

func getField(config uint32, mask uint32) uint32 {
	return (config & mask) >> bits.TrailingZeros32(mask)
}

func Decode(config uint32) Fields[uint32] {
	return Fields[uint32]{
		PortConnectivity:   getField(config, 0xc0000000),
		Location:           getField(config, 0x3f000000),
		DefaultDevice:      getField(config, 0x00f00000),
		ConnectionType:     getField(config, 0x000f0000),
		Color:              getField(config, 0x0000f000),
		Misc:               getField(config, 0x00000f00),
		DefaultAssociation: getField(config, 0x000000f0),
		Sequence:           getField(config, 0x0000000f),
	}
}

func PortIsConnected(config uint32) bool {
	return Decode(config).PortConnectivity != 0x1
}

var portConnectivityDescriptions = map[uint32]string{
	0x0: "AZALIA_JACK",
	0x1: "AZALIA_NC",
	0x2: "AZALIA_INTEGRATED",
	0x3: "AZALIA_JACK_AND_INTEGRATED",
}

var grossLocationDescriptions = map[uint32]string{
	0x00: "AZALIA_EXTERNAL_PRIMARY_CHASSIS",
	0x10: "AZALIA_INTERNAL",
	0x20: "AZALIA_SEPARATE_CHASSIS",
	0x30: "AZALIA_LOCATION_OTHER",
}

var geometricLocationDescriptions = map[uint32]string{
	0x00: "AZALIA_GEOLOCATION_NA",
	0x01: "AZALIA_REAR",
	0x02: "AZALIA_FRONT",
	0x03: "AZALIA_LEFT",
	0x04: "AZALIA_RIGHT",
	0x05: "AZALIA_TOP",
	0x06: "AZALIA_BOTTOM",
	0x07: "AZALIA_SPECIAL7",
	0x08: "AZALIA_SPECIAL8",
	0x09: "AZALIA_SPECIAL9",
}

var specialLocationDescriptions = map[uint32]string{
	0x00 | 0x07: "AZALIA_REAR_PANEL",
	0x00 | 0x08: "AZALIA_DRIVE_BAY",
	0x10 | 0x07: "AZALIA_RISER",
	0x10 | 0x08: "AZALIA_DIGITAL_DISPLAY",
	0x10 | 0x09: "AZALIA_ATAPI",
	0x30 | 0x07: "AZALIA_MOBILE_LID_INSIDE",
	0x30 | 0x08: "AZALIA_MOBILE_LID_OUTSIDE",
}

var defaultDeviceDescriptions = map[uint32]string{
	0x0: "AZALIA_LINE_OUT",
	0x1: "AZALIA_SPEAKER",
	0x2: "AZALIA_HP_OUT",
	0x3: "AZALIA_CD",
	0x4: "AZALIA_SPDIF_OUT",
	0x5: "AZALIA_DIGITAL_OTHER_OUT",
	0x6: "AZALIA_MODEM_LINE_SIDE",
	0x7: "AZALIA_MODEM_HANDSET_SIDE",
	0x8: "AZALIA_LINE_IN",
	0x9: "AZALIA_AUX",
	0xa: "AZALIA_MIC_IN",
	0xb: "AZALIA_TELEPHONY",
	0xc: "AZALIA_SPDIF_IN",
	0xd: "AZALIA_DIGITAL_OTHER_IN",
	0xf: "AZALIA_DEVICE_OTHER",
}

var connectionTypeDescriptions = map[uint32]string{
	0x0: "AZALIA_TYPE_UNKNOWN",
	0x1: "AZALIA_STEREO_MONO_1_8",
	0x2: "AZALIA_STEREO_MONO_1_4",
	0x3: "AZALIA_ATAPI_INTERNAL",
	0x4: "AZALIA_RCA",
	0x5: "AZALIA_OPTICAL",
	0x6: "AZALIA_OTHER_DIGITAL",
	0x7: "AZALIA_OTHER_ANALOG",
	0x8: "AZALIA_MULTICHANNEL_ANALOG",
	0x9: "AZALIA_XLR",
	0xa: "AZALIA_RJ_11",
	0xb: "AZALIA_COMBINATION",
	0xf: "AZALIA_TYPE_OTHER",
}

var colorDescriptions = map[uint32]string{
	0x0: "AZALIA_COLOR_UNKNOWN",
	0x1: "AZALIA_BLACK",
	0x2: "AZALIA_GREY",
	0x3: "AZALIA_BLUE",
	0x4: "AZALIA_GREEN",
	0x5: "AZALIA_RED",
	0x6: "AZALIA_ORANGE",
	0x7: "AZALIA_YELLOW",
	0x8: "AZALIA_PURPLE",
	0x9: "AZALIA_PINK",
	0xe: "AZALIA_WHITE",
	0xf: "AZALIA_COLOR_OTHER",
}

var miscDescriptions = map[uint32]string{
	0x0: "AZALIA_JACK_PRESENCE_DETECT",
	0x1: "AZALIA_NO_JACK_PRESENCE_DETECT",
}

func getDescription(field uint32, descriptions map[uint32]string) string {
	desc, exists := descriptions[field]

	if !exists {
		return fmt.Sprintf("0x%x", field)
	}
	return desc
}

func getLocationDescription(field uint32) string {
	desc, isSpecialLocation := specialLocationDescriptions[field]
	if isSpecialLocation {
		return desc
	}

	grossLocation := field & 0x30
	geometricLocation := field & 0x0f

	desc = grossLocationDescriptions[grossLocation]
	if geometricLocation != 0x00 {
		desc += " | " + getDescription(geometricLocation, geometricLocationDescriptions)
	}
	return desc
}

func getMiscDescription(field uint32) string {
	presenceBit := field & 0b0001
	reservedBits := field & 0b1110

	desc := miscDescriptions[presenceBit]
	if bits.OnesCount32(reservedBits) > 0 {
		desc += fmt.Sprintf(" | 0x%x", reservedBits)
	}
	return desc
}

func ToHumanReadable(fields Fields[uint32]) Fields[string] {
	return Fields[string]{
		PortConnectivity:   getDescription(fields.PortConnectivity, portConnectivityDescriptions),
		Location:           getLocationDescription(fields.Location),
		DefaultDevice:      getDescription(fields.DefaultDevice, defaultDeviceDescriptions),
		ConnectionType:     getDescription(fields.ConnectionType, connectionTypeDescriptions),
		Color:              getDescription(fields.Color, colorDescriptions),
		Misc:               getMiscDescription(fields.Misc),
		DefaultAssociation: fmt.Sprintf("%d", fields.DefaultAssociation),
		Sequence:           fmt.Sprintf("%d", fields.Sequence),
	}
}

func ConfigToVerbs(address uint32, nodeId uint32, config uint32) [4]uint32 {
	return [4]uint32{
		(address << 28) | (nodeId << 20) | (0x71c << 8) | ((config >> 0) & 0xff),
		(address << 28) | (nodeId << 20) | (0x71d << 8) | ((config >> 8) & 0xff),
		(address << 28) | (nodeId << 20) | (0x71e << 8) | ((config >> 16) & 0xff),
		(address << 28) | (nodeId << 20) | (0x71f << 8) | ((config >> 24) & 0xff),
	}
}
