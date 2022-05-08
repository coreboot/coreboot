/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config dmic_enable_pads[] = {
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF3),	/* DMIC_CLK0_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF3),	/* DMIC_DATA0_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF3),	/* DMIC_CLK1_R */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF3),	/* DMIC_DATA1_R */
};

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_R4, NONE),
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
};

static const struct pad_config i2s_enable_pads[] = {
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),	/* I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),	/* I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),	/* I2S_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),	/* I2S_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF4),	/* I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF4),	/* I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF4),	/* I2S_PCH_TX_SPKR_RX_R */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF4),	/* I2S_PCH_RX_SPKR_TX */
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_R0, NONE),
	PAD_NC(GPP_R1, NONE),
	PAD_NC(GPP_R2, NONE),
	PAD_NC(GPP_R3, NONE),
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
};

static const struct pad_config bt_i2s_enable_pads[] = {
	PAD_CFG_NF(GPP_VGPIO_30, NONE, DEEP, NF3),	/* BT_I2S_BCLK */
	PAD_CFG_NF(GPP_VGPIO_31, NONE, DEEP, NF3),	/* BT_I2S_SYNC */
	PAD_CFG_NF(GPP_VGPIO_32, NONE, DEEP, NF3),	/* BT_I2S_SDO */
	PAD_CFG_NF(GPP_VGPIO_33, NONE, DEEP, NF3),	/* BT_I2S_SDI */
	PAD_CFG_NF(GPP_VGPIO_34, NONE, DEEP, NF1),	/* SSP2_SCLK */
	PAD_CFG_NF(GPP_VGPIO_35, NONE, DEEP, NF1),	/* SSP2_SFRM */
	PAD_CFG_NF(GPP_VGPIO_36, NONE, DEEP, NF1),	/* SSP_TXD */
	PAD_CFG_NF(GPP_VGPIO_37, NONE, DEEP, NF1),	/* SSP_RXD */
};

static const struct pad_config bt_i2s_disable_pads[] = {
	PAD_NC(GPP_VGPIO_30, NONE),
	PAD_NC(GPP_VGPIO_31, NONE),
	PAD_NC(GPP_VGPIO_32, NONE),
	PAD_NC(GPP_VGPIO_33, NONE),
	PAD_NC(GPP_VGPIO_34, NONE),
	PAD_NC(GPP_VGPIO_35, NONE),
	PAD_NC(GPP_VGPIO_36, NONE),
	PAD_NC(GPP_VGPIO_37, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned() || fw_config_probe(FW_CONFIG(AUDIO, AUDIO_UNKNOWN))) {
		printk(BIOS_INFO, "Disable audio related GPIO pins.\n");
		gpio_configure_pads(i2s_disable_pads, ARRAY_SIZE(i2s_disable_pads));
		gpio_configure_pads(dmic_disable_pads, ARRAY_SIZE(dmic_disable_pads));
		gpio_configure_pads(bt_i2s_disable_pads, ARRAY_SIZE(bt_i2s_disable_pads));
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_NAU88L25B_I2S))) {
		printk(BIOS_INFO, "Configure audio over I2S with MAX98360 NAU88L25B.\n");
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
		printk(BIOS_INFO, "BT offload enabled\n");
		gpio_configure_pads(bt_i2s_enable_pads, ARRAY_SIZE(bt_i2s_enable_pads));
	} else {
		printk(BIOS_INFO, "BT offload disabled\n");
		gpio_configure_pads(bt_i2s_disable_pads, ARRAY_SIZE(bt_i2s_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
