/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <stdint.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

void variant_smi_sleep(u8 slp_typ);

struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);
void variant_nhlt_oem_overrides(const char **oem_id, const char **oem_table_id,
				uint32_t *oem_revision);

void variant_chip_display_init(void);

#endif /* __BASEBOARD_VARIANTS_H__ */
