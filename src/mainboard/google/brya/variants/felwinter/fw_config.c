/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

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
	if (!fw_config_is_provisioned() ||
	     fw_config_probe(FW_CONFIG(AUDIO_AMP, UNPROVISIONED))) {
		printk(BIOS_INFO, "Disable BT offload audio related GPIO pins.\n");
		gpio_configure_pads(bt_i2s_disable_pads, ARRAY_SIZE(bt_i2s_disable_pads));
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_AMP, MAX98360_ALC5682VS_I2S_2WAY))) {
		printk(BIOS_INFO, "BT offload enabled over I2S with MAX98360+RT5682VS\n");
		gpio_configure_pads(bt_i2s_enable_pads, ARRAY_SIZE(bt_i2s_enable_pads));
	}

}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
