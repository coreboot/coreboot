/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>
#include <soc/soc_chip.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config pre_mem_gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
};

static const struct pad_config pre_mem_gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
	/* GPP_A11: 	SOC_M2_GEN4_SSD3_RESET_N */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
};

static const struct pad_config gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
	/* GPP_E03: 	SOC_M2_GEN5_SSD2_RESET_N */
	PAD_CFG_GPO(GPP_E03, 1, DEEP),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_B25: 	UFS_RST_N */
	PAD_CFG_GPO(GPP_B25, 1, DEEP),
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
};

static const struct pad_config pcie_wlan_enable_pads[] = {
	/* GPP_A10:     WLAN_RST_N */
	PAD_CFG_GPO(GPP_A10, 1, PLTRST),
	/* GPP_C06:     WIFI_WAKE_N */
	PAD_CFG_GPI_SCI_LOW(GPP_C06, NONE, DEEP, LEVEL),
};

static const struct pad_config pcie_wlan_disable_pads[] = {
	/* GPP_A10:     WLAN_RST_N */
	PAD_NC(GPP_A10, NONE),
	/* GPP_C06:     WIFI_WAKE_N */
	PAD_NC(GPP_C06, NONE),
};

static const struct pad_config cnvi_enable_pads[] = {
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F00, NONE, DEEP, NF1),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F01, NONE, DEEP, NF1),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F02, NONE, DEEP, NF1),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F03, NONE, DEEP, NF1),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F04, NONE, DEEP, NF1),
	/* GPP_F05:     CRF_CLKREQ_R */
	/* NOTE: IOSSTAGE: 'Ignore' for S0ix */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_F05, NONE, DEEP, NF3),
	/* GPP_A15:     BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_A15, 1, DEEP),
	/* GPP_D23:     WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_D23, 1, DEEP),
};

static const struct pad_config cnvi_disable_pads[] = {
	/* GPP_F00:     M.2_CNV_BRI_DT_BT_UART2_RTS_N */
	PAD_NC(GPP_F00, NONE),
	/* GPP_F01:     M.2_CNV_BRI_RSP_BT_UART2_RXD */
	PAD_NC(GPP_F01, NONE),
	/* GPP_F02:     M.2_CNV_RGI_DT_BT_UART2_TXD */
	PAD_NC(GPP_F02, NONE),
	/* GPP_F03:     M.2_CNV_RGI_RSP_BT_UART2_CTS_N */
	PAD_NC(GPP_F03, NONE),
	/* GPP_F04:     CNV_RF_RESET_R_N */
	PAD_NC(GPP_F04, NONE),
	/* GPP_F05:     CRF_CLKREQ_R */
	PAD_NC(GPP_F05, NONE),

	/* GPP_A15:     BT_RF_KILL_N */
	PAD_NC(GPP_A15, NONE),
	/* GPP_D23:     WIFI_RF_KILL_N */
	PAD_NC(GPP_D23, NONE),
};

