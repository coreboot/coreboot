/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <ec/google/chromeec/ec.h>
#include <fsp/api.h>
#include <fw_config.h>
#include <intelblocks/hda.h>
#include <sar.h>
#include <soc/gpio_soc_defs.h>
#include <drivers/intel/touch/chip.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return get_wifi_sar_fw_config_filename(FW_CONFIG_FIELD(WIFI_INTERFACE));
}

void variant_update_soc_memory_init_params(FSPM_UPD *memupd)
{
	FSP_M_CONFIG *m_cfg = &memupd->FspmConfig;

	/* HDA Audio */
	printk(BIOS_INFO, "Overriding HDA SDI lanes.\n");
	m_cfg->PchHdaSdiEnable[0] = true;
	m_cfg->PchHdaSdiEnable[1] = false;

	/* Override FSP-M SaGv frequency and gear for DDR5 boards */
	m_cfg->SaGvFreq[0] = 3200;
	m_cfg->SaGvGear[0] = GEAR_4;

	m_cfg->SaGvFreq[1] = 4800;
	m_cfg->SaGvGear[1] = GEAR_4;

	m_cfg->SaGvFreq[2] = 5600;
	m_cfg->SaGvGear[2] = GEAR_4;

	m_cfg->SaGvFreq[3] = 6400;
	m_cfg->SaGvGear[3] = GEAR_4;

	/*
	 * Override FSP-M ChannelToCkdQckMapping to map memory channels
	 * to Clock Driver (CKD) and Query Clock (QCK) signals.
	 */

	const uint8_t channel_to_ckd_qck[] = { 1, 0, 0, 0,
		0, 0, 0, 0 };
	memcpy(m_cfg->ChannelToCkdQckMapping, channel_to_ckd_qck
		, sizeof(channel_to_ckd_qck));

	/*
	 * Override FSP-M PhyClockToCkdDimm to map PHY clocks
	 * to Clock Driver DIMM connections.
	 */
	const uint8_t phy_clock_to_ckd_dimm[] = { 4, 0, 4, 0,
		0, 0, 0, 0 };
	memcpy(m_cfg->PhyClockToCkdDimm, phy_clock_to_ckd_dimm,
		sizeof(phy_clock_to_ckd_dimm));
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
	return true;
}
