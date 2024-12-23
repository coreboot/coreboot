package test

import (
	"fmt"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/fields"
)

type (
	Pad struct {
		ID        string
		DW0       uint32
		DW1       uint32
		Ownership bool
	}
	Macro struct {
		Short string
		Long  string
	}
	TestCase struct {
		Pad
		Macro
	}
	Suite []TestCase
)

func (p Pad) toShortMacro() string {
	if err := p2m.SetFieldType("none"); err != nil {
		panic(err)
	}
	p2m.Config.AutoCheck = false
	constructor, err := platforms.GetConstructor()
	if err != nil {
		panic(err)
	}
	macro := common.CreateFrom(
		p.ID,
		p.Ownership,
		constructor(p.DW0, p.DW1),
		fields.Get(),
	)
	return macro.Generate()
}

func (p Pad) toLongMacro() string {
	if err := p2m.SetFieldType("cb"); err != nil {
		panic(err)
	}
	p2m.Config.AutoCheck = true
	constructor, err := platforms.GetConstructor()
	if err != nil {
		panic(err)
	}
	macro := common.CreateFrom(
		p.ID,
		p.Ownership,
		constructor(p.DW0, p.DW1),
		fields.Get(),
	)
	return macro.Generate()
}

func (m *Macro) check(pad Pad) error {
	var format string = "%s macro\nExpects:  '%s'\nActually: '%s'\n\n"
	if actually := pad.toLongMacro(); m.Long != "" && m.Long != actually {
		return fmt.Errorf(format, "LONG", m.Long, actually)
	}
	if actually := pad.toShortMacro(); m.Short != "" && m.Short != actually {
		return fmt.Errorf(format, "SHORT", m.Short, actually)
	}
	return nil
}

func (suite Suite) Run(t *testing.T, label string) {
	t.Run(label, func(t *testing.T) {
		for _, testcase := range suite {
			if err := testcase.Macro.check(testcase.Pad); err != nil {
				t.Errorf("Test failed for pad %s\n%v", testcase.Pad.ID, err)
			}
		}
	})
}
