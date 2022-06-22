package test

import (
	"fmt"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config"
)

type (
	Pad struct {
		ID        string
		DW0       uint32
		DW1       uint32
		Ownership uint8
	}
	Macro struct {
		Short string
		Long  string
	}
	TestCase struct {
		Pad
		Macro
	}
	PlatformSpecificIf interface {
		GenMacro(string, uint32, uint32, uint8) string
	}
	Suite []TestCase
)

func (p Pad) toShortMacro(platform PlatformSpecificIf) string {
	config.FldStyleSet("none")
	config.NonCheckingFlagSet(true)
	return platform.GenMacro(p.ID, p.DW0, p.DW1, p.Ownership)
}

func (p Pad) toLongMacro(platform PlatformSpecificIf) string {
	config.FldStyleSet("cb")
	config.NonCheckingFlagSet(false)
	return platform.GenMacro(p.ID, p.DW0, p.DW1, p.Ownership)
}

func (m *Macro) checkFor(platform PlatformSpecificIf, pad Pad) error {
	var format string = "%s macro\nExpects:  '%s'\nActually: '%s'\n\n"
	if actually := pad.toLongMacro(platform); m.Long != "" && m.Long != actually {
		return fmt.Errorf(format, "LONG", m.Long, actually)
	}
	if actually := pad.toShortMacro(platform); m.Short != "" && m.Short != actually {
		return fmt.Errorf(format, "SHORT", m.Short, actually)
	}
	return nil
}

func (suite Suite) Run(t *testing.T, label string, platform PlatformSpecificIf) {
	t.Run(label, func(t *testing.T) {
		for _, testcase := range suite {
			if err := testcase.Macro.checkFor(platform, testcase.Pad); err != nil {
				t.Errorf("Test failed for pad %s\n%v", testcase.Pad.ID, err)
			}
		}
	})
}
