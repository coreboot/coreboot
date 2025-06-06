package jsl_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/config/p2m"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/common"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/test"
)

func TestGenMacro(t *testing.T) {
	p2m.Config.Platform = p2m.Jasper
	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_A1", DW0: 0x11111111, DW1: 0x11111111, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_A1, DN_20K, PWROK, NF4),",
				Long:  "_PAD_CFG_STRUCT(GPP_A1, PAD_FUNC(NF4) | PAD_IRQ_ROUTE(IOAPIC) | PAD_BUF(TX_DISABLE) | (1 << 28) | 1, PAD_PULL(DN_20K) | PAD_IOSSTATE(Tx1RxDCRx1) | PAD_IOSTERM(DISPUPD) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_B2", DW0: 0x22222222, DW1: 0x22222222, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_TERM_GPO(GPP_B2, 0, INVALID, PWROK),",
				Long:  "_PAD_CFG_STRUCT(GPP_B2, PAD_FUNC(GPIO) | PAD_TRIG(EDGE_SINGLE) | PAD_IRQ_ROUTE(NMI) | PAD_BUF(RX_DISABLE) | (1 << 29) | (1 << 1), PAD_CFG1_TOL_1V8PAD_PULL(INVALID) | PAD_IOSSTATE(HIZCRx1) | PAD_IOSTERM(ENPD)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_C3", DW0: 0x44444444, DW1: 0x44444444, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_C3, INVALID, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_C3, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_IRQ_ROUTE(SMI), PAD_PULL(INVALID) | PAD_IOSSTATE(Tx0RxDCRx0) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_D4", DW0: 0x88888888, DW1: 0x88888888, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_D4, DN_5K, PLTRST, NF2),",
				Long:  "_PAD_CFG_STRUCT(GPP_D4, PAD_FUNC(NF2) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT), PAD_PULL(DN_5K) | PAD_IOSSTATE(Tx0RxDCRx1)),",
			},
		},
	}.Run(t, "INTEL-JASPER-LAKE-PCH/SLIDING-ONE-IN-NIBBLE-TEST")

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_E5", DW0: 0xEEEEEEEE, DW1: 0xEEEEEEEE, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_E5, UP_2K, RSMRST, NF3),",
				Long:  "_PAD_CFG_STRUCT(GPP_E5, PAD_FUNC(NF3) | PAD_RESET(RSMRST) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_RX_POL(INVERT) | PAD_BUF(RX_DISABLE) | (1 << 29) | (1 << 1), PAD_CFG1_TOL_1V8PAD_PULL(UP_2K) | PAD_IOSSTATE(ERROR) | PAD_IOSTERM(ENPD) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_F6", DW0: 0xDDDDDDDD, DW1: 0xDDDDDDDD, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_F6, INVALID, RSMRST, NF7),",
				Long:  "_PAD_CFG_STRUCT(GPP_F6, PAD_FUNC(NF7) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 28) | 1, PAD_PULL(INVALID) | PAD_IOSSTATE(HIZCRx0) | PAD_IOSTERM(DISPUPD)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_G7", DW0: 0xBBBBBBBB, DW1: 0xBBBBBBBB, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_G7, INVALID, PLTRST, NF6),",
				Long:  "_PAD_CFG_STRUCT(GPP_G7, PAD_FUNC(NF6) | PAD_RESET(PLTRST) | PAD_TRIG(EDGE_SINGLE) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(NMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(INVALID) | PAD_IOSSTATE(ERROR) | PAD_IOSTERM(ENPU) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_H8", DW0: 0x77777777, DW1: 0x77777777, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_H8, UP_667, DEEP, NF5),",
				Long:  "_PAD_CFG_STRUCT(GPP_H8, PAD_FUNC(NF5) | PAD_RESET(DEEP) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(UP_667) | PAD_IOSSTATE(ERROR) | PAD_IOSTERM(ENPU)),",
			},
		},
	}.Run(t, "INTEL-JASPER-LAKE-PCH/SLIDING-ZERO-IN-NIBBLE-TEST")

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_R9", DW0: 0x33333333, DW1: 0x33333333, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_R9, UP_20K, PWROK, NF4),",
				Long:  "_PAD_CFG_STRUCT(GPP_R9, PAD_FUNC(NF4) | PAD_TRIG(EDGE_SINGLE) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(NMI) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(UP_20K) | PAD_IOSSTATE(ERROR) | PAD_IOSTERM(ENPU) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_S10", DW0: 0x66666666, DW1: 0x66666666, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_S10, UP_1K, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_S10, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_BUF(RX_DISABLE) | (1 << 29) | (1 << 1), PAD_CFG1_TOL_1V8PAD_PULL(UP_1K) | PAD_IOSSTATE(TxDRxE) | PAD_IOSTERM(ENPD)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPP_T11", DW0: 0xCCCCCCCC, DW1: 0xCCCCCCCC, Ownership: common.Driver},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_T11, INVALID, RSMRST, NF3),",
				Long:  "_PAD_CFG_STRUCT(GPP_T11, PAD_FUNC(NF3) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_RX_POL(INVERT), PAD_PULL(INVALID) | PAD_IOSSTATE(Tx1RxDCRx0) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{
			Pad: test.Pad{ID: "GPD12", DW0: 0x99999999, DW1: 0x99999999, Ownership: common.Acpi},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPD12, INVALID, PLTRST, NF6),",
				Long:  "_PAD_CFG_STRUCT(GPD12, PAD_FUNC(NF6) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 28) | 1, PAD_PULL(INVALID) | PAD_IOSSTATE(Tx1RxE) | PAD_IOSTERM(DISPUPD)),",
			},
		},
	}.Run(t, "INTEL-JASPER-LAKE-PCH/SLIDING-ONE-ONE-IN-NIBBLE-TEST")
}
