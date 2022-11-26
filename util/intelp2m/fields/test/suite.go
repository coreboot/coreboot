package test

import (
	"fmt"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
)

type TestCase struct {
	DW0, DW1  uint32
	Ownership uint8
	Reference string
}

func (tc TestCase) Check(actuallyMacro string) error {
	if actuallyMacro != tc.Reference {
		return fmt.Errorf(`TestCase: DW0 = %d, DW1 = %d, Ownership = %d:
Expects:  '%s'
Actually: '%s'`, tc.DW0, tc.DW1, tc.Ownership, tc.Reference, actuallyMacro)
	}
	return nil
}

type Suite []TestCase

func (suite Suite) Run(t *testing.T, label string, decoderIf common.Fields) {
	t.Run(label, func(t *testing.T) {
		platform := snr.PlatformSpecific{}
		macro := common.GetInstanceMacro(platform, decoderIf)
		dw0 := macro.GetRegisterDW0()
		dw1 := macro.GetRegisterDW1()
		for _, tc := range suite {
			macro.Clear()
			macro.PadIdSet("").SetPadOwnership(tc.Ownership)
			dw0.Value = tc.DW0
			dw1.Value = tc.DW1
			macro.Fields.GenerateString()
			if err := tc.Check(macro.Get()); err != nil {
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
