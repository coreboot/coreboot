/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/gpio.h>
#include <soc/southbridge.h>
#include <variant/gpio.h>

/*
 * As a rule of thumb, GPIO pins used by coreboot should be initialized at
 * bootblock while GPIO pins used only by the OS should be initialized at
 * ramstage.
 */
static const struct soc_amd_gpio gpio_set_stage_reset[] = {
	/* GPIO_4 - EN_PP3300_WLAN */
	PAD_GPO(GPIO_4, HIGH),

	/* GPIO_6 - APU_RST_L / EC_SMI_ODL, SMI gets configured in ramstage */
	PAD_GPI(GPIO_6, PULL_UP),

	/* GPIO_9 - H1_PCH_INT_ODL */
	PAD_INT(GPIO_9, PULL_UP, EDGE_LOW, STATUS),

	/* GPIO_15 - EC_IN_RW_OD */
	PAD_GPI(GPIO_15, PULL_UP),

	/* GPIO_22 - EC_SCI_ODL, SCI gets configured in ramstage */
	PAD_GPI(GPIO_22, PULL_UP),

	/* GPIO_24 - EC_PCH_WAKE_L, SCI gets configured in ramstage */
	PAD_GPI(GPIO_24, PULL_UP),

	/* GPIO_26 - APU_PCIE_RST_L */
	PAD_NF(GPIO_26, PCIE_RST_L, PULL_NONE),

	/* GPIO_40 - EMMC_BRIDGE_RST */
	PAD_GPO(GPIO_40, LOW),

	/* GPIO_74 - LPC_CLK0_EC_R */
	PAD_NF(GPIO_74, LPCCLK0, PULL_DOWN),

	/* GPIO_92 - WLAN_PCIE_CLKREQ_3V3_ODL */
	PAD_NF(GPIO_92, CLK_REQ0_L, PULL_UP),

	/* GPIO_122 - APU_BIOS_FLASH_WP_L */
	PAD_GPI(GPIO_122, PULL_NONE),

	/* GPIO_131 - CONFIG_STRAP3 */
	PAD_GPI(GPIO_131, PULL_NONE),

	/* GPIO_132 - CONFIG_STRAP4 */
	PAD_GPI(GPIO_132, PULL_NONE),

	/* GPIO_136 - UART_PCH_RX_DEBUG_TX */
	PAD_NF(GPIO_136, UART0_RXD, PULL_NONE),

	/* GPIO_138 - UART_PCH_TX_DEBUG_RX */
	PAD_NF(GPIO_138, UART0_TXD, PULL_NONE),

	/* GPIO_139 - CONFIG_STRAP1 */
	PAD_GPI(GPIO_139, PULL_NONE),

	/* GPIO_142 - CONFIG_STRAP2 */
	PAD_GPI(GPIO_142, PULL_NONE),
};

static const struct soc_amd_gpio gpio_wlan_rst_early_reset[] = {
	/* GPIO_70 - WLAN_PE_RST_L */
	PAD_GPO(GPIO_70, HIGH),
};

static const struct soc_amd_gpio gpio_set_stage_rom[] = {
	/* GPIO_133 - APU_EDP_BKLTEN_L (backlight - Active LOW) */
	PAD_GPO(GPIO_133, HIGH),
};

