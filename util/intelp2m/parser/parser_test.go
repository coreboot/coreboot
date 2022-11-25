package parser_test

import (
	"fmt"
	"os"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

const testLogFilePath = "test/inteltool_test.log"

type Printer struct {
	lines []string
}

func (p *Printer) Linef(lvl int, format string, args ...interface{}) {
	if p2m.Config.GenLevel >= lvl {
		p.lines = append(p.lines, fmt.Sprintf(format, args...))
	}
}

func (p *Printer) Line(lvl int, str string) {
	if p2m.Config.GenLevel >= lvl {
		p.lines = append(p.lines, str)
	}
}

func TestParser(t *testing.T) {
	t.Run("PARSER/PARSE-INTELTOOL-FILE", func(t *testing.T) {
		var err error
		reference := []string{
			"\n\t/* ------- GPIO Community 0 ------- */\n",
			"\n\t/* ------- GPIO Group GPP_A ------- */\n",
			"\tPAD_CFG_NF(GPP_A0, NONE, PLTRST, NF1),", "\t/* RCIN# */", "\n",
			"\tPAD_CFG_NF(GPP_A1, UP_20K, PLTRST, NF1),", "\t/* LAD0 */", "\n",
			"\tPAD_CFG_NF(GPP_A5, NONE, PLTRST, NF1),", "\t/* LFRAME# */", "\n",
			"\tPAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),", "\t/* SUSWARN#/SUSPWRDNACK */", "\n",
			"\tPAD_CFG_GPI_TRIG_OWN(GPP_A23, NONE, PLTRST, OFF, ACPI),", "\t/* GPIO */", "\n",
			"\n\t/* ------- GPIO Group GPP_B ------- */\n",
			"\t/* GPP_C1 - RESERVED */\n",
			"\tPAD_CFG_GPI_TRIG_OWN(GPP_B0, NONE, PLTRST, OFF, ACPI),", "\t/* GPIO */", "\n",
			"\tPAD_CFG_NF(GPP_B23, DN_20K, PLTRST, NF2),", "\t/* PCHHOT# */", "\n",
			"\n\t/* ------- GPIO Community 1 ------- */\n",
			"\n\t/* ------- GPIO Group GPP_C ------- */\n",
			"\tPAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),", "\t/* SMBCLK */", "\n",
			"\tPAD_CFG_GPI_TRIG_OWN(GPP_C5, NONE, PLTRST, OFF, ACPI),", "\t/* GPIO */", "\n",
			"\t/* GPP_C6 - RESERVED */\n",
			"\t/* GPP_C7 - RESERVED */\n",
			"\tPAD_CFG_NF(GPP_C22, NONE, PLTRST, NF1),", "\t/* UART2_RTS# */", "\n",
			"\n\t/* ------- GPIO Group GPP_D ------- */\n",
			"\n\t/* ------- GPIO Group GPP_E ------- */\n",
			"\tPAD_CFG_NF(GPP_E0, UP_20K, PLTRST, NF1),", "\t/* SATAXPCIE0 */", "\n",
			"\n\t/* ------- GPIO Group GPP_G ------- */\n",
			"\tPAD_CFG_NF(GPP_G19, NONE, PLTRST, NF1),", "\t/* SMI# */", "\n",
			"\n\t/* ------- GPIO Community 2 ------- */\n",
			"\n\t/* -------- GPIO Group GPD -------- */\n",
			"\tPAD_CFG_NF(GPD9, NONE, PWROK, NF1),", "\t/* SLP_WLAN# */", "\n",
			"\n\t/* ------- GPIO Community 3 ------- */\n",
			"\n\t/* ------- GPIO Group GPP_I ------- */\n",
			"\tPAD_CFG_NF(GPP_I0, NONE, PLTRST, NF1),", "\t/* DDPB_HPD0 */", "\n",
			"\tPAD_CFG_NF(GPP_I1, NONE, PLTRST, NF1),", "\t/* DDPC_HPD1 */", "\n",
			"\tPAD_CFG_NF(GPP_I2, NONE, PLTRST, NF1),", "\t/* DDPD_HPD2 */", "\n",
		}
		if p2m.Config.InputFile, err = os.Open(testLogFilePath); err != nil {
			t.Errorf("Something is wrong with the test file - %s!\n", testLogFilePath)
			os.Exit(1)
		}
		defer p2m.Config.InputFile.Close()

		p2m.Config.AutoCheck = false
		p2m.Config.Field = p2m.NoFlds
		p2m.Config.GenLevel = 1

		prs := parser.ParserData{}
		prs.Parse()

		printer := Printer{lines: make([]string, 0)}
		generator := parser.Generator{
			PrinterIf: &printer,
			Data:      &prs,
		}

		if err := generator.Run(); err != nil {
			t.Errorf("Generator: %v", err)
			os.Exit(1)
		}

		if len(printer.lines) == len(reference) {
			for i := range printer.lines {
				if printer.lines[i] != reference[i] {
					t.Errorf("\nExpects:  '%s'\nActually: '%s'\n\n", reference[i], printer.lines[i])
					return
				}
			}
		} else {
			t.Errorf("%d does not match the reference slice len - %d!",
				len(reference), len(printer.lines))
		}
	})
}
