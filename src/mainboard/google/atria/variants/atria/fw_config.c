/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <delay.h>
#include <fw_config.h>
#include <gpio.h>
#include <inttypes.h>

/* t: base table; o: override table */
#define GPIO_PADBASED_OVERRIDE(t, o) gpio_padbased_override(t, o, ARRAY_SIZE(o))
/* t: table */
#define GPIO_CONFIGURE_PADS(t) gpio_configure_pads(t, ARRAY_SIZE(t))

static const struct pad_config pre_mem_gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
};

static const struct pad_config pre_mem_gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
};

static const struct pad_config gen4_ssd_pads[] = {
	/* GPP_B10:     SOC_M2_GEN4_SSD3_PWREN */
	PAD_CFG_GPO(GPP_B10, 1, DEEP),
	/* GPP_A11: 	SOC_M2_GEN4_SSD3_RESET_N */
	PAD_CFG_GPO(GPP_A11, 1, DEEP),
};

static const struct pad_config gen5_ssd_pads[] = {
	/* GPP_E04:     SOC_M2_GEN5_SSD2_PWREN */
	PAD_CFG_GPO(GPP_E04, 1, DEEP),
	/* GPP_E03: 	SOC_M2_GEN5_SSD2_RESET_N */
	PAD_CFG_GPO(GPP_E03, 1, DEEP),
};

static const struct pad_config ufs_enable_pads[] = {
	/* GPP_B25: 	UFS_RST_N */
	PAD_CFG_GPO(GPP_B25, 1, DEEP),
	/* GPP_D21:     GPP_D21_UFS_REFCLK */
	PAD_CFG_NF(GPP_D21, NONE, DEEP, NF1),
};

void fw_config_configure_pre_mem_gpio(void)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_CONFIGURE_PADS(pre_mem_gen5_ssd_pads);
	}
}

void fw_config_gpio_padbased_override(struct pad_config *padbased_table)
{
	if (!fw_config_is_provisioned()) {
		printk(BIOS_WARNING, "FW_CONFIG is not provisioned\n");
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
		return;
	}

	if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN4))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen4_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_NVME_GEN5))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, gen5_ssd_pads);
	} else if (fw_config_probe(FW_CONFIG(STORAGE_TYPE, STORAGE_TYPE_UFS))) {
		GPIO_PADBASED_OVERRIDE(padbased_table, ufs_enable_pads);
	}
}
