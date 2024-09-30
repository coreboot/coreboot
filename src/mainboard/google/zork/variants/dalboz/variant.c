/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/i2c_tunnel/chip.h>
#include <static.h>

#define DALBOZ_DB_HDMI 0x1

/*
 * This is to allow support for audio on older board versions (< 2). [b/153458561]. This
 * should be removed once these boards are phased out.
 */
static void update_audio_configuration(void)
{
	uint32_t board_version;
	const struct device *i2c_tunnel_dev = DEV_PTR(audio_rt5682);
	struct ec_google_chromeec_i2c_tunnel_config *cfg;

	/* If CBI board version cannot be read, assume this is an older revision of hardware. */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 1;

	if (board_version >= 2)
		return;

	cfg = config_of(i2c_tunnel_dev);
	cfg->remote_bus = 5;
}

void variant_devtree_update(void)
{
	uint32_t board_version;
	struct soc_amd_picasso_config *cfg;

	cfg = config_of_soc();

	/*
	 * If CBI board version cannot be read, assume this is an older revision
	 * of hardware.
	 */
	if (google_chromeec_cbi_get_board_version(&board_version) != 0)
		board_version = 1;

	if (variant_has_emmc() || boot_is_factory_unprovisioned()) {
		if (board_version <= 2) {
			/*
			 * rev0 and rev1 boards have issues with HS400
			 *
			 * There is a tuning fix with ES which shows promise
			 * for some boards, and a HW fix with stitching vias.
			 * There were also concerns that these boards did not
			 * have good margins for certain skus.
			 *
			 * But these original boards have none of these fixes.
			 * So we keep the speed low here, with the intent that
			 * other variants implement these corrections.
			 */
			cfg->emmc_config.timing = SD_EMMC_EMMC_HS200;
		}
	} else {
		DEV_PTR(emmc)->enabled = 0;
	}

	update_audio_configuration();
}

/* FIXME: Comments seem to suggest these are not entirely correct. */
static const fsp_ddi_descriptor non_hdmi_ddi_descriptors[] = {
	{
		// DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{
		// DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = DP,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	{
		// DP2 pins not connected on Dali
		// DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

static const fsp_ddi_descriptor hdmi_ddi_descriptors[] = {
	{ // DDI0, DP0, eDP
		.connector_type = EDP,
		.aux_index = AUX1,
		.hdp_index = HDP1
	},
	{ // DDI1, DP1, DB OPT2 USB-C1 / DB OPT3 MST hub
		.connector_type = HDMI,
		.aux_index = AUX2,
		.hdp_index = HDP2
	},
	// DP2 pins not connected on Dali
	{ // DDI2, DP3, USB-C0
		.connector_type = DP,
		.aux_index = AUX4,
		.hdp_index = HDP4,
	}
};

void variant_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
				      size_t *dxio_num,
				      const fsp_ddi_descriptor **ddi_descs,
				      size_t *ddi_num)
{
	uint32_t daughterboard_id = variant_get_daughterboard_id();

	*dxio_descs = baseboard_get_dxio_descriptors(dxio_num);

	/*
	 * Get daughterboard id from FW_CONFIG and configure descriptors accordingly.
	 * For unprovisioned boards use DB_HDMI as default.
	 */
	if ((daughterboard_id == DALBOZ_DB_HDMI) || boot_is_factory_unprovisioned()) {
		*ddi_descs = &hdmi_ddi_descriptors[0];
		*ddi_num = ARRAY_SIZE(hdmi_ddi_descriptors);
	} else {
		*ddi_descs = &non_hdmi_ddi_descriptors[0];
		*ddi_num = ARRAY_SIZE(non_hdmi_ddi_descriptors);
	}
}
