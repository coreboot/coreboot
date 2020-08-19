/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config dmic_enable_pads[] = {
	PAD_CFG_NF(GPP_S4, NONE, DEEP, NF2),	/* DMIC_CLK1 */
	PAD_CFG_NF(GPP_S5, NONE, DEEP, NF2),	/* DMIC_DATA1 */
	PAD_CFG_NF(GPP_S6, NONE, DEEP, NF2),	/* DMIC_CLK0 */
	PAD_CFG_NF(GPP_S7, NONE, DEEP, NF2),	/* DMIC_DATA0 */
};

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_S4, NONE),
	PAD_NC(GPP_S5, NONE),
	PAD_NC(GPP_S6, NONE),
	PAD_NC(GPP_S7, NONE),
};

static const struct pad_config sndw_enable_pads[] = {
	PAD_CFG_NF(GPP_S0, NONE, DEEP, NF1),	/* SNDW0_CLK */
	PAD_CFG_NF(GPP_S1, NONE, DEEP, NF1),	/* SNDW0_DATA */
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF1),	/* SNDW1_CLK */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF1),	/* SNDW1_DATA */
};

static const struct pad_config sndw_disable_pads[] = {
	PAD_NC(GPP_S0, NONE),
	PAD_NC(GPP_S1, NONE),
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
};

static const struct pad_config i2s_up3_enable_pads[] = {
	PAD_CFG_NF(GPP_A23, NONE, DEEP, NF1),	/* I2S1_SCLK */
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),	/* I2S_MCLK1 */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),	/* I2S0_SCLK */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),	/* I2S0_SFRM */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),	/* I2S0_TXD */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),	/* I2S0_RXD */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),	/* I2S1_RXD */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),	/* I2S1_TXD */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF2),	/* I2S1_SFRM */
};

static const struct pad_config i2s_up4_enable_pads[] = {
	PAD_CFG_NF(GPP_A7, NONE, DEEP, NF1),	/* I2S2_SCLK */
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),	/* I2S_MCLK1 */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),	/* I2S0_SCLK */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),	/* I2S0_SFRM */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),	/* I2S0_TXD */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),	/* I2S0_RXD */
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_A23, NONE),
	PAD_NC(GPP_D19, NONE),
	PAD_NC(GPP_R0, NONE),
	PAD_NC(GPP_R1, NONE),
	PAD_NC(GPP_R2, DN_20K),
	PAD_NC(GPP_R3, NONE),
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, NONE))) {
		printk(BIOS_INFO, "Configure GPIOs for no audio.\n");
		gpio_configure_pads(i2s_disable_pads, ARRAY_SIZE(i2s_disable_pads));
		gpio_configure_pads(dmic_disable_pads, ARRAY_SIZE(dmic_disable_pads));
		gpio_configure_pads(sndw_disable_pads, ARRAY_SIZE(sndw_disable_pads));
	}
	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98373_ALC5682_SNDW))) {
		printk(BIOS_INFO, "Configure GPIOs for SoundWire audio.\n");
		gpio_configure_pads(sndw_enable_pads, ARRAY_SIZE(sndw_enable_pads));
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(i2s_disable_pads, ARRAY_SIZE(i2s_disable_pads));
	}
	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98357_ALC5682I_I2S)) ||
	    fw_config_probe(FW_CONFIG(AUDIO, MAX98373_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio on UP3.\n");
		gpio_configure_pads(i2s_up3_enable_pads, ARRAY_SIZE(i2s_up3_enable_pads));
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(sndw_disable_pads, ARRAY_SIZE(sndw_disable_pads));
	}
	if (fw_config_probe(FW_CONFIG(AUDIO, MAX98373_ALC5682I_I2S_UP4))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio on UP4.\n");
		gpio_configure_pads(i2s_up4_enable_pads, ARRAY_SIZE(i2s_up4_enable_pads));
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(sndw_disable_pads, ARRAY_SIZE(sndw_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
