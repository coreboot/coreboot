/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config hda_enable_pads[] = {
	/* HDA_BCLK */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF1),
	/* HDA_SYNC */
	PAD_CFG_NF(GPP_D11, NATIVE, DEEP, NF1),
	/* HDA_SDO */
	PAD_CFG_NF(GPP_D12, NATIVE, DEEP, NF1),
	/* HDA_SDI_0 */
	PAD_CFG_NF(GPP_D13, NATIVE, DEEP, NF1),
	/* HDA_RST_B */
	PAD_CFG_NF(GPP_D16, NONE, DEEP, NF1),
	/* HDA_SDI_1 */
	PAD_CFG_NF(GPP_D17, NATIVE, DEEP, NF1),

	/* DMIC_CLK */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* DMIC_DATA */
	PAD_CFG_NF(GPP_S05, NONE, DEEP, NF5),
};

static const struct pad_config bt_i2s_enable_pads[] = {
	/* GPP_V30 : [] ==> BT_I2S_BCLK - SSP2 */
	PAD_CFG_NF(GPP_VGPIO30, NONE, DEEP, NF3),
	/* GPP_V31 : [] ==> BT_I2S_SYNC - SSP2 */
	PAD_CFG_NF(GPP_VGPIO31, NONE, DEEP, NF3),
	/* GPP_V32 : [] ==> BT_I2S_SDO - SSP2 */
	PAD_CFG_NF(GPP_VGPIO32, NONE, DEEP, NF3),
	/* GPP_V33 : [] ==> BT_I2S_SDI - SSP2 */
	PAD_CFG_NF(GPP_VGPIO33, NONE, DEEP, NF3),
	/* GPP_V34 : [] ==> SSP_SCLK */
	PAD_CFG_NF(GPP_VGPIO34, NONE, DEEP, NF1),
	/* GPP_V35 : [] ==> SSP_SFRM */
	PAD_CFG_NF(GPP_VGPIO35, NONE, DEEP, NF1),
	/* GPP_V36 : [] ==> SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO36, NONE, DEEP, NF1),
	/* GPP_V37 : [] ==> SSP_RXD */
	PAD_CFG_NF(GPP_VGPIO37, NONE, DEEP, NF1),
};

