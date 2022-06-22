package snr_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/snr"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/test"
)

func TestGenMacro(t *testing.T) {
	sunrise := snr.PlatformSpecific{}
	test.Suite{
		{ /* GPP_A1 - LAD0  */
			Pad: test.Pad{ID: "GPP_A1", DW0: 0x84000402, DW1: 0x00003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_A1, UP_20K, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_A1, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | (1 << 1), PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPP_A5 - LFRAME# */
			Pad: test.Pad{ID: "GPP_A5", DW0: 0x84000600, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_A5, NONE, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_A5, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPP_A22 - GPIO */
			Pad: test.Pad{ID: "GPP_A22", DW0: 0x84000102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_A22, NONE, PLTRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_A22, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_B3 - GPIO */
			Pad: test.Pad{ID: "GPP_B3", DW0: 0x44000201, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPP_B3, 1, DEEP),",
				Long:  "_PAD_CFG_STRUCT(GPP_B3, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1, 0),",
			},
		},
		{ /* GPP_B5 - GPIO */
			Pad: test.Pad{ID: "GPP_B5", DW0: 0x84000100, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_B5, NONE, PLTRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_B5, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE), 0),",
			},
		},
		{ /* GPP_B8 - NC */
			Pad: test.Pad{ID: "GPP_B8", DW0: 0x44000300, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_NC(GPP_B8, NONE),",
				Long:  "_PAD_CFG_STRUCT(GPP_B8, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), 0),",
			},
		},
		{ /* GPP_C2 - GPIO */
			Pad: test.Pad{ID: "GPP_C2", DW0: 0x44000201, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPP_C2, 1, DEEP),",
				Long:  "_PAD_CFG_STRUCT(GPP_C2, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1, 0),",
			},
		},
		{ /* GPP_C10 - GPIO */
			Pad: test.Pad{ID: "GPP_C10", DW0: 0x04000000, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPP_C10, 0, NONE, RSMRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_C10, PAD_FUNC(GPIO) | PAD_RESET(RSMRST) | PAD_TRIG(OFF), 0),",
			},
		},
		{ /* GPP_C22 - UART2_RTS#  */
			Pad: test.Pad{ID: "GPP_C22", DW0: 0x84000600, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_C22, NONE, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_C22, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPP_C23 - GPIO */
			Pad: test.Pad{ID: "GPP_C23", DW0: 0x40880102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SCI(GPP_C23, NONE, DEEP, LEVEL, INVERT),",
				Long:  "_PAD_CFG_STRUCT(GPP_C23, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_D0 - GPIO */
			Pad: test.Pad{ID: "GPP_D0", DW0: 0x84000200, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPP_D0, 0, PLTRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_D0, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
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
			Pad: test.Pad{ID: "GPP_E0", DW0: 0x84000502, DW1: 0x00003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_E0, UP_20K, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_E0, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPP_E7 - GPIO */
			Pad: test.Pad{ID: "GPP_E7", DW0: 0x84000102, DW1: 0x00000000, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPP_E7, NONE, PLTRST, OFF, DRIVER),",
				Long:  "_PAD_CFG_STRUCT(GPP_E7, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_CFG_OWN_GPIO(DRIVER)),",
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
		{ /* GPP_F14 - GPIO */
			Pad: test.Pad{ID: "GPP_F14", DW0: 0x40900102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_APIC_LOW(GPP_F14, NONE, DEEP),",
				Long:  "_PAD_CFG_STRUCT(GPP_F14, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_IRQ_ROUTE(IOAPIC) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_H13 - GPIO */
			Pad: test.Pad{ID: "GPP_H13", DW0: 0x80100102, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_APIC_HIGH(GPP_H13, NONE, PLTRST),",
				Long:  "_PAD_CFG_STRUCT(GPP_H13, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_IRQ_ROUTE(IOAPIC) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPD1 - GPIO */
			Pad: test.Pad{ID: "GPD1", DW0: 0x04000200, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPO(GPD1, 0, PWROK),",
				Long:  "_PAD_CFG_STRUCT(GPD1, PAD_FUNC(GPIO) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE), 0),",
			},
		},
		{ /* GPD2 - LAN_WAKE# */
			Pad: test.Pad{ID: "GPD2", DW0: 0x00080602, DW1: 0x00003c00, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPD2, NATIVE, PWROK, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPD2, PAD_FUNC(NF1) | PAD_IRQ_ROUTE(SCI) | PAD_BUF(RX_DISABLE) | (1 << 1), PAD_PULL(NATIVE)),",
			},
		},
		{ /* GPD3 - PWRBTN# */
			Pad: test.Pad{ID: "GPD3", DW0: 0x04000502, DW1: 0x00003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPD3, UP_20K, PWROK, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPD3, PAD_FUNC(NF1) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPD7 - GPIO */
			Pad: test.Pad{ID: "GPD7", DW0: 0x84000103, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPD7, NONE, PLTRST, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPD7, PAD_FUNC(GPIO) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1) | 1, 0),",
			},
		},
		{ /* GPP_I1 - DDPC_HPD1 */
			Pad: test.Pad{ID: "GPP_I1", DW0: 0x84000502, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_I1, NONE, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_I1, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), 0),",
			},
		},
		{ /* GPP_I8 - DDPC_CTRLDATA */
			Pad: test.Pad{ID: "GPP_I8", DW0: 0x84000500, DW1: 0x00001000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPP_I8, DN_20K, PLTRST, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPP_I8, PAD_FUNC(NF1) | PAD_RESET(PLTRST) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE), PAD_PULL(DN_20K)),",
			},
		},
	}.Run(t, "INTEL-SUNRISE-PCH/PAD-MAP", sunrise)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_Axx", DW0: 0xBFFFFFFF, DW1: 0xFFFFFFFF, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_1V8(GPP_Axx, NATIVE, PLTRST, NF7),",
				Long:  "_PAD_CFG_STRUCT(GPP_Axx, PAD_FUNC(NF7) | PAD_RESET(PLTRST) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(NATIVE) | PAD_IOSSTATE(IGNORE) | PAD_IOSTERM(ENPU) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
	}.Run(t, "INTEL-SUNRISE-PCH/MASK", sunrise)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_Bxx", DW0: 0x00000000, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPP_Bxx, 0, NONE, RSMRST, LEVEL, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_Bxx, PAD_FUNC(GPIO) | PAD_RESET(RSMRST), 0),",
			},
		},
	}.Run(t, "INTEL-SUNRISE-PCH/EMRTY", sunrise)
}
