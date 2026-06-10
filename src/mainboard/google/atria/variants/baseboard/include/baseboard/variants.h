/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <fsp/api.h>
#include <soc/gpio.h>
#include <soc/meminit.h>

/**
 * @brief Get the variant GPIO table
 *
 * @param[out] num  Number of entries in the table
 *
 * @return Pointer to the variant GPIO table
 */
const struct pad_config *variant_gpio_table(size_t *num);

/**
 * @brief Get the variant early GPIO table
 *
 * @param[out] num  Number of entries in the table
 *
 * @return Pointer to the variant early GPIO table
 */
const struct pad_config *variant_early_gpio_table(size_t *num);

/**
 * @brief Get the variant romstage GPIO table
 *
 * @param[out] num  Number of entries in the table
 *
 * @return Pointer to the variant romstage GPIO table
 */
const struct pad_config *variant_romstage_gpio_table(size_t *num);

/**
 * @brief Get the variant memory parameters
 *
 * @return Pointer to the variant memory parameters
 */
const struct mb_cfg *variant_memory_params(void);

/**
 * @brief Get the variant SPD info
 *
 * @param[out] spd_info Pointer to the SPD info structure
 */
void variant_get_spd_info(struct mem_spd *spd_info);

/**
 * @brief Get the variant memory SKU
 *
 * @return Memory SKU
 */
int variant_memory_sku(void);

/**
 * @brief Check if the variant is half populated
 *
 * @return True if the variant is half populated, false otherwise
 */
bool variant_is_half_populated(void);

/**
 * @brief Update the variant SOC memory init parameters
 *
 * @param[in,out] memupd Pointer to the FSPM_UPD structure
 */
void variant_update_soc_memory_init_params(FSPM_UPD *memupd);

void fw_config_configure_pre_mem_gpio(void);

void fw_config_gpio_padbased_override(struct pad_config *padbased_table);

#endif /* __BASEBOARD_VARIANTS_H__ */
