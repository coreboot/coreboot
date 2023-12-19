/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <console/console.h>
#include <gpio.h>
#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_ddi_descriptor myst_ddi_descriptors[] = {
	{ /* DDI0 - eDP */
		.connector_type = DDI_EDP,
		.aux_index = DDI_AUX1,
		.hdp_index = DDI_HDP1
	},
	{ /* DDI1 - HDMI/DP */
		.connector_type = DDI_HDMI,
		.aux_index = DDI_AUX2,
		.hdp_index = DDI_HDP2
	},
	{ /* DDI2 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX3,
		.hdp_index = DDI_HDP3,
	},
	{ /* DDI3 - DP (type C) */
		.connector_type = DDI_DP_W_TYPEC,
		.aux_index = DDI_AUX4,
		.hdp_index = DDI_HDP4,
	},
	{ /* DDI4 - Unused */
		.connector_type = DDI_UNUSED_TYPE,
		.aux_index = DDI_AUX5,
		.hdp_index = DDI_HDP5,
	},
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	variant_get_dxio_descriptors(dxio_descs, dxio_num);
	*ddi_descs = myst_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(myst_ddi_descriptors);
}