static const struct pad_config ish_disable_pads[] = {
	/* GPP_B02: 	ISH_I3C0_SDA_SNSR_HDR_R */
	PAD_NC(GPP_B02, NONE),
	/* GPP_B03: 	ISH_I3C0_SCL_SNSR_HDR_R */
	PAD_NC(GPP_B03, NONE),
	/* GPP_B04: 	ISH_GP_0_SNSR_HDR */
	PAD_NC(GPP_B04, NONE),
	/* GPP_B05: 	ISH_GP_1_SNSR_HDR */
	PAD_NC(GPP_B05, NONE),
	/* GPP_B06: 	ISH_GP_2_SNSR_HDR */
	PAD_NC(GPP_B06, NONE),
	/* GPP_B07: 	ISH_GP_3_SNSR_HDR */
	PAD_NC(GPP_B07, NONE),
	/* GPP_B08: 	ISH_GP_4_SNSR_HDR */
	PAD_NC(GPP_B08, NONE),
	/* GPP_B18: 	ISH_I2C2_SDA_SNSR_HDR */
	PAD_NC(GPP_B18, NONE),
	/* GPP_B19: 	ISH_I2C2_SCL_SNSR_HDR */
	PAD_NC(GPP_B19, NONE),
	/* GPP_B22: 	ISH_GP_5_SNSR_HDR */
	PAD_NC(GPP_B22, NONE),
	/* GPP_B23: 	ISH_GP_6_SNSR_HDR */
	PAD_NC(GPP_B23, NONE),

	/* GPP_D05: 	ISH_SPI_CS_N_SNSR_HDR */
	PAD_NC(GPP_D05, NONE),
	/* GPP_D06: 	ISH_SPI_CLK_SNSR_HDR */
	PAD_NC(GPP_D06, NONE),
	/* GPP_D07: 	ISH_SPI_MISO_SNSR_HDR */
	PAD_NC(GPP_D07, NONE),
	/* GPP_D08: 	ISH_SPI_MOSI_SNSR_HDR */
	PAD_NC(GPP_D08, NONE),

	/* GPP_F08: 	ISH_GP_7A_SNSR_HDR_R */
	PAD_NC(GPP_F08, NONE),
	/* GPP_F09: 	ISH_INT_GP11_CVS */
	PAD_NC(GPP_F09, NONE),
	/* GPP_F23:     ISH_GP_9A; NOTE: ISH will handle SMC_LID; not AP */
	PAD_NC(GPP_F23, NONE),

	/* GPP_H14: 	ISH_I2C1_ISH_I3C1_SDA_SNSR_HDR */
	PAD_NC(GPP_H14, NONE),
	/* GPP_H15: 	ISH_I2C1_ISH_I3C1_SCL_SNSR_HDR */
	PAD_NC(GPP_H15, NONE),
};

static const struct pad_config sndw_alc721_enable_pads[] = {
	/* SNDW3_CLK   */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF1),
	/* SNDW3_DATA0 */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF1),
	/* SNDW3_DATA1 */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF1),
	/* SNDW3_DATA2 */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF1),
	/* DMIC_CLK_A0 */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* DMIC_DATA_0 */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
	/* SNDW1_CLK   */
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF5),
	/* SNDW1_DATA  */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF5),
	/* DMIC_CLK (native PCH DMIC1) - unused, keep disabled */
	PAD_NC(GPP_D16, NONE),
	/* DMIC_DATA (native PCH DMIC1) - unused, keep disabled */
	PAD_NC(GPP_D17, NONE),
};

static const struct pad_config audio_disable_pads[] = {
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S02, NONE),
	PAD_NC(GPP_S03, NONE),
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),
	PAD_NC(GPP_D09, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, NONE),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
};

static const struct pad_config touchscreen_pwr_enable_pads[] = {
	/* GPP_E22:     TCH_PNL1_PWR_EN */
	PAD_CFG_GPO(GPP_E22, 1, PLTRST),
	/* GPP_B20:     M.2_WWAN_RST_N */
	/* NOTE: requires rework to use for TCH_PAUSE_SCAN_N */
	PAD_CFG_GPO(GPP_B20, 1, PLTRST),

	/* GPP_E16:     THC0_SPI1_RST_N_TCH_PNL1 NF3: THC HID-SPI */
	/* THC NOTE: use GPO instead of NF for THC0 Rst */
	PAD_CFG_GPO(GPP_E16, 1, DEEP),
};

static const struct pad_config touchscreen_pwr_disable_pads[] = {
	/* GPP_E22:     TCH_PNL1_PWR_EN */
	PAD_CFG_GPO(GPP_E22, 0, PLTRST),

	/* GPP_B20:     M.2_WWAN_RST_N: rework for TCH_PAUSE_SCAN_N */
	/* NOTE: GPP_B20 might be used for M.2_WWAN_RST_N so we don't disable this PAD here. */

	/* GPP_E16:     THC0_SPI1_RST_N_TCH_PNL1 */
	PAD_NC(GPP_E16, NONE),
};