static const struct soc_amd_gpio gpio_set_stage_ram[] = {
	/* GPIO_0 - EC_PCH_PWR_BTN_ODL */
	PAD_NF(GPIO_0, PWR_BTN_L, PULL_UP),

	/* GPIO_1 - SYS_RST_ODL */
	PAD_NF(GPIO_1, SYS_RESET_L, PULL_UP),

	/* GPIO_2 - WLAN_PCIE_WAKE_3V3_ODL */
	PAD_NF_SCI(GPIO_2, WAKE_L, PULL_UP, EDGE_LOW),

	/* GPIO_3 - MEM_VOLT_SEL */
	PAD_GPI(GPIO_3, PULL_UP),

	/* GPIO_5 - PCH_TRACKPAD_INT_3V3_ODL, SCI */
	PAD_SCI(GPIO_5, PULL_UP, EDGE_LOW),

	/* GPIO_6 - APU_RST_L / EC_SMI_ODL, SMI */
	PAD_SMI(GPIO_6, PULL_UP, LEVEL_LOW),

	/* GPIO_7 - APU_PWROK_OD (currently not used) */
	PAD_GPI(GPIO_7, PULL_UP),

	/* GPIO_8 - DDR_ALERT_3V3_L (currently not used) */
	PAD_GPI(GPIO_8, PULL_UP),

	/* GPIO_10 - SLP_S0_L, EC_SYNC_IRQ */
	PAD_GPI(GPIO_10, PULL_UP),

	/* GPIO_11 - TOUCHSCREEN_INT_3V3_ODL, SCI */
	PAD_SCI(GPIO_11, PULL_UP, EDGE_LOW),

	/* GPIO_12 - EN_PP3300_TRACKPAD */
	PAD_GPO(GPIO_12, HIGH),

	/* GPIO_13 - APU_PEN_PDCT_ODL (currently not used) */
	PAD_GPI(GPIO_13, PULL_UP),

	/* GPIO_14 - APU_HP_INT_ODL, SCI */
	PAD_SCI(GPIO_14, PULL_UP, EDGE_LOW),

	/* GPIO_16 - USB_C0_OC_L */
	PAD_NF(GPIO_16, USB_OC0_L, PULL_UP),

	/* GPIO_17 - USB_C1_OC_L */
	PAD_NF(GPIO_17, USB_OC1_L, PULL_UP),

	/* GPIO_18 - USB_A0_OC_ODL */
	PAD_NF(GPIO_18, USB_OC2_L, PULL_UP),

	/* GPIO_19 - APU_I2C_SCL3 (Touchscreen) */
	PAD_NF(GPIO_19, I2C3_SCL, PULL_UP),

	/* GPIO_20 - APU_I2C_SDA3 (Touchscreen) */
	PAD_NF(GPIO_20, I2C3_SDA, PULL_UP),

	/* GPIO_21 - APU_PEN_INT_ODL, SCI */
	PAD_SCI(GPIO_21, PULL_UP, EDGE_LOW),

	/* GPIO_22 - EC_SCI_ODL, SCI */
	PAD_SCI(GPIO_22, PULL_UP, EDGE_LOW),

	/* GPIO_24 - EC_PCH_WAKE_L, SCI */
	PAD_SCI(GPIO_24, PULL_UP, EDGE_LOW),

	/* GPIO_25 - SD_CD */
	PAD_NF(GPIO_25, SD0_CD, PULL_UP),

	/* GPIO_42 - S5_MUX_CTRL */
	PAD_NF(GPIO_42, S5_MUX_CTRL, PULL_NONE),

	/* GPIO_67 - PEN_RESET */
	PAD_GPO(GPIO_67, LOW),

	/* GPIO_75 - Unused (strap) (R139/R130) */
	PAD_GPI(GPIO_75, PULL_UP),

	/* GPIO_76 - EN_PP3300_TOUCHSCREEN */
	PAD_GPO(GPIO_76, HIGH),

	/* GPIO_84 - HUB_RST (Active High) */
	PAD_GPO(GPIO_84, LOW),

	/* GPIO_85 - TOUCHSCREEN_RST (Active High) */
	PAD_GPO(GPIO_85, LOW),

	/* GPIO_86 - Unused (TP109) */
	PAD_GPI(GPIO_86, PULL_UP),

	/* GPIO_87 - LPC_SERIRQ */
	PAD_NF(GPIO_87, SERIRQ, PULL_NONE),

	/* GPIO_88 - LPC_CLKRUN_L */
	PAD_NF(GPIO_88, LPC_CLKRUN_L, PULL_NONE),

	/* GPIO_90 - EN_PP3300_CAMERA */
	PAD_GPO(GPIO_90, HIGH),

	/* GPIO_91 - DMIC_CLK1_EN */
	PAD_GPO(GPIO_91, HIGH),

	/* GPIO_93 - EMMC_RST */
	PAD_GPO(GPIO_93, LOW),

	/* GPIO_101 - SD_WP_L */
	PAD_NF(GPIO_101, SD0_WP, PULL_DOWN),

	/* GPIO_102 - EN_SD_SOCKET_PWR */
	PAD_NF(GPIO_102, SD0_PWR_CTRL, PULL_DOWN),

	/* GPIO_113 - APU_I2C_SCL2 (Pen & Trackpad) */
	PAD_NF(GPIO_113, I2C2_SCL, PULL_UP),

	/* GPIO_114 - APU_I2C_SDA2 (Pen & Trackpad) */
	PAD_NF(GPIO_114, I2C2_SDA, PULL_UP),

	/* GPIO_115 - Unused (TP127) */
	PAD_GPI(GPIO_115, PULL_UP),

	/* GPIO_116 - PCIE_EMMC_CLKREQ_L */
	PAD_NF(GPIO_116, CLK_REQ2_L, PULL_NONE),

	/* GPIO_118 - PCH_SPI_CS0_L */
	PAD_NF(GPIO_118, SPI_CS1_L, PULL_NONE),

	/* GPIO_119 - SPK_PA_EN */
	PAD_GPO(GPIO_119, LOW),

	/* GPIO_126 - DMIC_CLK2_EN */
	PAD_GPO(GPIO_126, HIGH),

	/* GPIO_129 - APU_KBRST_L */
	PAD_NF(GPIO_129, KBRST_L, PULL_UP),

	/* GPIO_130 - Unused (TP55) */
	PAD_GPI(GPIO_130, PULL_UP),

	/* GPIO_135 - BCLK Buffer Enable */
	PAD_GPO(GPIO_135, HIGH),

	/* GPIO_137 - Unused (TP27) */
	PAD_GPI(GPIO_137, PULL_UP),

	/* GPIO_140 - I2S_BCLK_R (init to func0, used for I2S) */
	PAD_NF(GPIO_140, UART1_CTS_L, PULL_NONE),

	/* GPIO_141 - I2S2_DATA_MIC2 (init to func0, used for I2S) */
	PAD_NF(GPIO_141, UART1_RXD, PULL_NONE),

	/* GPIO_143 - I2S2_DATA (init to func0, used for I2S) */
	PAD_NF(GPIO_143, UART1_TXD, PULL_NONE),

	/* GPIO_144 - I2S_LR_R (init to func0, used for I2S) */
	PAD_NF(GPIO_144, UART1_INTR, PULL_NONE),

	/* GPIO_145 - PCH_I2C_AUDIO_SCL */
	PAD_NF(GPIO_145, I2C0_SCL, PULL_NONE),

	/* GPIO_146 - PCH_I2C_AUDIO_SDA */
	PAD_NF(GPIO_146, I2C0_SDA, PULL_NONE),

	/* GPIO_147 - PCH_I2C_H1_TPM_SCL */
	PAD_NF(GPIO_147, I2C1_SCL, PULL_NONE),

	/* GPIO_148 - PCH_I2C_H1_TPM_SDA */
	PAD_NF(GPIO_148, I2C1_SDA, PULL_NONE),
};

const __weak
struct soc_amd_gpio *variant_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_reset);
	return gpio_set_stage_reset;
}

const __weak
struct soc_amd_gpio *variant_wlan_rst_early_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_wlan_rst_early_reset);
	return gpio_wlan_rst_early_reset;
}

const
struct soc_amd_gpio *baseboard_romstage_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_rom);
	return gpio_set_stage_rom;
}

const __weak
struct soc_amd_gpio *variant_gpio_table(size_t *size)
{
	*size = ARRAY_SIZE(gpio_set_stage_ram);
	return gpio_set_stage_ram;
}

int __weak variant_get_xhci_oc_map(uint16_t *map)
{
	*map =  USB_OC0 << OC_PORT0_SHIFT; /* USB-C Port0/4 = OC0 */
	*map |= USB_OC1 << OC_PORT1_SHIFT; /* USB-C Port1/5 = OC1 */
	*map |= USB_OC2 << OC_PORT2_SHIFT; /* USB-A HUB Port2/6 = OC2 */
	*map |= USB_OC_DISABLE << OC_PORT3_SHIFT;
	return 0;
}

int __weak variant_get_ehci_oc_map(uint16_t *map)
{
	*map = USB_OC_DISABLE_ALL;
	return 0;
}
