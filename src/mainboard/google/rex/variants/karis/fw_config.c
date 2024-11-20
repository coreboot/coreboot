/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

#define GPIO_PADBASED_OVERRIDE(b, a) gpio_padbased_override(b, a, ARRAY_SIZE(a))

static const struct pad_config fp_disable_pads[] = {
	/* GPP_B11 : [] ==> EN_FP_PWR */
	PAD_NC(GPP_B11, NONE),
	/* GPP_C22 : [] ==> SOC_FP_BOOT0 */
	PAD_NC(GPP_C22, NONE),
	/* GPP_C23 : [] ==> FP_RST_ODL */
	PAD_NC(GPP_C23, NONE),
	/* GPP_E10 : [] ==> SOC_FPMCU_INT_L */
	PAD_NC(GPP_E10, NONE),
	/* GPP_F11 : GSP1_SOC_CLK_R */
	PAD_NC(GPP_F11, NONE),
	/* GPP_F12 : GSPI1_SOC_DO_FPMCU_DI_R */
	PAD_NC(GPP_F12, NONE),
	/* GPP_F13 : GSPI1_SOC_DI_FPMCU_DO_LS_R */
	PAD_NC(GPP_F13, NONE),
	/* GPP_F17 : [] ==> GSPI1_SOC_CS_L */
	PAD_NC(GPP_F17, NONE),
};

static const struct pad_config stylus_disable_pads[] = {
	/* GPP_E04 : SOC_PEN_DETECT */
	PAD_NC(GPP_E04, NONE),
	/* GPP_E09 : SOC_PEN_DETECT */
	PAD_NC(GPP_E09, NONE),
};

static const struct pad_config cnvi_bt_disable_pads[] = {
	/* GPP_F04 :  [] ==> CNV_RF_RST_L */
	PAD_NC(GPP_F04, NONE),
	/* GPP_F05 :  [] ==> CNV_CLKREQ */
	PAD_NC(GPP_F05, NONE),
};

static const struct pad_config discrete_bt_disable_pads[] = {
	/* GPP_S01 : [] ==> SDW_HP_DATA_WLAN_PCM_SYNC */
	PAD_NC(GPP_S01, NONE),
	/* GPP_S02 : [] ==> DMIC_SOC_CLK0_WLAN_PCM_OUT */
	PAD_NC(GPP_S02, NONE),
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

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no stylus.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, stylus_disable_pads);
	}
	if (fw_config_probe(FW_CONFIG(FP_MCU, FP_ABSENT))) {
		printk(BIOS_INFO, "Configure GPIOs for no FP module.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, fp_disable_pads);
	}
	if (fw_config_probe(FW_CONFIG(WIFI_TYPE, WIFI_CNVI))) {
		printk(BIOS_INFO, "Configure GPIOs for CNVi WIFI/BT module.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
		printk(BIOS_INFO, "Disable GPIOs for PCIe WIFI/BT module.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, discrete_bt_disable_pads);
	}
	if (fw_config_probe(FW_CONFIG(WIFI_TYPE, WIFI_PCIE))) {
		printk(BIOS_INFO, "Configure GPIOs for discrete WIFI/BT module.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, discrete_bt_i2s_enable_pads);
		printk(BIOS_INFO, "Configure GPIOs for discrete PCM BT path.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, cnvi_bt_disable_pads);
	}
}
