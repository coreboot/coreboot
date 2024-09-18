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
void fw_config_gpio_padbased_override(struct pad_config *padbased_table);

const struct mb_cfg *variant_memory_params(void);
void variant_get_spd_info(struct mem_spd *spd_info);
int variant_memory_sku(void);
bool variant_is_half_populated(void);
void variant_update_soc_chip_config(struct soc_intel_alderlake_config *config);
void variant_fill_ssdt(const struct device *dev);
void variant_configure_pads(void);
void variant_smi_sleep(u8 slp_typ);

enum s0ix_entry {
	S0IX_EXIT,
	S0IX_ENTRY,
};

void variant_generate_s0ix_hook(enum s0ix_entry entry);

/* Modify devictree settings during ramstage */
void variant_devtree_update(void);

void variant_update_descriptor(void);

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
};

/* Modify Power Limit devictree settings during ramstage */
void variant_update_power_limits(const struct cpu_power_limits *limits,
				 size_t num_entries);
void variant_init(void);
void variant_finalize(void);

#endif /*__BASEBOARD_VARIANTS_H__ */
