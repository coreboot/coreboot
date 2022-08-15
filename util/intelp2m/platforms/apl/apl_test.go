package apl_test

import (
	"testing"

	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/apl"
	"review.coreboot.org/coreboot.git/util/intelp2m/platforms/test"
)

func TestGenMacro(t *testing.T) {
	apollolake := apl.PlatformSpecific{}
	test.Suite{
		{ /* GPIO_0 - GPIO */
			Pad: test.Pad{ID: "GPIO_0", DW0: 0x44000300, DW1: 0x1003d000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_HI_Z(GPIO_0, DN_20K, DEEP, IGNORE, SAME),",
				Long:  "_PAD_CFG_STRUCT(GPIO_0, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), PAD_PULL(DN_20K) | PAD_IOSSTATE(IGNORE)),",
			},
		},
		{ /* GPIO_15 - GPIO */
			Pad: test.Pad{ID: "GPIO_15", DW0: 0x44000000, DW1: 0x10001000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPIO_15, 0, DN_20K, DEEP, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPIO_15, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(DN_20K)),",
			},
		},
		{ /* GPIO_16 - GPIO */
			Pad: test.Pad{ID: "GPIO_16", DW0: 0x44000003, DW1: 0x10003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPIO_16, 1, UP_20K, DEEP, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPIO_16, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | (1 << 1) | 1, PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPIO_18 - GPIO */
			Pad: test.Pad{ID: "GPIO_18", DW0: 0x44000102, DW1: 0x10003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_OWN(GPIO_18, UP_20K, DEEP, OFF, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPIO_18, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPIO_21 - GPIO */
			Pad: test.Pad{ID: "GPIO_21", DW0: 0x44000102, DW1: 0x10027000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_IOSSTATE_OWN(GPIO_21, UP_20K, DEEP, OFF, TxDRxE, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPIO_21, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K) | PAD_IOSSTATE(TxDRxE)),",
			},
		},
		{ /* GPIO_22 - GPIO */
			Pad: test.Pad{ID: "GPIO_22", DW0: 0x44800102, DW1: 0x10024100, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_IOS_OWN(GPIO_22, NONE, DEEP, OFF, TxDRxE, DISPUPD, DRIVER),",
				Long:  "_PAD_CFG_STRUCT(GPIO_22, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_IOSSTATE(TxDRxE) | PAD_IOSTERM(DISPUPD) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPIO_25 - GPIO */
			Pad: test.Pad{ID: "GPIO_25", DW0: 0x40880102, DW1: 0x00027100, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SCI_IOS(GPIO_25, UP_20K, DEEP, LEVEL, INVERT, TxDRxE, DISPUPD),",
				Long:  "_PAD_CFG_STRUCT(GPIO_25, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K) | PAD_IOSSTATE(TxDRxE) | PAD_IOSTERM(DISPUPD)),",
			},
		},
		{ /* GPIO_26 - SATA_LEDN */
			Pad: test.Pad{ID: "GPIO_26", DW0: 0x44001400, DW1: 0x00003c00, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPIO_26, NATIVE, DEEP, NF5),",
				Long:  "_PAD_CFG_STRUCT(GPIO_26, PAD_FUNC(NF5) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(NATIVE) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPIO_27 - GPIO */
			Pad: test.Pad{ID: "GPIO_27", DW0: 0x42880102, DW1: 0x00024100, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_SCI_IOS(GPIO_27, NONE, DEEP, EDGE_SINGLE, INVERT, TxDRxE, DISPUPD),",
				Long:  "_PAD_CFG_STRUCT(GPIO_27, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(EDGE_SINGLE) | PAD_IRQ_ROUTE(SCI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_IOSSTATE(TxDRxE) | PAD_IOSTERM(DISPUPD)),",
			},
		},
		{ /* TDI - JTAG_TDI */
			Pad: test.Pad{ID: "TDI", DW0: 0x44000700, DW1: 0x00c00000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(TDI, NONE, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(TDI, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_RX_DISABLE), 0),",
			},
		},
		{ /* CNV_BRI_DT - GPIO */
			Pad: test.Pad{ID: "CNV_BRI_DT", DW0: 0x44000100, DW1: 0x1003d000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_IOSSTATE_OWN(CNV_BRI_DT, DN_20K, DEEP, OFF, IGNORE, ACPI),",
				Long:  "_PAD_CFG_STRUCT(CNV_BRI_DT, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(TX_DISABLE), PAD_PULL(DN_20K) | PAD_IOSSTATE(IGNORE)),",
			},
		},
		{ /* CNV_BRI_RSP - GPIO */
			Pad: test.Pad{ID: "CNV_BRI_RSP", DW0: 0x44000201, DW1: 0x10003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_TERM_GPO(CNV_BRI_RSP, 1, UP_20K, DEEP),",
				Long:  "_PAD_CFG_STRUCT(CNV_BRI_RSP, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | PAD_BUF(RX_DISABLE) | 1, PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPIO_188 - DDI0_DDC_SCL */
			Pad: test.Pad{ID: "GPIO_188", DW0: 0x44000400, DW1: 0x0003fc00, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSTANDBY_IGNORE(GPIO_188, NATIVE, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPIO_188, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(NATIVE) | PAD_IOSSTATE(IGNORE)),",
			},
		},
		{ /* GPIO_172 - SDCARD_CLK */
			Pad: test.Pad{ID: "GPIO_172", DW0: 0x44000400, DW1: 0x00021100, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSSTATE_IOSTERM(GPIO_172, DN_20K, DEEP, NF1, HIZCRx1, DISPUPD),",
				Long:  "_PAD_CFG_STRUCT(GPIO_172, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(DN_20K) | PAD_IOSSTATE(HIZCRx1) | PAD_IOSTERM(DISPUPD)),",
			},
		},
		{ /* GPIO_176 - SDCARD_D3 */
			Pad: test.Pad{ID: "GPIO_176", DW0: 0x44000400, DW1: 0x00021000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSSTATE(GPIO_176, DN_20K, DEEP, NF1, HIZCRx1),",
				Long:  "_PAD_CFG_STRUCT(GPIO_176, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(DN_20K) | PAD_IOSSTATE(HIZCRx1)),",
			},
		},
		{ /* GPIO_177 - GPIO */
			Pad: test.Pad{ID: "GPIO_177", DW0: 0x46000102, DW1: 0x10027000, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_GPI_TRIG_IOSSTATE_OWN(GPIO_177, UP_20K, DEEP, EDGE_BOTH, TxDRxE, DRIVER),",
				Long:  "_PAD_CFG_STRUCT(GPIO_177, PAD_FUNC(GPIO) | PAD_RESET(DEEP) | PAD_TRIG(EDGE_BOTH) | PAD_BUF(TX_DISABLE) | (1 << 1), PAD_PULL(UP_20K) | PAD_IOSSTATE(TxDRxE) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
		{ /* GPIO_186 - SDCARD_LVL_WP */
			Pad: test.Pad{ID: "GPIO_186", DW0: 0x44000402, DW1: 0x00003000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF(GPIO_186, UP_20K, DEEP, NF1),",
				Long:  "_PAD_CFG_STRUCT(GPIO_186, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF) | (1 << 1), PAD_PULL(UP_20K)),",
			},
		},
		{ /* GPIO_182 - EMMC_RCLK */
			Pad: test.Pad{ID: "GPIO_182", DW0: 0x44000400, DW1: 0x0001d000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSSTATE(GPIO_182, DN_20K, DEEP, NF1, HIZCRx0),",
				Long:  "_PAD_CFG_STRUCT(GPIO_182, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_PULL(DN_20K) | PAD_IOSSTATE(HIZCRx0)),",
			},
		},
		{ /* LPC_CLKOUT0 - LPC_CLKOUT0 */
			Pad: test.Pad{ID: "LPC_CLKOUT0", DW0: 0x44000400, DW1: 0x00020100, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSSTATE_IOSTERM(LPC_CLKOUT0, NONE, DEEP, NF1, HIZCRx1, DISPUPD),",
				Long:  "_PAD_CFG_STRUCT(LPC_CLKOUT0, PAD_FUNC(NF1) | PAD_RESET(DEEP) | PAD_TRIG(OFF), PAD_IOSSTATE(HIZCRx1) | PAD_IOSTERM(DISPUPD)),",
			},
		},
	}.Run(t, "INTEL-APOLLO-PCH/PAD-MAP", apollolake)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_1xx", DW0: 0xBFFFFFFF, DW1: 0xFFFFFFFF, Ownership: 1},
			Macro: test.Macro{
				Short: "PAD_CFG_NF_IOSSTATE_IOSTERM(GPP_1xx, NATIVE, PLTRST, NF7, IGNORE, ENPU),",
				Long:  "_PAD_CFG_STRUCT(GPP_1xx, PAD_FUNC(NF7) | PAD_RESET(PLTRST) | PAD_TRIG(EDGE_BOTH) | PAD_IRQ_ROUTE(IOAPIC) | PAD_IRQ_ROUTE(SCI) | PAD_IRQ_ROUTE(SMI) | PAD_IRQ_ROUTE(NMI) | PAD_RX_POL(INVERT) | PAD_BUF(TX_RX_DISABLE) | (1 << 29) | (1 << 28) | (1 << 1) | 1, PAD_CFG1_TOL_1V8PAD_PULL(NATIVE) | PAD_IOSSTATE(IGNORE) | PAD_IOSTERM(ENPU) | PAD_CFG_OWN_GPIO(DRIVER)),",
			},
		},
	}.Run(t, "INTEL-APOLLO-PCH/MASK", apollolake)

	test.Suite{
		{
			Pad: test.Pad{ID: "GPP_2xx", DW0: 0x00000000, DW1: 0x00000000, Ownership: 0},
			Macro: test.Macro{
				Short: "PAD_CFG_GPIO_BIDIRECT(GPP_2xx, 0, NONE, PWROK, LEVEL, ACPI),",
				Long:  "_PAD_CFG_STRUCT(GPP_2xx, PAD_FUNC(GPIO), 0),",
			},
		},
	}.Run(t, "INTEL-APOLLO-PCH/EMRTY", apollolake)
}
