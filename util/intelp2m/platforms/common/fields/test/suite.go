package test

import (
	"fmt"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common/fields"
)

type TestCase struct {
	DW0, DW1  uint32
	Ownership uint8
	Reference string
}

func (tc TestCase) Check(actuallyMacro string) error {
	if actuallyMacro != tc.Reference {
		return fmt.Errorf(`TestCase: DW0 = 0x%x, DW1 = 0x%x, Ownership = %d:
Expects:  '%s'
Actually: '%s'`, tc.DW0, tc.DW1, tc.Ownership, tc.Reference, actuallyMacro)
	}
	return nil
}

type Suite []TestCase

func (suite Suite) Run(t *testing.T, label string) {
	t.Run(label, func(t *testing.T) {
		for _, tc := range suite {
			constructor, err := platforms.GetConstructor()
			if err != nil {
				panic(err)
			}
			macro := common.CreateFrom(
				"",
				tc.Ownership == 1,
				constructor(tc.DW0, tc.DW1),
				fields.Get(),
			)
			if err := tc.Check(macro.Fields.GenerateMacro(&macro).String()); err != nil {
				t.Errorf("Test failed: %v", err)
			}
		}
	})
}

func SlidingOneTestSuiteCreate(referenceSlice []string) Suite {
	suite := make([]TestCase, len(referenceSlice))
	dw := uint32(0x80000000)
	for i, reference := range referenceSlice {
		suite[i] = TestCase{DW0: dw >> i, DW1: dw >> i, Reference: reference}
	}
	return suite
}
