/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <drivers/intel/touch/chip.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/api.h>
#include <fw_config.h>
#include <intelblocks/hda.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI));
}

void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config)
{
	/* CNVi */
	if (fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_6)) ||
		fw_config_probe(FW_CONFIG(WIFI, WIFI_CNVI_7))) {
		config->cnvi_wifi_core = true;
		config->cnvi_bt_core = true;

		if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_MAX98360_ALC5682I_I2S)) ||
			fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC722_SNDW)) ||
			fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC721_SNDW))) {
			printk(BIOS_INFO, "BT audio offload configured.\n");
			config->cnvi_bt_audio_offload = true;
		}
	}

    /* Touchscreen and touchpad WOT support:
     * +===================+==================+=================+============================+
     * | touchsreen        | touchpad         | PMC_GPE0_DW0    | WOT                        |
     * +===================+==================+==============================================+
     * | THC-SPI/THC-I2C   | LPSS-I2C         | GPP_F           | TS, TP                     |
     * +-------------------+------------------+----------------------------------------------+
     * | THC-SPI/THC-I2C   | not used or      | GPP_A (default) | TS                         |
     * |                   | with WOT disabled|                 |                            |
     * +-------------------+------------------+----------------------------------------------+
     * | THC-SPI/THC-I2C   | THC-I2C          | GPP_A (default) | TS, TP                     |
     * +-------------------+------------------+----------------------------------------------+
     * | LPSS-SPI/LPSS-I2C | LPSS-I2C         | GPP_F           | TS via PMC_GPE0_DW2: GPP_E,|
     * |                   |                  |                 | TP                         |
     * +-------------------+------------------+----------------------------------------------+
     * | LPSS-SPI/LPSS-I2C | not used or      | GPP_A (default) | TS via PMC_GPE0_DW2: GPP_E |
     * |                   | with WOT disabled|                 |                            |
     * +-------------------+------------------+----------------------------------------------+
     * | not used          | not used or      | GPP_A (default) | NA                         |
     * | with WOT disabled | with WOT disabled|                 |                            |
     * +===================+==================+=================+============================+
     */
	if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_THC_I2C))) {
		config->thc_mode[0] = THC_HID_I2C_MODE;
	} else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_THC_SPI))) {
		config->thc_mode[0] = THC_HID_SPI_MODE;
	} else if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_GSPI))) {
		config->serial_io_gspi_mode[PchSerialIoIndexGSPI0] = PchSerialIoPci;
	}

	if (fw_config_probe(FW_CONFIG(TOUCHPAD, TOUCHPAD_LPSS_I2C))) {
		/* touchpad: GPP_F18: GPE0_DW0_18 */
		if (config->thc_wake_on_touch[1])
			config->pmc_gpe0_dw0 = GPP_F;
	} else if (fw_config_probe(FW_CONFIG(TOUCHPAD, TOUCHPAD_THC_I2C))) {
		config->thc_mode[1] = THC_HID_I2C_MODE;

		if (fw_config_probe(FW_CONFIG(TOUCHSCREEN, TOUCHSCREEN_NONE)))
			/* When THC0 is only enabled due to THC1 is enabled, we force THC0
			 * to I2C mode so that kernel THC I2C driver will find it and put
			 * THC0 to low power state because no connected device is found.
			 */
			config->thc_mode[0] = THC_HID_I2C_MODE;
	}
}

void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	/* HDA Audio */
	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256_HDA))) {
		printk(BIOS_INFO, "Overriding HDA SDI lanes.\n");
		m_cfg->PchHdaSdiEnable[0] = true;
		m_cfg->PchHdaSdiEnable[1] = false;
	}
}

bool variant_is_barrel_charger_present(void)
{
	if (fw_config_probe(FW_CONFIG(PSU, PSU_BJ)))
		return google_chromeec_is_barrel_charger_present();
	else
		return false;
}

/*
 * HDA verb table loading is supported based on the firmware configuration.
 *
 * This function determines if the current platform has an HDA codec enabled by
 * examining the `FW_CONFIG` value. Specifically, it checks if the
 * `FW_CONFIG` includes the `AUDIO_ALC256_HDA` value, which is used to identify
 * Fatcat SKUs with HDA codec support.
 *
 * Return true if the `FW_CONFIG` indicates HDA support (i.e., contains
 * `AUDIO_ALC256_HDA`), false otherwise.
 */
bool mainboard_is_hda_codec_enabled(void)
{
	if (fw_config_probe(FW_CONFIG(AUDIO, AUDIO_ALC256_HDA)))
		return true;

	return false;
}
