/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <amdblocks/gpio_banks.h>

/*
 * This function provides base GPIO configuration table. It is typically provided by
 * baseboard using a weak implementation. If GPIO configuration for a variant differs
 * significantly from the baseboard, then the variant can also provide a strong implementation
 * of this function.
 */
const struct soc_amd_gpio *variant_base_gpio_table(size_t *size);
/*
 * This function allows variant to override any GPIOs that are different than the base GPIO
 * configuration provided by variant_base_gpio_table().
 */
const struct soc_amd_gpio *variant_override_gpio_table(size_t *size);

/* This function provides early GPIO init in bootblock or psp. */
const struct soc_amd_gpio *variant_early_gpio_table(size_t *size);

#endif /* __BASEBOARD_VARIANTS_H__ */
