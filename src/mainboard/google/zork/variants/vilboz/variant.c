/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>
#include <fw_config.h>
#include <sar.h>

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

	/* b:/174121847 Use external OSC to mitigate noise for WWAN sku. */
	if (variant_has_wwan())
		soc_cfg->acp_i2s_use_external_48mhz_osc = 1;
}

/*
+----------+------+--------+--------+------+--+---------+--+---------+
|          |Vilboz|Vilboz14|Vilboz14|Vilboz|NA|Vilboz360|NA|Vilboz360|
|          |WiFi  |WiFi    |LTE     |LTE   |  |WiFi     |  |LTE      |
+----------+------+--------+--------+------+--+---------+--+---------+
|SAR[26]   |0     |0       |0       |0     |0 |0        |0 |0        |
|SAR[25]   |0     |0       |0       |0     |1 |1        |1 |1        |
|SAR[24]   |0     |0       |1       |1     |0 |0        |1 |1        |
|SAR[23]   |0     |1       |0       |1     |0 |1        |0 |1        |
+----------+------+--------+--------+------+--+---------+--+---------+
|SAR_config|0     |1       |2       |3     |4 |5        |6 |7        |
+----------+------+--------+--------+------+--+---------+--+---------+
*/

const char *get_wifi_sar_cbfs_filename(void)
{
	const char *filename = NULL;
	int sar_config;

	sar_config = variant_gets_sar_config();

	switch (sar_config) {
	case 1:
		filename = "wifi_sar-vilboz-0.hex";
		break;
	case 3:
		/*
		TODO: Set default first. It will be replaced after the
		new table is generated.
		*/
		filename = "wifi_sar_defaults.hex";
		break;
	case 5:
		filename = "wifi_sar-vilboz-1.hex";
		break;
	case 7:
		filename = "wifi_sar-vilboz-1.hex";
		break;
	}

	return filename;
}
