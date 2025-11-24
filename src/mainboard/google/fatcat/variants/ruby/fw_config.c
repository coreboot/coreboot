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

static const struct pad_config i2s_enable_pads[] = {
	/* I2S_MCLK1_OUT */
	PAD_CFG_NF(GPP_D09, NONE, DEEP, NF2),
	/* I2S0_SCLK_HDR */
	PAD_CFG_NF(GPP_D10, NONE, DEEP, NF2),
	/* I2S0_SFRM_HDR */
	PAD_CFG_NF(GPP_D11, NONE, DEEP, NF2),
	/* I2S0_TXD_HDR */
	PAD_CFG_NF(GPP_D12, NONE, DEEP, NF2),
	/* I2S0_RXD_HDR */
	PAD_CFG_NF(GPP_D13, NONE, DEEP, NF2),

	/* I2S1_TXD_HDR */
	PAD_CFG_NF(GPP_S00, NONE, DEEP, NF6),
	/* I2S1_RXD_HDR */
	PAD_CFG_NF(GPP_S01, NONE, DEEP, NF6),
	/* I2S1_SCLK_HDR */
	PAD_CFG_NF(GPP_S02, NONE, DEEP, NF6),
	/* I2S1_SFRM_HDR */
	PAD_CFG_NF(GPP_S03, NONE, DEEP, NF6),

	/* DMIC_CLK_A0 */
	PAD_CFG_NF(GPP_S04, NONE, DEEP, NF5),
	/* DMIC_CLK_A0 */
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
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_UNKNOWN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, audio_disable_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_disable_pads);
	} else if (fw_config_probe(FW_CONFIG(AUDIO_CODEC, AUDIO_CODEC_ALC5682IVS))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S ALC5682IVS audio.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, i2s_enable_pads);
		printk(BIOS_INFO, "Configure GPIOs for BT offload mode.\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, bt_i2s_enable_pads);
	}
}
