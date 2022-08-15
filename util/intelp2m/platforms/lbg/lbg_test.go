package lbg_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/lbg"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/test"
)

func TestGenMacro(t *testing.T) {
	lewisburg := lbg.PlatformSpecific{InheritanceMacro: snr.PlatformSpecific{}}
	test.Suite{
		{ /* GPP_A1 - ESPI_ALERT1#  */
			Pad: test.Pad{ID: "GPP_A1", DW0: 0x44000c00, DW1: 0x00003000, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_A1, UP_20K, DEEP, NF3),",
				Long:  "_PAD_CFG_STRUCT(GPP_A1, PAD_FUNC(NF3) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(UP_20K) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPP_A12 - LFRAME# */
			Pad: test.Pad{ID: "GPP_A12", DW0: 0x80880102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SCI(GPP_A12, NONE, PLTRST, LEVEL, INVERT),",
				Long:  "_PAD_CFG_STRUCT(GPP_A12, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_A16 - GPIO */
			Pad: test.Pad{ID: "GPP_A16", DW0: 0x44000201, DW1: 0x00000000, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO_GPIO_DRIVER(GPP_A16, 1, DEEP, NONE),",
				Long:  "_PAD_CFG_STRUCT(GPP_A16, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1, PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPP_A20 - GPIO */
			Pad: test.Pad{ID: "GPP_A20", DW0: 0x04000100, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_A20, NONE, RSMRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_A20, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE), 0),",
			},
		},
		{ /* GPP_B10 - GPIO */
			Pad: test.Pad{ID: "GPP_B10", DW0: 0x04000102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_B10, NONE, RSMRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_B10, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_B20 - GPIO */
			Pad: test.Pad{ID: "GPP_B20", DW0: 0x04000200, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPP_B20, 0, RSMRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_B20, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPP_B23 - PCHHOT# */
			Pad: test.Pad{ID: "GPP_B23", DW0: 0x04000a00, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_B23, NONE, RSMRST, NF2),",
				Long:  "_PAD_CFG_STRUCT(GPP_B23, PAD_FUNC(NF2) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPP_F0 - SATAXPCIE3 */
			Pad: test.Pad{ID: "GPP_F0", DW0: 0x04000502, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_F0, NONE, RSMRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_F0, PAD_FUNC(NF1) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_C10 - GPIO */
			Pad: test.Pad{ID: "GPP_C10", DW0: 0x04000000, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPP_C10, 0, NONE, RSMRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_C10, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF), 0),",
			},
		},
		{ /* GPP_C23 - GPIO */
			Pad: test.Pad{ID: "GPP_C23", DW0: 0x40880102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SCI(GPP_C23, NONE, DEEP, LEVEL, INVERT),",
				Long:  "_PAD_CFG_STRUCT(GPP_C23, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_D1 - GPIO */
			Pad: test.Pad{ID: "GPP_D1", DW0: 0x04000200, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPP_D1, 0, RSMRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_D1, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPP_D16 - GPIO */
			Pad: test.Pad{ID: "GPP_D16", DW0: 0x84000100, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_D16, NONE, PLTRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_D16, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE), 0),",
			},
		},
		{ /* GPP_E0 - SATAXPCIE0 */
			Pad: test.Pad{ID: "GPP_E0", DW0: 0x04000502, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_E0, NONE, RSMRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_E0, PAD_FUNC(NF1) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_E7 - GPIO */
			Pad: test.Pad{ID: "GPP_E7", DW0: 0x40840102, DW1: 0x00000000, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SMI(GPP_E7, NONE, DEEP, LEVEL, INVERT),",
				Long:  "_PAD_CFG_STRUCT(GPP_E7, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_IRQ_ROUTE(SMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPP_F2 - GPIO */
			Pad: test.Pad{ID: "GPP_F2", DW0: 0x44000300, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_NC(GPP_F2, NONE),",
				Long:  "_PAD_CFG_STRUCT(GPP_F2, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), 0),",
			},
		},
		{ /* GPP_F12 - GPIO */
			Pad: test.Pad{ID: "GPP_F12", DW0: 0x80900102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_APIC_LOW(GPP_F12, NONE, PLTRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_F12, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(IOAPIC) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_F13 - GPIO */
			Pad: test.Pad{ID: "GPP_F13", DW0: 0x80100102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_APIC_HIGH(GPP_F13, NONE, PLTRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_F13, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(IOAPIC) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_H10 - NC */
			Pad: test.Pad{ID: "GPP_H10", DW0: 0x44000300, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_NC(GPP_H10, NONE),",
				Long:  "_PAD_CFG_STRUCT(GPP_H10, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), 0),",
			},
		},
		{ /* GPP_L1 - CSME_INTR_OUT */
			Pad: test.Pad{ID: "GPP_L1", DW0: 0x44000700, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_L1, NONE, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_L1, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), 0),",
			},
		},
		{ /* GPP_L19 - TESTCH1_CLK */
			Pad: test.Pad{ID: "GPP_L19", DW0: 0x04000600, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_L19, NONE, RSMRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_L19, PAD_FUNC(NF1) | PAD_RESET(RSMRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
	}.Run(t, "INTEL-LEWISBURG-PCH/PAD-MAP", lewisburg)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_Axx", DW0: 0xBFFFFFFF, DW1: 0xFFFFFFFF, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_Axx, NATIVE, PLTRST, NF7),",
				Long:  "_PAD_CFG_STRUCT(GPP_Axx, PAD_FUNC(NF7) | PAD_RESET(PLTRST) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(NATIVE) | PAD_IOSSTATE(IGNORE) | PAD_IOSTERM(ENPU) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
	}.Run(t, "INTEL-LEWISBURG-PCH/MASK", lewisburg)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_Bxx", DW0: 0x00000000, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPP_Bxx, 0, NONE, RSMRST, LEVEL, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_Bxx, PAD_FUNC(GPIO) | PAD_RESET(RSMRST), 0),",
			},
		},
	}.Run(t, "INTEL-LEWISBURG-PCH/EMRTY", lewisburg)
}
