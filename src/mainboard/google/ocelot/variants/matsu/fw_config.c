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

/* Gen4 NVME: at the top M.2 slot */
static const struct pad_config pre_mem_gen4_ssd_pwr_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 1, PLTRST),
	/* GPP_A08:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_H18:     GEN4_SSD_PWREN */
	PAD_CFG_GPO(GPP_H18, 1, PLTRST),
	/* GPP_A08:     M2_GEN4_SSD_RESET_N */
	PAD_CFG_GPO(GPP_A08, 1, PLTRST),
	/* GPP_C12:     CLKREQ3_X4_GEN4_M2_SSD_N */
	PAD_CFG_NF(GPP_C12, NONE, DEEP, NF1),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
	/* GPP_F09:     M2_UFS_RST_N */
	PAD_CFG_GPO(GPP_F09, 1, DEEP),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UNKNOWN))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pwr_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned, Exiting\n");
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UNKNOWN))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}
}