static const struct pad_config bt_i2s_disable_pads[] = {
	/* GPP_V30 : [] ==> BT_I2S_BCLK */
	PAD_NC(GPP_VGPIO30, NONE),
	/* GPP_V31 : [] ==> BT_I2S_SYNC */
	PAD_NC(GPP_VGPIO31, NONE),
	/* GPP_V32 : [] ==> BT_I2S_SDO */
	PAD_NC(GPP_VGPIO32, NONE),
	/* GPP_V33 : [] ==> BT_I2S_SDI */
	PAD_NC(GPP_VGPIO33, NONE),
	/* GPP_V34 : [] ==> SSP2_SCLK */
	PAD_NC(GPP_VGPIO34, NONE),
	/* GPP_V35 : [] ==> SSP2_SFRM */
	PAD_NC(GPP_VGPIO35, NONE),
	/* GPP_V36 : [] ==> SSP_TXD */
	PAD_NC(GPP_VGPIO36, NONE),
	/* GPP_V37 : [] ==> SSP_RXD */
	PAD_NC(GPP_VGPIO37, NONE),
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
	PAD_CFG_NF(GPP_S06, NONE, DEEP, NF3),
	/* SNDW1_DATA  */
	PAD_CFG_NF(GPP_S07, NONE, DEEP, NF3),

	/* DMIC_CLK */
	PAD_CFG_NF(GPP_D16, NONE, DEEP, NF3),
	/* DMIC_DATA */
	PAD_CFG_NF(GPP_D17, NONE, DEEP, NF3),
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

static const struct pad_config x1slot_pads[] = {
	/* GPP_A08:     X1_PCIE_SLOT_PWR_EN */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_D19:     X1_DT_PCIE_RST_N */
	PAD_CFG_GPO(GPP_D19, 1, PLTRST),
	/* GPP_B25:     X1_SLOT_WAKE_N */
	PAD_CFG_GPI_SCI_LOW(GPP_B25, NONE, DEEP, LEVEL),
};

static const struct pad_config x1slot_disable_pads[] = {
	/* GPP_A08:     X1_PCIE_SLOT_PWR_EN */
	PAD_CFG_GPO(GPP_A08, 0, PLTRST),
	/* GPP_D19:     X1_DT_PCIE_RST_N */
	PAD_NC(GPP_D19, NONE),
	/* GPP_B25:     X1_SLOT_WAKE_N */
	PAD_NC(GPP_B25, NONE)
};

/*
 * WWAN: power sequence requires three stages:
 * step 1: 3.3V power, FCP# (Full Card Power), RST#, and PERST# off
 * step 2: deassert FCP#
 * step 3: deassert RST# first, and then PERST#.
 * NOTE: Since PERST# is gated by platform reset, PERST# deassertion will happen
 * at much later time and time between RST# and PERSET# is guaranteed.
 */
static const struct pad_config pre_mem_wwan_pwr_seq1_pads[] = {
	/* GPP_H16:     WWAN_PWREN */
	PAD_CFG_GPO(GPP_H16, 1, PLTRST),
	/* GPP_A09:  M.2_WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_A09, 0, PLTRST),
	/* GPP_B20:     M.2_WWAN_RST_N */
	PAD_CFG_GPO(GPP_B20, 0, PLTRST),
	/* GPP_D03:     M.2_WWAN_PERST_GPIO_N */
	PAD_CFG_GPO(GPP_D03, 0, PLTRST),
};

static const struct pad_config pre_mem_wwan_pwr_seq2_pads[] = {
	/* GPP_A09:     M.2_WWAN_FCP_OFF_N */
	PAD_CFG_GPO(GPP_A09, 1, PLTRST),
};

static const struct pad_config wwan_pwr_seq3_pads[] = {
	/* GPP_D03:     M.2_WWAN_PERST_GPIO_N */
	PAD_CFG_GPO(GPP_D03, 1, PLTRST),
	/* GPP_B20:     M.2_WWAN_RST_N */
	PAD_CFG_GPO(GPP_B20, 1, PLTRST),
	/* GPP_E02:     WWAN_WAKE_GPIO_N */
	PAD_CFG_GPI_SCI_LOW(GPP_E02, NONE, DEEP, LEVEL),
};

static const struct pad_config wwan_disable_pads[] = {
	/* GPP_A09:     M.2_WWAN_FCP_OFF_N */
	PAD_NC(GPP_A09, NONE),
	/* GPP_D03:     M.2_WWAN_PERST_GPIO_N */
	PAD_NC(GPP_D03, NONE),
	/* GPP_B20:     M.2_WWAN_RST_N */
	PAD_NC(GPP_B20, NONE),
	/* GPP_H16:     WWAN_PWREN */
	PAD_NC(GPP_H16, NONE),
	/* GPP_A10:     M.2_WWAN_DISABLE_N */
	PAD_NC(GPP_A10, NONE),
	/* GPP_E02:     WWAN_WAKE_GPIO_N */
	PAD_NC(GPP_E02, NONE),
};

/* Gen4 NVME: at the top M.2 slot */
static const struct pad_config pre_mem_gen4_ssd_pwr_pads[] = {
	/* GPP_B10:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_B10, 0, PLTRST),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_B10:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, PLTRST),
	/* GPP_B09:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_B09, 1, PLTRST),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
};

/* Gen5 NVME: at the bottom M.2 slot */
static const struct pad_config pre_mem_gen5_ssd_pwr_pads[] = {
	/* GPP_B16:     GEN5_SSD_PWREN */
	PAD_CFG_GPO(GPP_B16, 0, PLTRST),
};

static const struct pad_config gen5_ssd_pads[] = {
	/* GPP_B16:     GEN5_SSD_PWREN */
	PAD_CFG_GPO(GPP_B16, 1, PLTRST),
	/* GPP_E03:     M2_GEN5_SSD_RESET_N */
	PAD_CFG_GPO(GPP_E03, 1, PLTRST),
};

static const struct pad_config peg_x4slot_wake_disable_pads[] = {
	/* GPP_D24:     PEG_SLOT_WAKE_N */
	PAD_NC(GPP_D24, NONE),
	/* GPP_D25:     X4_SLOT_WAKE_N */
	PAD_NC(GPP_D25, NONE),
};

static const struct pad_config pcie_wlan_enable_pads[] = {
	/* GPP_A11:     WLAN_RST_N */
	PAD_CFG_GPO(GPP_A11, 1, PLTRST),
	/* GPP_A12:     WIFI_WAKE_N */
	PAD_CFG_GPI_SCI_LOW(GPP_A12, NONE, DEEP, LEVEL),
};

static const struct pad_config pcie_wlan_disable_pads[] = {
	/* GPP_A11:     WLAN_RST_N */
	PAD_NC(GPP_A11, NONE),
	/* GPP_A12:     WIFI_WAKE_N */
	PAD_NC(GPP_A12, NONE),
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
	/* GPP_A16:     BT_RF_KILL_N */
	PAD_CFG_GPO(GPP_A16, 1, DEEP),
	/* GPP_A17:     WIFI_RF_KILL_N */
	PAD_CFG_GPO(GPP_A17, 1, DEEP),
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

	/* GPP_A16:     BT_RF_KILL_N */
	PAD_NC(GPP_A16, NONE),
	/* GPP_A17:     WIFI_RF_KILL_N */
	PAD_NC(GPP_A17, NONE),
};

static const struct pad_config touchscreen_disable_pads[] = {
	/* GPP_F08:     TCH_PNL1_PWR_EN */
	PAD_CFG_GPO(GPP_F08, 0, PLTRST),

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
	/* GPP_E16:     THC0_SPI1_RST_N_TCH_PNL1 */
	PAD_NC(GPP_E16, NONE),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	PAD_NC(GPP_E18, NONE),
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
	/* GPP_E16:     THC0_SPI1_RST_N_TCH_PNL1 */
	PAD_CFG_GPO(GPP_E16, 1, DEEP),
	/* GPP_E17:     THC0_SPI1_CS0_N_TCH_PNL1 */
	PAD_NC(GPP_E17, NONE),
	/* GPP_E18:     THC0_SPI1_INT_N_TCH_PNL1 */
	/* NOTE: this SPI INT NF is also used in THC-I2C mode */
	PAD_CFG_NF(GPP_E18, NONE, DEEP, NF3),
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
	/* GPP_E16:     THC0_SPI1_RST_N_TCH_PNL1 NF3: THC HID-SPI */
	/* THC NOTE: use GPO instead of NF for THC0 Rst */
	PAD_CFG_GPO(GPP_E16, 1, DEEP),
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
	/* NOTE: require rework to switch from GPP_A13 to GPP_F18 */
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

static const struct pad_config ish_disable_pads[] = {
	/* GPP_D06:     NC */
	PAD_NC(GPP_D06, NONE),
	/* GPP_E05:     NC */
	PAD_NC(GPP_E05, NONE),
	/* GPP_F23:     NC */
	PAD_NC(GPP_F23, NONE),
};

static const struct pad_config ish_enable_pads[] = {
	/* GPP_D06:     ISH_UART0_TXD */
	PAD_CFG_NF(GPP_D06, NONE, DEEP, NF2),
	/* GPP_E05:     ISH_GP_7_SNSR_HDR */
	PAD_CFG_NF(GPP_E05, NONE, DEEP, NF4),
	/* GPP_F23:     ISH_GP_9A */
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF8),
};

static const struct pad_config fp_disable_pads[] = {
	PAD_NC(GPP_C15, NONE),
	/* GPP_D01:     MOD_TCSS1_TYP_A_VBUS_EN */
	PAD_CFG_GPO(GPP_D01, 1, DEEP),
	PAD_NC(GPP_E17, NONE),
	/* FIXME: b/390031369
	 * use dedicated GPIO PIN for codec enable
	 * when FPS is enabled.
	 */
	/* GPP_E19:     CODEC_EN */
	PAD_CFG_GPO(GPP_E19, 1, PLTRST),
	PAD_NC(GPP_E20, NONE),
	PAD_NC(GPP_F14, NONE),
	PAD_NC(GPP_F15, NONE),
	PAD_NC(GPP_F16, NONE),
};

static const struct pad_config fp_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO_LOCK(GPP_C15, 1, LOCK_CONFIG),
	/* GPP_D01:     FPS_SOC_INT_L */
	PAD_CFG_GPI_IRQ_WAKE(GPP_D01, NONE, PWROK, LEVEL, INVERT),
	/* GPP_E17:     GSPI0A_CS0 */
	PAD_CFG_NF(GPP_E17, NONE, DEEP, NF5),
	/* GPP_E19:     FPMCU_PWREN */
	PAD_CFG_GPO(GPP_E19, 1, DEEP),
	/* GPP_E20:     FPMCU_FW_UPDATE */
	PAD_CFG_GPO_LOCK(GPP_E20, 0, LOCK_CONFIG),
	/* GPP_F14:     GPSI0A_MOSI */
	PAD_CFG_NF(GPP_F14, NONE, DEEP, NF8),
	/* GPP_F15:     GSPI0A_MISO */
	PAD_CFG_NF(GPP_F15, NONE, DEEP, NF8),
	/* GPP_F16:     GPSI0A_CLK */
	PAD_CFG_NF(GPP_F16, NONE, DEEP, NF8),
};

static const struct pad_config pre_mem_fp_enable_pads[] = {
	/* GPP_C15:     FPS_RST_N */
	PAD_CFG_GPO(GPP_C15, 0, DEEP),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (!fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_ABSENT)))
		GPIO_CONFIGURE_PADS(pre_mem_wwan_pwr_seq1_pads);

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN5))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pwr_pads);
	}

	/*
	 * NOTE: We place WWAN sequence 2 here. According to the WWAN FIBOCOM
	 * FM350-GL datasheet, the minimum time requirement (Tpr: time between 3.3V
	 * and FCP#) is '0'. Therefore, it will be fine even though there is no
	 * GPIO configured for other PADs via fw_config to have the time delay
	 * introduced in between sequence 1 and 2. Also, FCP# was not the last PAD
	 * configured in sequence 1. Although the Tpr is '0' in the datasheet, three
	 * stages are preserved at this time to guarantee the sequence shown in the
	 * datasheet timing diagram.
	 */
	if (!fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_ABSENT)))
		GPIO_CONFIGURE_PADS(pre_mem_wwan_pwr_seq2_pads);

	if (fw_config_probe(FW_CONFIG(FP, FP_PRESENT)))
		GPIO_CONFIGURE_PADS(pre_mem_fp_enable_pads);

}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	const struct soc_intel_pantherlake_config *config = config_of_soc();

	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME_GEN5))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE, STORAGE_UNKNOWN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_NONE))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC721_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for Soundwire audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_alc721_enable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256_HDA))) {
		printk(BIOS_INFO, "Configure GPIOs for HDA ALC 256 mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, hda_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_PCIE_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_PCIE_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_enable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, pcie_wlan_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_PCIE)) ||
		fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_USB))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, wwan_pwr_seq3_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, wwan_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(SD, SD_NONE)))
		GPIO_PADBASED_OVERRIDE(padbased_table, x1slot_disable_pads);
	else
		GPIO_PADBASED_OVERRIDE(padbased_table, x1slot_pads);

	if (fw_config_probe(FW_CONFIG(TOUCHPAD, TOUCHPAD_LPSS_I2C))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_lpss_i2c_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(TOUCHPAD, TOUCHPAD_THC_I2C))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_thc_i2c_enable_pads);
		if (config->thc_wake_on_touch[1])
			GPIO_PADBASED_OVERRIDE(padbased_table, thc1_enable_wake);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchpad_i2c_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_THC_I2C)))
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_thc_i2c_enable_pads);
	else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_THC_SPI))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_thc_spi_enable_pads);
		if (config->thc_wake_on_touch[0])
			GPIO_PADBASED_OVERRIDE(padbased_table, thc0_enable_wake);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(ISH, ISH_DISABLE)))
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_disable_pads);
	else
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);

	/* NOTE: disable PEG (x8 slot) and x4 slot wake for now */
	GPIO_PADBASED_OVERRIDE(padbased_table, peg_x4slot_wake_disable_pads);

	if (fw_config_probe(FW_CONFIG(FP, FP_PRESENT))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_enable_pads);
	} else {
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_disable_pads);
	}
}
