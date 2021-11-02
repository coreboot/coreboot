/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <chip.h>
#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>

/* The next set of functions return the gpio table and fill in the number of entries for
 * each table.
 */

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_gpio_override_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_romstage_gpio_table(size_t *num);

const struct mb_cfg *variant_memory_params(void);
void variant_get_spd_info(struct mem_spd *spd_info);
int variant_memory_sku(void);
bool variant_is_half_populated(void);
void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config);
void variant_fill_ssdt(const struct device *dev);

enum s0ix_entry {
	S0IX_EXIT,
	S0IX_ENTRY,
};

void variant_generate_s0ix_hook(enum s0ix_entry);

/* Modify devictree settings during ramstage */
void variant_devtree_update(void);

struct cpu_power_limits {
	uint16_t mchid;
	u8 cpu_tdp;
	unsigned int pl1_min_power;
	unsigned int pl1_max_power;
	unsigned int pl2_min_power;
	unsigned int pl2_max_power;
	unsigned int pl4_power;
};

struct system_power_limits {
	uint16_t mchid;
	u8 cpu_tdp;
	/* PsysPL2 in Watts */
	unsigned int psys_pl2_power;
};

struct psys_config {
	/*
	 * The efficiency of type-c chargers
	 * For example, 'efficiency = 97' means setting 97% of max power to account for
	 * cable loss and FET Rdson loss in the path from the source.
	 */
	unsigned int efficiency;

	/* The maximum current maps to the Psys signal */
	unsigned int psys_imax_ma;

	/* The voltage of barrel jack */
	unsigned int bj_volts_mv;
};

/* Modify Power Limit devictree settings during ramstage */
void variant_update_power_limits(const struct cpu_power_limits *limits,
				 size_t num_entries);


/*
 * Modify Power Limit and PsysPL devictree settings during ramstage.
 * Note, this function must be called in front of calling variant_update_power_limits.
 */
void variant_update_psys_power_limits(const struct cpu_power_limits *limits,
					const struct system_power_limits *sys_limits,
					size_t num_entries,
					const struct psys_config *config);

#endif /*__BASEBOARD_VARIANTS_H__ */
