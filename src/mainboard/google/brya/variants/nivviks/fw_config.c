/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <boardid.h>
#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config lte_disable_pads_nivviks[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* D6  : WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC(GPP_F12, NONE),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config lte_disable_pads_nirwen[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* E13  : WWAN_EN */
	PAD_NC(GPP_E13, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC(GPP_F12, NONE),
	/* H19 : SOC_I2C_SUB_INT_ODL */
	PAD_NC(GPP_H19, NONE),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

static const struct pad_config sd_disable_pads[] = {
	/* D8  : SD_CLKREQ_ODL */
	PAD_NC(GPP_D8, NONE),
	/* H12 : SD_PERST_L */
	PAD_NC(GPP_H12, NONE),
	/* H13 : EN_PP3300_SD_X */
	PAD_NC(GPP_H13, NONE),
};

static const struct pad_config wfc_disable_pads[] = {
	/* D3  : WCAM_RST_L */
	PAD_NC(GPP_D3, NONE),
	/* D15 : EN_PP2800_WCAM_X */
	PAD_NC(GPP_D15, NONE),
	/* D16 : EN_PP1800_PP1200_WCAM_X */
	PAD_NC(GPP_D16, NONE),
	/* H22 : WCAM_MCLK_R */
	PAD_NC(GPP_H22, NONE),
	/* R6 : DMIC_WCAM_CLK_R */
	PAD_NC(GPP_R6, NONE),
	/* R7 : DMIC_WCAM_DATA */
	PAD_NC(GPP_R7, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_probe(FW_CONFIG(DB_USB, DB_1C_LTE))) {
		if (board_id() == 2) {
			printk(BIOS_INFO, "Disable LTE-related GPIO pins on nirwen.\n");
			gpio_configure_pads(
				lte_disable_pads_nirwen,
				ARRAY_SIZE(lte_disable_pads_nirwen)
			);
		} else {
			printk(BIOS_INFO, "Disable LTE-related GPIO pins on nivviks.\n");
			gpio_configure_pads(
				lte_disable_pads_nivviks,
				ARRAY_SIZE(lte_disable_pads_nivviks)
			);
		}
	}

	if (fw_config_probe(FW_CONFIG(SD_CARD, SD_ABSENT))) {
		printk(BIOS_INFO, "Disable SD card GPIO pins.\n");
		gpio_configure_pads(sd_disable_pads, ARRAY_SIZE(sd_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(WFC, WFC_ABSENT))) {
		printk(BIOS_INFO, "Disable MIPI WFC GPIO pins.\n");
		gpio_configure_pads(wfc_disable_pads, ARRAY_SIZE(wfc_disable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
