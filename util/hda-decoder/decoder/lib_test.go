// SPDX-License-Identifier: GPL-2.0-only
package decoder

import (
	"reflect"
	"testing"
)

type portIsConnectedTest struct {
	arg1     uint32
	expected bool
}

var portIsConnectedTests = []portIsConnectedTest{
	portIsConnectedTest{0x20000000, true},
	portIsConnectedTest{0xC0000000, true},
	portIsConnectedTest{0x40000000, false},
}

func TestPortIsConnected(t *testing.T) {
	for _, test := range portIsConnectedTests {
		output := PortIsConnected(test.arg1)
		if output != test.expected {
			t.Errorf("Expected %v, received %v", test.expected, output)
		}
	}
}

type decodeTest struct {
	arg1     uint32
	expected Fields[uint32]
}

var decodeTests = []decodeTest{
	decodeTest{0xe23d1a0e, Fields[uint32]{0x3, 0x22, 0x3, 0xd, 0x1, 0xa, 0x0, 0xe}},
	decodeTest{0x66a8a2e4, Fields[uint32]{0x1, 0x26, 0xa, 0x8, 0xa, 0x2, 0xe, 0x4}},
	decodeTest{0x2e00a164, Fields[uint32]{0x0, 0x2e, 0x0, 0x0, 0xa, 0x1, 0x6, 0x4}},
	decodeTest{0x3b83dfe9, Fields[uint32]{0x0, 0x3b, 0x8, 0x3, 0xd, 0xf, 0xe, 0x9}},
	decodeTest{0x51708701, Fields[uint32]{0x1, 0x11, 0x7, 0x0, 0x8, 0x7, 0x0, 0x1}},
}

func TestDecode(t *testing.T) {
	for _, test := range decodeTests {
		output := Decode(test.arg1)
		if !reflect.DeepEqual(output, test.expected) {
			t.Errorf("Expected %v, received %v", test.expected, output)
		}
	}
}

type toHumanReadableTest struct {
	arg1     uint32
	expected Fields[string]
}

var toHumanReadableTests = []toHumanReadableTest{
	toHumanReadableTest{0xe23d1a0e, Fields[string]{
		"AZALIA_JACK_AND_INTEGRATED",
		"AZALIA_SEPARATE_CHASSIS | AZALIA_FRONT",
		"AZALIA_CD",
		"0xd",
		"AZALIA_BLACK",
		"AZALIA_JACK_PRESENCE_DETECT | 0xa",
		"0",
		"14",
	}},

	toHumanReadableTest{0x57708701, Fields[string]{
		"AZALIA_NC",
		"AZALIA_RISER",
		"AZALIA_MODEM_HANDSET_SIDE",
		"AZALIA_TYPE_UNKNOWN",
		"AZALIA_PURPLE",
		"AZALIA_NO_JACK_PRESENCE_DETECT | 0x6",
		"0",
		"1",
	}},

	toHumanReadableTest{0x2e00a164, Fields[string]{
		"AZALIA_JACK",
		"AZALIA_SEPARATE_CHASSIS | 0xe",
		"AZALIA_LINE_OUT",
		"AZALIA_TYPE_UNKNOWN",
		"0xa",
		"AZALIA_NO_JACK_PRESENCE_DETECT",
		"6",
		"4",
	}},

	toHumanReadableTest{0x80949653, Fields[string]{
		"AZALIA_INTEGRATED",
		"AZALIA_EXTERNAL_PRIMARY_CHASSIS",
		"AZALIA_AUX",
		"AZALIA_RCA",
		"AZALIA_PINK",
		"AZALIA_JACK_PRESENCE_DETECT | 0x6",
		"5",
		"3",
	}},
}

func TestToHumanReadable(t *testing.T) {
	for _, test := range toHumanReadableTests {
		output := ToHumanReadable(Decode(test.arg1))
		if output != test.expected {
			t.Errorf("Expected %v, received %v", test.expected, output)
		}
	}
}
