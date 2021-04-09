/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>

static const struct pad_config i2s_enable_pads[] = {
	/* Audio Codec INT N */
	PAD_CFG_GPI_APIC(GPP_H3, NONE, PLTRST, EDGE_BOTH, INVERT),

	/* I2S0_SCLK */
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),
	/* I2S0_SFRM */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),
	/* I2S0_TXD */
	PAD_CFG_NF(GPP_R2, NONE, DEEP, NF2),
	/* I2S0_RXD */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),

	/* I2S2_SCLK */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF2),
	/* I2S2_SFRM */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),
	/* I2S2_TXD */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),
	/* I2S2_RXD */
	PAD_CFG_NF(GPP_R7, NONE, DEEP, NF2),

	/* I2S_MCLK1_OUT */
	PAD_CFG_NF(GPP_D19, NONE, DEEP, NF1),
};

static void fw_config_handle(void *unused)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, ADL_MAX98373_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure GPIOs for I2S audio.\n");
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
