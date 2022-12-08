package generator_test

import (
	"strings"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/generator"
	"review.coreboot.org/coreboot.git/util/intelp2m/generator/testsuites"
	parsertest "review.coreboot.org/coreboot.git/util/intelp2m/parser/test"
)

func do(t *testing.T, reference string) {
	actually, err := generator.Run(parsertest.Suite)
	if err != nil {
		t.Errorf("failed to run generator: %v", err)
	}

	expects := strings.SplitAfter(reference, "\n")
	for i := range actually {
		if expects[i] != actually[i] {
			t.Errorf("row number %d:\n\tExpects: <%s>\n\tActually <%s>",
				i, expects[i], actually[i])
		}
	}
}

func TestGenerator(t *testing.T) {
	t.Run("GENERATOR/I0-NO-COMMENTS", func(t *testing.T) {
		p2m.SettingsReset()
		p2m.Config.AutoCheck = false
		do(t, testsuites.ReferenceI0NoComments)
	})

	t.Run("GENERATOR/I1-WITH-COMMENTS", func(t *testing.T) {
		p2m.SettingsReset()
		p2m.Config.AutoCheck = false
		p2m.Config.GenLevel = 1
		do(t, testsuites.ReferenceI1Comments)
	})

	t.Run("GENERATOR/I2-AUTO-CHECK", func(t *testing.T) {
		p2m.SettingsReset()
		p2m.Config.GenLevel = 2
		do(t, testsuites.ReferenceI2AutoCheck)
	})

	t.Run("GENERATOR/I3-UNCHECK", func(t *testing.T) {
		p2m.SettingsReset()
		p2m.Config.AutoCheck = false
		p2m.Config.IgnoredFields = true
		p2m.Config.GenLevel = 3
		do(t, testsuites.ReferenceI3Uncheck)
	})

	t.Run("GENERATOR/I4-EXCLUDE-UNUSED-CB-FIELDS", func(t *testing.T) {
		p2m.SettingsReset()
		p2m.Config.IgnoredFields = true
		p2m.Config.Field = p2m.CbFlds
		p2m.Config.GenLevel = 4
		do(t, testsuites.ReferenceI4ExcludeUnusedCbFlds)
	})
}
