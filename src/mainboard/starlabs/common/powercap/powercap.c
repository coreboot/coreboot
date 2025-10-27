/* SPDX-License-Identifier: GPL-2.0-only */

#include <option.h>
#include <types.h>
#include <common/powercap.h>

static enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}


void update_power_limits(config_t *cfg)
{
	uint8_t performance_scale = 100;

	/* Scale PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		performance_scale -= 50;
		cfg->tcc_offset = TCC(80);
		break;
	case PP_BALANCED:
		performance_scale -= 25;
		cfg->tcc_offset = TCC(90);
		break;
	case PP_PERFORMANCE:
		/* Use the Intel defaults */
		cfg->tcc_offset = TCC(100);
		break;
	}

	struct soc_power_limits_config *limits =
		(struct soc_power_limits_config *)&cfg->power_limits_config;
	size_t limit_count = sizeof(cfg->power_limits_config) /
			     sizeof(struct soc_power_limits_config);

	for (size_t i = 0; i < limit_count; i++) {
		struct soc_power_limits_config *entry = &limits[i];

		entry->tdp_pl4 = (uint16_t)CONFIG_PL4_WATTS;

		if (!entry->tdp_pl2_override)
			continue;

		/* Set PL1 to 50% of PL2 */
		entry->tdp_pl1_override = (entry->tdp_pl2_override / 2) & ~1;

		if (performance_scale == 100)
			continue;

		entry->tdp_pl1_override = ((entry->tdp_pl1_override * performance_scale) / 100);
		entry->tdp_pl2_override = ((entry->tdp_pl2_override * performance_scale) / 100);
	}
}
