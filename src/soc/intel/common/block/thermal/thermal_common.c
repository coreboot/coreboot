/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/cfg.h>
#include <intelblocks/thermal.h>

/* Get PCH Thermal Trip from common chip config */
uint8_t get_thermal_trip_temp(void)
{
	const struct soc_intel_common_config *common_config;
	common_config = chip_get_common_soc_structure();

	return common_config->pch_thermal_trip;
}

/* PCH Low Temp Threshold (LTT) */
uint32_t pch_get_ltt_value(void)
{
	uint8_t thermal_config;

	thermal_config = get_thermal_trip_temp();
	if (!thermal_config)
		thermal_config = DEFAULT_TRIP_TEMP;

	if (thermal_config > MAX_TRIP_TEMP)
		die("Input PCH temp trip is higher than allowed range!");

	return GET_LTT_VALUE(thermal_config);
}
