/* SPDX-License-Identifier: GPL-2.0-only */

#include <variants.h>

/* Early pad configuration in bootblock */
const struct soc_amd_gpio early_gpio_table[] = {
	/* EGPIO26:	PCIE_RST0# asserted		*/
	PAD_NFO(GPIO_26, PCIE_RST_L, LOW),
	/* EGPIO27:	PCIE_RST1# asserted		*/
	PAD_NFO(GPIO_27, PCIE_RST1_L, LOW),
	/* EGPIO113:	SMB SCL 0			*/
	PAD_NF(GPIO_113, I2C2_SCL, PULL_NONE),
	/* EGPIO114:	SMB SDA 0			*/
	PAD_NF(GPIO_114, I2C2_SDA, PULL_NONE),
	/* EGPIO141:	UART0 RXD			*/
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* EGPIO143:	UART0 TXD			*/
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
};

const struct soc_amd_gpio *variant_early_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(early_gpio_table);
	return early_gpio_table;
}

/*
 * Romstage pad configuration.
 *
 * Keep this table limited to pads backed by the board schematic or by obvious
 * SoC-native requirements.
 */
const struct soc_amd_gpio gpio_table[] = {
	/* AGPIO0:	PWR_BTN_L			*/
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_NONE),
	/* AGPIO1:	SYS_RESET_L			*/
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_NONE),
	/* AGPIO2:	PCIE_WAKE_N			*/
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_NONE, EDGE_LOW),
	/* AGPIO3:	EC_SMI#_APU			*/
	PAD_GPI(GPIO_3, PULL_NONE),
	/* AGPIO4:	SSD_DET				*/
	PAD_GPI(GPIO_4, PULL_NONE),
	/* AGPIO5:	HDD_DEVSLP			*/
	PAD_NF(GPIO_5, DEVSLP0, PULL_DOWN),
	/* AGPIO6:	M2_SSD_DEVSLP			*/
	PAD_NF(GPIO_6, DEVSLP1, PULL_DOWN),
	/* AGPIO7:	Not Connected			*/
	PAD_NC(GPIO_7),
	/* AGPIO8:	Not Connected			*/
	PAD_NC(GPIO_8),
	/* AGPIO9:	BIOS_S0_EC			*/
	PAD_GPO(GPIO_9, HIGH),
	/* AGPIO10:	S0A3				*/
	PAD_NF(GPIO_10, S0A3, PULL_NONE),
	/* AGPIO11:	BLINK				*/
	PAD_NF(GPIO_11, BLINK, PULL_NONE),
	/* AGPIO12:	LLB_L				*/
	PAD_NF(GPIO_12, LLB_L, PULL_NONE),
	/* AGPIO16:	USB OC0				*/
	PAD_NF(GPIO_16, USB_OC0_L, PULL_NONE),
	/* AGPIO17:	Unused				*/
	PAD_NC(GPIO_17),
	/* AGPIO18:	WLAN_WAKE_N			*/
	PAD_GPI(GPIO_18, PULL_UP),
	/* AGPIO19:	I2C 3 SCL			*/
	PAD_NF(GPIO_19, I2C3_SCL, PULL_NONE),
	/* AGPIO20:	I2C 3 SDA			*/
	PAD_NF(GPIO_20, I2C3_SDA, PULL_NONE),
	/* AGPIO21:	LPC_PD_L			*/
	PAD_NF(GPIO_21, LPC_PD_L, PULL_NONE),
	/* AGPIO22:	EC_SCI#_APU / LPC_PME_L		*/
	PAD_NF_SCI(GPIO_22, LPC_PME_L, PULL_NONE, EDGE_LOW),
	/* AGPIO23:	AC_PRESENT			*/
	PAD_NF(GPIO_23, AC_PRES, PULL_UP),
	/* AGPIO24:	LAN_WAKE_N			*/
	PAD_GPI(GPIO_24, PULL_UP),
	/* AGPIO26:	PCIE_RST0#			*/
	PAD_NFO(GPIO_26, PCIE_RST_L, HIGH),
	/* AGPIO27:	PCIE_RST1#			*/
	PAD_NFO(GPIO_27, PCIE_RST1_L, HIGH),
	/* AGPIO29:	SPI0 TPM 1 CS2			*/
	/* AGPIO30:	Not Connected			*/
	/* AGPIO31:	Unused				*/
	PAD_NC(GPIO_31),
	/* AGPIO32:	LPC_RST				*/
	PAD_NF(GPIO_32, LPC_RST_L, PULL_NONE),
	/* AGPIO40:	M.2_SSD_PEDET_R			*/
	PAD_GPI(GPIO_40, PULL_NONE),
	/* AGPIO41:	Unavailable			*/
	/* EGPIO42:	Not Connected			*/
	PAD_NC(GPIO_42),
	/* EGPIO67:	SPI_ROM_REQ			*/
	PAD_NF(GPIO_67, SPI_ROM_REQ, PULL_NONE),
	/* AGPIO68:	Test Point 90			*/
	/* EGPIO69:	BT_RF_KILL_N			*/
	PAD_GPO(GPIO_69, HIGH),
	/* EGPIO70:	Test Point 89			*/
	/* EGPIO74:	LPCCLK0				*/
	PAD_NF(GPIO_74, LPCCLK0, PULL_NONE),
	/* EGPIO75:	LPCCLK1				*/
	PAD_NF(GPIO_75, LPCCLK1, PULL_NONE),
	/* EGPIO76:	SPI_ROM_GNT			*/
	PAD_NF(GPIO_76, SPI_ROM_GNT, PULL_NONE),
	/* AGPIO84:	Unused				*/
	PAD_NC(GPIO_84),
	/* AGPIO85:	Unused				*/
	PAD_NC(GPIO_85),
	/* AGPIO86:	EC_SMI_N			*/
	PAD_NF(GPIO_86, LPC_SMI_L, PULL_UP),
	/* AGPIO87:	LPC Serial Interrupt Request	*/
	PAD_NF(GPIO_87, SERIRQ, PULL_NONE),
	/* AGPIO88:	LPC Clock Run			*/
	PAD_NF(GPIO_88, LPC_CLKRUN_L, PULL_NONE),
	/* AGPIO89:	Unused GPIO output		*/
	PAD_GPO(GPIO_89, LOW),
	/* AGPIO90:	Unused GPIO output		*/
	PAD_GPO(GPIO_90, HIGH),
	/* AGPIO91:	WIFI_DISABLE_N			*/
	PAD_GPO(GPIO_91, HIGH),
	/* AGPIO92:	Unused GPIO output		*/
	PAD_GPO(GPIO_92, HIGH),
	/* EGPIO104:	LAD 0				*/
	PAD_NF(GPIO_104, LAD0, PULL_NONE),
	/* EGPIO105:	LAD 1				*/
	PAD_NF(GPIO_105, LAD1, PULL_NONE),
	/* EGPIO106:	LAD 2				*/
	PAD_NF(GPIO_106, LAD2, PULL_NONE),
	/* EGPIO107:	LAD 3				*/
	PAD_NF(GPIO_107, LAD3, PULL_NONE),
	/* EGPIO108:	ESPI Alert			*/
	PAD_NF(GPIO_108, ESPI_ALERT_D1, PULL_UP),
	/* EGPIO109:	LFRAME				*/
	PAD_NF(GPIO_109, LFRAME_L, PULL_NONE),
	/* AGPIO115:	Clock Request 1			*/
	PAD_NF(GPIO_115, CLK_REQ1_L, PULL_UP),
	/* AGPIO116:	Clock Request 2 (Not Used)	*/
	PAD_NC(GPIO_116),
	/* EGPIO120:	Clock Request 5			*/
	PAD_NF(GPIO_120, CLK_REQ5_L, PULL_UP),
	/* EGPIO121:	Clock Request 6			*/
	PAD_NF(GPIO_121, CLK_REQ6_L, PULL_UP),
	/* AGPIO129:	ESPI_RESET_L / KBRST_L		*/
	PAD_NF(GPIO_129, KBRST_L, PULL_UP),
	/* AGPIO130:	SATA_ACT_L			*/
	PAD_NF(GPIO_130, SATA_ACT_L, PULL_UP),
	/* EGPIO131:	Clock Request 3 (Not Used)	*/
	PAD_NC(GPIO_131),
	/* EGPIO132:	Clock Request 4			*/
	PAD_NF(GPIO_132, CLK_REQ4_L, PULL_UP),
	/* EGPIO140:	Not Connected			*/
	PAD_NC(GPIO_140),
	/* EGPIO141:	UART0 RXD			*/
	PAD_NF(GPIO_141, UART0_RXD, PULL_NONE),
	/* EGPIO142:	Test Point 96			*/
	PAD_NC(GPIO_142),
	/* EGPIO143:	UART0 TXD			*/
	PAD_NF(GPIO_143, UART0_TXD, PULL_NONE),
	/* EGPIO144:	Not Connected			*/
	PAD_NC(GPIO_144),
	/* EGPIO145:	I2C 0 SCL (Not Used)		*/
	/* EGPIO146:	I2C 0 SDA (Not Used)		*/
	/* EGPIO147:	I2C 1 SCL (Not Used)		*/
	/* EGPIO147:	I2C 1 SDA (Not Used)		*/
};

const struct soc_amd_gpio *variant_gpio_table(size_t *num)
{
	*num = ARRAY_SIZE(gpio_table);
	return gpio_table;
}
