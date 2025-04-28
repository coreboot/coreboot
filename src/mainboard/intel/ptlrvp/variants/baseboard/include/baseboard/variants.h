/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <chip.h>
#include <fsp/api.h>
#include <soc/gpio.h>
#include <soc/meminit.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

enum ptl_boardid {
	PTLP_LP5_T3_RVP = 0x01,
	PTLP_LP5_T4_RVP = 0x03,
	GCS_32GB = 0x11,
	GCS_64GB = 0x12,
};

/* The next set of functions return the gpio table and fill in the number of entries for
 * each table.
 */

const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_romstage_gpio_table(size_t *num);
void fw_config_configure_pre_mem_gpio(void);
void fw_config_gpio_padbased_override(struct pad_config *padbased_table);

const struct mb_cfg *variant_memory_params(void);
void variant_get_spd_info(struct mem_spd *spd_info);
int variant_memory_sku(void);
bool variant_is_half_populated(void);
void variant_update_soc_memory_init_params(FSPM_UPD *memupd);
void variant_update_soc_chip_config(struct soc_intel_pantherlake_config *config);

enum s0ix_entry {
	S0IX_EXIT,
	S0IX_ENTRY,
};

void variant_generate_s0ix_hook(enum s0ix_entry entry);

/* Modify devictree settings during ramstage by baseboard */
void baseboard_devtree_update(void);
/* Modify devictree settings during ramstage by dedicated variant */
void variant_devtree_update(void);

#endif /*__BASEBOARD_VARIANTS_H__ */
