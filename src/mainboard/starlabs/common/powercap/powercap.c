/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/helpers.h>
#include <option.h>
#include <types.h>
#include <common/powercap.h>

static enum cmos_power_profile get_power_profile(enum cmos_power_profile fallback)
{
	const unsigned int power_profile = get_uint_option("power_profile", fallback);
	return power_profile < NUM_POWER_PROFILES ? power_profile : fallback;
}

static uint16_t round_up_to_5(uint16_t value)
{
	return DIV_ROUND_UP(value, 5) * 5;
}

void update_power_limits(config_t *cfg)
{
	uint8_t performance_scale = 100;
	uint32_t performance_tcc_offset = CONFIG(EC_STARLABS_FAN) ? 10 : 20;

	/* Scale PL1 & PL2 based on CMOS settings */
	switch (get_power_profile(PP_POWER_SAVER)) {
	case PP_POWER_SAVER:
		performance_scale -= 50;
		cfg->tcc_offset = performance_tcc_offset + 20;
		break;
	case PP_BALANCED:
		performance_scale -= 25;
		cfg->tcc_offset = performance_tcc_offset + 10;
		break;
	case PP_PERFORMANCE:
		cfg->tcc_offset = performance_tcc_offset;
		break;
	}

	struct soc_power_limits_config *limits =
		(struct soc_power_limits_config *)&cfg->power_limits_config;
	size_t limit_count =
		sizeof(cfg->power_limits_config) / sizeof(struct soc_power_limits_config);

	for (size_t i = 0; i < limit_count; i++) {
		struct soc_power_limits_config *entry = &limits[i];
		uint16_t tdp, pl1, pl2;

		entry->tdp_pl4 = (uint16_t)CONFIG_PL4_WATTS;

		tdp = entry->tdp_pl1_override;
		if (!tdp)
			continue;

		pl1 = (tdp * performance_scale) / 100;
		pl2 = round_up_to_5(pl1 * 2);

		entry->tdp_pl1_override = pl1;
		entry->tdp_pl2_override = pl2;
	}
}
