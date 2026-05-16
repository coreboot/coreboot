/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <console/console.h>
#include <fw_config.h>
#include <gpio.h>
#include <option.h>
#include <string.h>
#include <types.h>

enum storage_device {
	STORAGE_NVME = 0,
	STORAGE_EMMC = 1,
};

static enum storage_device storage_default_from_fw_config(void)
{
	return (fw_config_get() & FW_CONFIG_FIELD_BOOT_EMMC_MASK_MASK) ?
		STORAGE_EMMC : STORAGE_NVME;
}

/* Override fw_config_probe_mainboard_override to check CFR for storage selection */
bool fw_config_probe_mainboard_override(const struct fw_config *match, bool *result)
{
	/* Check if this is a storage-related probe */
	if (match->field_name) {
		/* Read CFR option; if unset, use fw_config setting */
		uint8_t storage_selection =
			get_uint_option("storage_device", storage_default_from_fw_config());
		if (strcmp(match->field_name, "BOOT_NVME_MASK") == 0) {
			/* NVMe is enabled if storage selection is NVMe */
			*result = (storage_selection == STORAGE_NVME);
			if (*result) {
				printk(BIOS_INFO, "fw_config: NVMe enabled by CFR\n");
			}
			return true;
		}
		if (strcmp(match->field_name, "BOOT_EMMC_MASK") == 0) {
			/* eMMC is enabled if storage selection is eMMC */
			*result = (storage_selection == STORAGE_EMMC);
			if (*result) {
				printk(BIOS_INFO, "fw_config: eMMC enabled by CFR\n");
			}
			return true;
		}
	}

	/* Not handled - use standard fw_config logic */
	return false;
}

static const struct pad_config dmic_enable_pads[] = {
	PAD_CFG_NF(GPP_S2, NONE, DEEP, NF2),	/* DMIC_CLK0_R */
	PAD_CFG_NF(GPP_S3, NONE, DEEP, NF2),	/* DMIC_DATA0_R */

};

static const struct pad_config dmic_disable_pads[] = {
	PAD_NC(GPP_S2, NONE),
	PAD_NC(GPP_S3, NONE),
};

static const struct pad_config i2s_enable_pads[] = {
	PAD_CFG_NF(GPP_R0, NONE, DEEP, NF2),	/* I2S_HP_SCLK_R */
	PAD_CFG_NF(GPP_R1, NONE, DEEP, NF2),	/* I2S_HP_SFRM_R */
	PAD_CFG_NF(GPP_R2, DN_20K, DEEP, NF2),	/* I2S_PCH_TX_HP_RX_STRAP */
	PAD_CFG_NF(GPP_R3, NONE, DEEP, NF2),	/* I2S_PCH_RX_HP_TX */
	PAD_CFG_NF(GPP_R4, NONE, DEEP, NF2),	/* I2S_SPKR_SCLK_R */
	PAD_CFG_NF(GPP_R5, NONE, DEEP, NF2),	/* I2S_SPKR_SFRM_R */
	PAD_CFG_NF(GPP_R6, NONE, DEEP, NF2),	/* I2S_PCH_TX_SPKR_RX_R */
	PAD_NC(GPP_R7, NONE),			/* I2S_PCH_RX_SPKR_TX */
};

static const struct pad_config i2s_disable_pads[] = {
	PAD_NC(GPP_R0, NONE),
	PAD_NC(GPP_R1, NONE),
	PAD_NC(GPP_R2, NONE),
	PAD_NC(GPP_R3, NONE),
	PAD_NC(GPP_R4, NONE),
	PAD_NC(GPP_R5, NONE),
	PAD_NC(GPP_R6, NONE),
	PAD_NC(GPP_R7, NONE),
};

static void fw_config_handle(void *unused)
{
	if (!fw_config_is_provisioned() || fw_config_probe(FW_CONFIG(AUDIO, AUDIO_UNKNOWN))) {
		printk(BIOS_INFO, "Disable audio related GPIO pins.\n");
		gpio_configure_pads(i2s_disable_pads, ARRAY_SIZE(i2s_disable_pads));
		gpio_configure_pads(dmic_disable_pads, ARRAY_SIZE(dmic_disable_pads));
		return;
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_MAX98357_ALC5682I_I2S))) {
		printk(BIOS_INFO, "Configure audio over I2S with MAX98357 ALC5682I.\n");
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}

	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_MAX98357_ALC5682I_VS_I2S))) {
		printk(BIOS_INFO, "Configure audio over I2S with MAX98357 ALC5682I-VS.\n");
		gpio_configure_pads(dmic_enable_pads, ARRAY_SIZE(dmic_enable_pads));
		gpio_configure_pads(i2s_enable_pads, ARRAY_SIZE(i2s_enable_pads));
	}
}
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, fw_config_handle, NULL);
