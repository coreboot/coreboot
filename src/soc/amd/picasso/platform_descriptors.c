/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <static.h>
#include <types.h>

#include "chip.h"

struct picasso_smu_telemetry picasso_get_smu_telemetry(void)
{
	const struct soc_amd_picasso_config *config = config_of_soc();
	struct picasso_smu_telemetry telemetry = {
		.vdd_slope = config->telemetry_vddcr_vdd_slope_mA,
		.vdd_offset = config->telemetry_vddcr_vdd_offset,
		.soc_slope = config->telemetry_vddcr_soc_slope_mA,
		.soc_offset = config->telemetry_vddcr_soc_offset,
	};

	mainboard_update_picasso_smu_telemetry(&telemetry);
	return telemetry;
}

void __weak mainboard_update_picasso_smu_telemetry(struct picasso_smu_telemetry *telemetry)
{
}
