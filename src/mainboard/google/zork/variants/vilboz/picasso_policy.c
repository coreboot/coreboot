/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <soc/platform_descriptors.h>

void mainboard_update_picasso_smu_telemetry(struct picasso_smu_telemetry *telemetry)
{
	/* The MAINBOARD_TYPE in FW_CONFIG selects an alternate calibration */
	if (!variant_gets_mb_type_config())
		return;

	telemetry->vdd_slope = 32453;
	telemetry->vdd_offset = 168;
	telemetry->soc_slope = 22644;
	telemetry->soc_offset = -70;
}
