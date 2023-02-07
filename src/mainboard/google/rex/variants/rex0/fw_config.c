/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <bootstate.h>
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
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO, MAX98360_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, sndw_disable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
	}
}
