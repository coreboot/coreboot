/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/gpio.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <fw_config.h>

static const struct pad_config lte_disable_pads[] = {
	/* A8  : WWAN_RF_DISABLE_ODL */
	PAD_NC(GPP_A8, NONE),
	/* D6  : WWAN_EN */
	PAD_NC(GPP_D6, NONE),
	/* F12 : WWAN_RST_L */
	PAD_NC_LOCK(GPP_F12, NONE, LOCK_CONFIG),
	/* H23 : WWAN_SAR_DETECT_ODL */
	PAD_NC(GPP_H23, NONE),
};

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

static const struct pad_config sd_disable_pads[] = {
	/* D8  : SD_CLKREQ_ODL */
	PAD_NC(GPP_D8, NONE),
	/* H12 : SD_PERST_L */
	PAD_NC_LOCK(GPP_H12, NONE, LOCK_CONFIG),
	/* H13 : EN_PP3300_SD_X */
	PAD_NC_LOCK(GPP_H13, NONE, LOCK_CONFIG),
};

static const struct pad_config stylus_disable_pads[] = {
	/* F13 : SOC_PEN_DETECT_R_ODL */
	PAD_NC_LOCK(GPP_F13, NONE, LOCK_CONFIG),
	/* F15 : SOC_PEN_DETECT_ODL */
	PAD_NC_LOCK(GPP_F15, NONE, LOCK_CONFIG),
};

static const struct pad_config nvme_disable_pads[] = {
	/* B4  : SSD_PERST_L */
	PAD_NC_LOCK(GPP_B4, NONE, LOCK_CONFIG),
	/* D11 : EN_PP3300_SSD */
	PAD_NC_LOCK(GPP_D11, NONE, LOCK_CONFIG),
	/* E17 : SSD_PLN_L */
	PAD_NC_LOCK(GPP_E17, NONE, LOCK_CONFIG),
	/*
	 * Note: don't disable GPP_D6 = SSD_CLKREQ_ODL, since this is used as
	 * WWAN_EN on LTE variants.
	 */
};

/*
 * GPP_D6 is used as WWAN_EN on LTE variants and SSD_CLKREQ_ODL on NVMe
 * variants (there is no craask variant supporting both LTE and NVMe).
 * In craask/gpio.c, it's set to WWAN_EN since this needs to be done in
 * bootblock. So we override it to SSD_CLKREQ_ODL here for NVMe variants.
 */
static const struct pad_config nvme_enable_pads[] = {
	/* D6  : SRCCLKREQ1# ==> SSD_CLKREQ_ODL */
	PAD_CFG_NF(GPP_D6, NONE, DEEP, NF1),
};

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	/*
	 * Since GPP_D6 is used as WWAN_EN on LTE variants and SSD_CLKREQ_ODL on
	 * NVMe variants, we don't support both together. If there's a variant
	 * using both in the future, this GPIO handling will need to be updated.
	 */
	if (fw_config_probe(FW_CONFIG(DB_USB, DB_1C_LTE)) &&
	    fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME))) {
		printk(BIOS_ERR, "LTE and NVMe together is not supported on craask\n");
	}

	if (!fw_config_probe(FW_CONFIG(DB_USB, DB_1C_LTE))) {
		printk(BIOS_INFO, "Disable LTE-related GPIO pins on craask.\n");
		gpio_padbased_override(padbased_table, lte_disable_pads,
						ARRAY_SIZE(lte_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(WFC, WFC_ABSENT))) {
		printk(BIOS_INFO, "Disable MIPI WFC GPIO pins.\n");
		gpio_padbased_override(padbased_table, wfc_disable_pads,
						ARRAY_SIZE(wfc_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(SD_CARD, SD_ABSENT))) {
		printk(BIOS_INFO, "Disable SD card GPIO pins.\n");
		gpio_padbased_override(padbased_table, sd_disable_pads,
						ARRAY_SIZE(sd_disable_pads));
	}

	if (fw_config_probe(FW_CONFIG(STYLUS, STYLUS_ABSENT))) {
		printk(BIOS_INFO, "Disable Stylus GPIO pins.\n");
		gpio_padbased_override(padbased_table, stylus_disable_pads,
						ARRAY_SIZE(stylus_disable_pads));
	}

	if (!fw_config_is_provisioned() ||
	    fw_config_probe(FW_CONFIG(STORAGE, STORAGE_NVME))) {
		/*
		 * Note: this must be done after lte_disable_pads, otherwise
		 * GPP_D6 will be disabled again.
		 */
		printk(BIOS_INFO, "Enable NVMe SSD GPIO pins.\n");
		gpio_padbased_override(padbased_table, nvme_enable_pads,
						ARRAY_SIZE(nvme_enable_pads));
	} else {
		printk(BIOS_INFO, "Disable NVMe SSD GPIO pins.\n");
		gpio_padbased_override(padbased_table, nvme_disable_pads,
						ARRAY_SIZE(nvme_disable_pads));
	}
}