static const struct pad_config touchscreen_disable_pads[] = {
	/* GPP_E11:     THC0_SPI1_CLK_TCH_PNL1 */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     THC0_SPI1_IO_0_I2C4_SCL_TCH_PNL1 NF8: I2C4_SCL */
	PAD_NC(GPP_E12, NONE),
	/* GPP_E13:     THC0_SPI1_IO_1_I2C4_SDA_TCH_PNL1 NF8: I2C4 SDA */
	PAD_NC(GPP_E13, NONE),
	/* GPP_E14:     THC0_SPI1_IO_2_TCH_PNL1 */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     THC0_SPI1_IO_3_TCH_PNL1 */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	PAD_NC(GPP_E18, NONE),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchscreen_lpss_i2c_enable_pads[] = {
	/* GPP_E11:     THC0_SPI1_CLK_TCH_PNL1 */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     THC0_SPI1_IO_0_I2C4_SCL_TCH_PNL1 NF8: I2C4_SCL */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF8),
	/* GPP_E13:     THC0_SPI1_IO_1_I2C4_SDA_TCH_PNL1 NF8: I2C4 SDA */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF8),
	/* GPP_E14:     THC0_SPI1_IO_2_TCH_PNL1 */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     THC0_SPI1_IO_3_TCH_PNL1 */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	PAD_CFG_GPI_APIC(GPP_E18, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchscreen_thc_i2c_enable_pads[] = {
	/* GPP_E11:     THC0_SPI1_CLK_TCH_PNL1 */
	PAD_NC(GPP_E11, NONE),
	/* GPP_E12:     THC0_SPI1_IO_0_I2C4_SCL_TCH_PNL1 NF1: THC I2C0_SCL */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF1),
	/* GPP_E13:     THC0_SPI1_IO_1_I2C4_SDA_TCH_PNL1 NF1: THC I2C0 SDA */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF1),
	/* GPP_E14:     THC0_SPI1_IO_2_TCH_PNL1 */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     THC0_SPI1_IO_3_TCH_PNL1 */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	/* NOTE: this SPI INT NF is also used in THC-I2C mode */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF3),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchscreen_gspi_enable_pads[] = {
	/* GPP_E11:     THC0_SPI1_CLK_TCH_PNL1 NF5: GSPI0 */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF5),
	/* GPP_E12:     THC0_SPI1_IO_0_I2C4_SCL_TCH_PNL1 NF5: GSPI0 */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF5),
	/* GPP_E13:     THC0_SPI1_IO_1_I2C4_SDA_TCH_PNL1 NF5: GSPI0 */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF5),
	/* GPP_E14:     THC0_SPI1_IO_2_TCH_PNL1 */
	PAD_NC(GPP_E14, NONE),
	/* GPP_E15:     THC0_SPI1_IO_3_TCH_PNL1 */
	PAD_NC(GPP_E15, NONE),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 NF5: GSPI0 */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF5),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	PAD_CFG_GPI_APIC(GPP_E18, NONE, PLTRST, EDGE_SINGLE, INVERT),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchscreen_thc_spi_enable_pads[] = {
	/* GPP_E11:     THC0_SPI1_CLK_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E11, NONE, DEEP, NF3),
	/* GPP_E12:     THC0_SPI1_IO_0_I2C4_SCL_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E12, NONE, DEEP, NF3),
	/* GPP_E13:     THC0_SPI1_IO_1_I2C4_SDA_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E13, NONE, DEEP, NF3),
	/* GPP_E14:     THC0_SPI1_IO_2_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E14, NONE, DEEP, NF3),
	/* GPP_E15:     THC0_SPI1_IO_3_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E15, NONE, DEEP, NF3),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF3),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 NF3: THC HID-SPI */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF3),
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_NC(GPP_VGPIO3_THC0, NONE),
};

