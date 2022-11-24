package parser_test

import (
	"fmt"
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/parser"
)

func TestTemp(t *testing.T) {
	t.Run("TEMPLATE/INTELTOOL-LINE", func(t *testing.T) {
		const (
			ref_fn  string = "SLP_S0#"
			ref_id  string = "GPP_B12"
			ref_dw0 uint32 = 0x44000600
			ref_dw1 uint32 = 0x0000003c
		)
		var (
			fn, id   string
			dw0, dw1 uint32
		)
		line := fmt.Sprintf("0x0520: 0x%08x%08x %s %s", ref_dw1, ref_dw0, ref_id, ref_fn)
		_ = parser.UseTemplate(line, &fn, &id, &dw0, &dw1)
		if fn != ref_fn {
			t.Errorf("function from '%s':\nExpects:  '%s'\nActually: '%s'\n\n",
				line, ref_fn, fn)
		} else if id != ref_id {
			t.Errorf("id from '%s':\nExpects:  '%s'\nActually: '%s'\n\n",
				line, ref_id, id)
		} else if dw0 != ref_dw0 {
			t.Errorf("dw0 from '%s':\nExpects:  '0x%08x'\nActually: '0x%08x'\n\n",
				line, ref_dw0, dw0)
		} else if dw1 != (ref_dw1 & parser.INTSEL_MASK) {
			t.Errorf("dw1 from '%s':\nExpects:  '0x%08x'\nActually: '0x%08x'\n\n",
				line, (ref_dw1 & parser.INTSEL_MASK), dw1)
		}
	})
}
