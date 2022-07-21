/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config wfc_disable_pads[] = {
	/* D3  : WCAM_RST_L */
	PAD_NC_LOCK(GPP_D3, NONE, LOCK_CONFIG),
	/* D15 : EN_PP2800_WCAM_X */
	PAD_NC_LOCK(GPP_D15, NONE, LOCK_CONFIG),
	/* D16 : EN_PP1800_PP1200_WCAM_X */
	PAD_NC_LOCK(GPP_D16, NONE, LOCK_CONFIG),
	/* H22 : WCAM_MCLK_R */
	PAD_NC(GPP_H22, NONE),
	/* R6 : DMIC_WCAM_CLK_R */
	PAD_NC(GPP_R6, NONE),
	/* R7 : DMIC_WCAM_DATA */
	PAD_NC(GPP_R7, NONE),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (fw_config_probe(FW_CONFIG(WFC, WFC_ABSENT))) {
		printk(BIOS_INFO, "Disable MIPI WFC GPIO pins.\n");
		gpio_padbased_override(padbased_table, wfc_disable_pads,
						ARRAY_SIZE(wfc_disable_pads));
	}

}
