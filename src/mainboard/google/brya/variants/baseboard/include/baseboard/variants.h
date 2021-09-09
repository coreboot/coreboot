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
const struct cros_gpio *variant_cros_gpios(size_t *num);
const struct pad_config *variant_romstage_gpio_table(size_t *num);

const struct mb_cfg *variant_memory_params(void);
int variant_memory_sku(void);
bool variant_is_half_populated(void);
void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config);

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

/* Modify Power Limit devictree settings during ramstage */
void variant_update_power_limits(const struct cpu_power_limits *limits,
				 size_t num_entries);

#endif /*__BASEBOARD_VARIANTS_H__ */
