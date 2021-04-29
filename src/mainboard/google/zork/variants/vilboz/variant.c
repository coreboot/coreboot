/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <fw_config.h>
#include <sar.h>
#include "chip.h"

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
	*dxio_descs = baseboard_get_dxio_descriptors(dxio_num);

	*ddi_descs = &hdmi_ddi_descriptors[0];
	*ddi_num = ARRAY_SIZE(hdmi_ddi_descriptors);
}

void variant_devtree_update(void)
{
	struct soc_amd_picasso_config *soc_cfg;
	soc_cfg = config_of_soc();

	if (board_id() <= 2 || board_id() == BOARD_ID_UNKNOWN)
		return;

	/* b:/174121847 Use external OSC to mitigate noise for WWAN sku. */
	if (variant_has_wwan()) {
		soc_cfg->acp_i2s_use_external_48mhz_osc = 1;

		/* eDP phy tuning settings */
		soc_cfg->edp_phy_override = ENABLE_EDP_TUNINGSET;
		/* bit vector of phy, bit0=1: DP0, bit1=1: DP1, bit2=1: DP2 bit3=1: DP3 */
		soc_cfg->edp_physel = 0x1;
		/* override for 0.6v 0db swing 1, pre-emphasis 0 */
		soc_cfg->edp_tuningset.dp_vs_pemph_level = 0x01;
		soc_cfg->edp_tuningset.margin_deemph = 0x004b;
		soc_cfg->edp_tuningset.deemph_6db4 = 0x00;
		soc_cfg->edp_tuningset.boostadj = 0x80;
	}
}

/*
+----------+------+--------+--------+------+--+---------+--+---------+
|          |Vilboz|Vilboz14|Vilboz14|Vilboz|NA|Vilboz360|NA|Vilboz360|
|          |WiFi  |WiFi    |LTE     |LTE   |  |WiFi     |  |LTE      |
+----------+------+--------+--------+------+--+---------+--+---------+
|SAR[25]   |0     |0       |0       |0     |1 |1        |1 |1        |
|SAR[24]   |0     |0       |1       |1     |0 |0        |1 |1        |
|SAR[23]   |0     |1       |0       |1     |0 |1        |0 |1        |
+----------+------+--------+--------+------+--+---------+--+---------+
|SAR_config|0     |1       |2       |3     |4 |5        |6 |7        |
+----------+------+--------+--------+------+--+---------+--+---------+
*/

const char *get_wifi_sar_cbfs_filename(void)
{
	int sar_config;

	sar_config = variant_gets_sar_config();

	switch (sar_config) {
	case 1:
		return "wifi_sar-vilboz-0.hex";
	case 3:
		return "wifi_sar-vilboz-2.hex";
	case 5:
		return "wifi_sar-vilboz-1.hex";
	case 7:
		return "wifi_sar-vilboz-3.hex";
	default:
		return WIFI_SAR_CBFS_DEFAULT_FILENAME;
	}
}
