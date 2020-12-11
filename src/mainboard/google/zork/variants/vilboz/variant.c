/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <soc/pci_devs.h>

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
