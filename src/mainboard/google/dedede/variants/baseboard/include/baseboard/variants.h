/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <stdint.h>
#include <acpi/acpi_device.h>

/* The next set of functions return the gpio table and fill in the number of
 * entries for each table. */

const struct pad_config *baseboard_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);
const struct pad_config *variant_sleep_gpio_table(size_t *num);
const struct pad_config *variant_override_gpio_table(size_t *num);
const struct pad_config *variant_romstage_gpio_table(size_t *num);

enum s0ix_entry {
	S0IX_EXIT,
	S0IX_ENTRY,
};

/**
 * Get board's Hardware features as defined in FW_CONFIG
 *
 * @param fw_config	Address where the fw_config is stored.
 * @return 0 on success or negative integer for errors.
 */
int board_info_get_fw_config(uint64_t *fw_config);

/* Return memory configuration structure. */
const struct mb_cfg *variant_memcfg_config(void);

/* Return memory SKU for the variant */
int variant_memory_sku(void);

/**
 * Get data whether memory channel is half-populated or not
 *
 * @return false on boards where memory channel is half-populated, true otherwise.
 */
bool variant_mem_is_half_populated(void);

/* Allow each variants to customize SMI sleep flow. */
void variant_smi_sleep(u8 slp_typ);

/* Modify devictree settings during ramstage. */
void variant_devtree_update(void);

/* Modify LTE devictree settings during ramstage. */
void update_lte_device(struct acpi_gpio *lte_reset_gpio, struct acpi_gpio *lte_enable_gpio);

void variant_generate_s0ix_hook(enum s0ix_entry);
#endif /*__BASEBOARD_VARIANTS_H__ */
