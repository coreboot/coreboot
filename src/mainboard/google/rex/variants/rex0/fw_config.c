/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(b, a) gpio_padbased_override(b, a, ARRAY_SIZE(a))

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_S02, NONE),
	PAD_NC(GPP_S03, NONE),
	PAD_NC(GPP_S06, NONE),
	PAD_NC(GPP_S07, NONE),
};

static const struct pad_config sndw_disable_pads[] = {
	PAD_NC(GPP_S00, NONE),
	PAD_NC(GPP_S01, NONE),
	PAD_NC(GPP_S04, NONE),
	PAD_NC(GPP_S05, NONE),
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_D09, NONE),
	PAD_NC(GPP_D10, NONE),
	PAD_NC(GPP_D11, NONE),
	PAD_NC(GPP_D12, DN_20K),
	PAD_NC(GPP_D13, NONE),
	PAD_NC(GPP_D14, NONE),
	PAD_NC(GPP_D15, NONE),
	PAD_NC(GPP_D16, NONE),
	PAD_NC(GPP_D17, NONE),
};

static const struct pad_config bt_i2s_enable_pads[] = {
	/* GPP_V30 : [] ==> BT_I2S_BCLK */
	PAD_CFG_NF(GPP_VGPIO30, NONE, DEEP, NF2),
	/* GPP_V31 : [] ==> BT_I2S_SYNC */
	PAD_CFG_NF(GPP_VGPIO31, NONE, DEEP, NF2),
	/* GPP_V32 : [] ==> BT_I2S_SDO */
	PAD_CFG_NF(GPP_VGPIO32, NONE, DEEP, NF2),
	/* GPP_V33 : [] ==> BT_I2S_SDI */
	PAD_CFG_NF(GPP_VGPIO33, NONE, DEEP, NF2),
	/* GPP_V34 : [] ==> SSP2_SCLK */
	PAD_CFG_NF(GPP_VGPIO34, NONE, DEEP, NF1),
	/* GPP_V35 : [] ==> SSP2_SFRM */
	PAD_CFG_NF(GPP_VGPIO35, NONE, DEEP, NF1),
	/* GPP_V36 : [] ==> SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO36, NONE, DEEP, NF1),
	/* GPP_V37 : [] ==> SSP_RXD */
	PAD_CFG_NF(GPP_VGPIO37, NONE, DEEP, NF1),
};

static const struct pad_config discrete_bt_i2s_enable_pads[] = {
	/* GPP_S00 : [] ==> I2S1_SCLK */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF6),
	/* GPP_S01 : [] ==> I2S1_SFRM */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF6),
	/* GPP_S02 : [] ==> I2S1_TXD */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF6),
	/* GPP_S03 : [] ==> I2S1_RXD */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF6),
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

static const struct pad_config ish_enable_pads[] = {
	/* GPP_B02 : ISH I2C0_SDA */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B02, NONE, DEEP, NF3),
	/* GPP_B03 : ISH_I2C0_SCL */
	PAD_CFG_NF_IOSTANDBY_IGNORE(GPP_B03, NONE, DEEP, NF3),
	/* GPP_B08 : ISH_GP3, SOC_ISH_ACCEL_INT_L */
	PAD_CFG_NF(GPP_B08, NONE, DEEP, NF4),
	/* GPP_E15 : ISH_GP5A, SOC_ISH_IMU_INT_L */
	PAD_CFG_NF(GPP_E15, NONE, DEEP, NF8),
	/* GPP_E16 : ISH_GP10, SOC_EC_ISH_NB_MODE */
	PAD_CFG_NF(GPP_E16, NONE, DEEP, NF8),
	/* GPP_F22 : ISH_GP8A, SOC_ISH_ALS_INT_L */
	PAD_CFG_NF(GPP_F22, NONE, DEEP, NF8),
	/* GPP_F23 : ISH_GP9A, LID_OPEN_1V8 */
	PAD_CFG_NF(GPP_F23, NONE, DEEP, NF8),
	/* GPP_H11 : ISH_GP11A, SOC_ISH_IMU_INT_L */
	PAD_CFG_NF(GPP_H11, NONE, DEEP, NF8),
};

static const struct pad_config touchscreen_i2c_int_pads[] = {
	/* GPP_C07 : [] ==> SOC_TCHSCR_INT */
	PAD_CFG_GPI_APIC(GPP_C07, NONE, PLTRST, LEVEL, NONE),
};

static const struct pad_config touchscreen_spi_int_pads[] = {
	/* GPP_C07 : [] ==> SOC_TCHSCR_INT */
	PAD_CFG_GPI_APIC(GPP_C07, NONE, PLTRST, EDGE_SINGLE, INVERT),
};

static const struct pad_config uwb_gspi1_enable_pads[] = {
	PAD_CFG_GPO_LOCK(GPP_D07, 0, LOCK_CONFIG), /* FPMCU_UWB_MUX_SEL */
	PAD_CFG_GPO(GPP_E06, 0, DEEP), /* UWB_SOC_SYNC */
	PAD_CFG_GPO(GPP_F19, 0, DEEP), /* EN_PP1800_UWB */
	PAD_CFG_GPI_INT(GPP_F20, NONE, PLTRST, LEVEL), /* UWB_SOC_INT */
};

static const struct pad_config uwb_gspi1_disable_pads[] = {
	PAD_CFG_GPO_LOCK(GPP_D07, 1, LOCK_CONFIG), /* FPMCU_UWB_MUX_SEL */
	PAD_NC(GPP_E06, NONE),
	PAD_NC(GPP_F19, NONE),
	PAD_NC(GPP_F20, NONE),
};

static const struct pad_config wwan_disable_pads[] = {
	PAD_CFG_GPO(GPP_B17, 0, DEEP) /* EN_WWAN_PWR */
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, dmic_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_UNKNOWN))) {
		printk(BIOS_INFO, "Configure GPIOs for no audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, dmic_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98363_CS42L42_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for SoundWire audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_disable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_DISCRETE_I2S_BT))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode(discrete).\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, discrete_bt_i2s_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(ISH, ISH_ENABLE))) {
		printk(BIOS_INFO, "Configure GPIOs for ISH.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, ish_enable_pads);
	}

	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_I2C))) {
		printk(BIOS_INFO, "Configure Touchscreen Interrupt for I2C.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_i2c_int_pads);
	} else { /* SPI */
		printk(BIOS_INFO, "Configure Touchscreen Interrupt for SPI.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, touchscreen_spi_int_pads);
	}

	if (fw_config_probe(FW_CONFIG(UWB, UWB_GSPI1)) &&
		fw_config_probe(FW_CONFIG(FP, FP_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for UWB over GSPI1.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, uwb_gspi1_enable_pads);
	} else {
		printk(BIOS_INFO, "Disabling UWB (absent or misconfigured)\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, uwb_gspi1_disable_pads);
	}

	if (fw_config_probe(FW_CONFIG(CELLULAR, CELLULAR_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no cellular.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, wwan_disable_pads);
	}
}