static const struct pad_config touchpad_thc_i2c_enable_pads[] = {
	/* GPP_F12:     NF1: THC_I2C1_SCL */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF1),
	/* GPP_F13:     NF1: THC_I2C1_SDA */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF1),
	/* GPP_F18:     TCH_PAD_INT_N */
	/* NOTE: this SPI INT NF is also used in THC-I2C mode */
	/* NOTE: require rework to switch from GPP_A13 to GPP_F18 */
	PAD_CFG_NF(GPP_F18, NONE, DEEP, NF3),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

static const struct pad_config touchpad_lpss_i2c_enable_pads[] = {
	/* GPP_F12:     THC_I2C1_SCL_TCH_PAD */
	PAD_CFG_NF(GPP_F12, NONE, DEEP, NF8),
	/* GPP_F13:     THC_I2C1_SDA_TCH_PAD */
	PAD_CFG_NF(GPP_F13, NONE, DEEP, NF8),
	/* GPP_F18:     TCH_PAD_INT_N */
	PAD_CFG_GPI_APIC(GPP_F18, NONE, PLTRST, LEVEL, INVERT),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

static const struct pad_config touchpad_i2c_disable_pads[] = {
	/* GPP_F12:     THC_I2C1_SCL_TCH_PAD */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13:     THC_I2C1_SDA_TCH_PAD */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F18:     TCH_PAD_INT_N */
	/* NOTE: require rework to switch from GPP_A13 to GPP_F18 */
	PAD_NC(GPP_F18, NONE),
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_NC(GPP_VGPIO3_THC1, NONE),
};

static const struct pad_config thc0_enable_wake[] = {
	/* GPP_VGPIO3_THC0: THC0_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC0, NONE, PLTRST, LEVEL, NONE),
};

static const struct pad_config thc1_enable_wake[] = {
	/* GPP_VGPIO3_THC1: THC1_WOT */
	PAD_CFG_GPI_APIC_DRIVER(GPP_VGPIO3_THC1, NONE, PLTRST, LEVEL, NONE),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	const config_t *config = config_of_soc();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}

	if (!fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_UNDEFINED)))
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_pwr_enable_pads);

	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_LPSS_I2C))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_lpss_i2c_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_THC))) {

		if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_B36_I2C)) ||
			fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_WACOM_B52_I2C)) ||
			fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_REX_I2C))) {
			GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_thc_i2c_enable_pads);

		} else if (fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_B36_SPI)) ||
			fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_WACOM_B52_SPI)) ||
			fw_config_probe(FW_CONFIG(AP_OEM_3BIT_FIELD0, TOUCHSCREEN_ELAN_REX_SPI))) {
			GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_thc_spi_enable_pads);
		}
		if (config->wake_on_touch[0])
			GPIO_PADBASED_OVERRIDE(padbased_table, thc0_enable_wake);

	} else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN_SOC_INTERFACE, TOUCHSCREEN_INTERFACE_GSPI))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_gspi_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_pwr_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(TRACKPAD_SOC_INTERFACE, TRACKPAD_INTERFACE_LPSS_I2C))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_lpss_i2c_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(TRACKPAD_SOC_INTERFACE, TRACKPAD_INTERFACE_THC))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_thc_i2c_enable_pads);
		if (config->wake_on_touch[1])
			GPIO_PADBASED_OVERRIDE(padbased_table, thc1_enable_wake);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_i2c_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(SENSOR_HUB, ISH_ABSENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ALC721))) {
		printk(BIOS_INFO, "Configure GPIOs for ALC721/ALC722 SoundWire audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_alc721_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ABSENT))) {
		printk(BIOS_INFO, "Audio codec absent; disable audio GPIOs.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
	}

	if ( fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_PCIE))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(WIFI_INTERFACE, WIFI_INTERFACE_CNVI))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_disable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_disable_pads);
	}
}
