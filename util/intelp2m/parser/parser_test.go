package parser_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser/test"
)

const TestLogFilePath = "./testlog/inteltool_test.log"

func TestParser(t *testing.T) {
	t.Run("PARSER/PARSE-INTELTOOL-FILE", func(t *testing.T) {

		p2m.Config.AutoCheck = false
		p2m.Config.Field = p2m.NoFlds
		p2m.Config.GenLevel = 1
		p2m.Config.InputPath = TestLogFilePath

		entries, err := parser.Run()
		if err != nil {
			t.Errorf("failed to run parser: %v", err)
		}

		for i := range test.Suite {
			if entries[i] != test.Suite[i] {
				t.Errorf("\nExpects:  '%v'\nActually: '%v'\n\n", test.Suite[i], entries[i])
			}
		}
	})
}
