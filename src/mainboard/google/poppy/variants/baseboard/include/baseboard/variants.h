/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __BASEBOARD_VARIANTS_H__
#define __BASEBOARD_VARIANTS_H__

#include <soc/gpio.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

/*
 * The next set of functions return the gpio table and fill in the number of
 * entries for each table.
 */
const struct pad_config *variant_gpio_table(size_t *num);
const struct pad_config *variant_early_gpio_table(size_t *num);

const struct cros_gpio *variant_cros_gpios(size_t *num);
/* Config gpio by different sku id */
const struct pad_config *variant_sku_gpio_table(size_t *num);

enum memory_type {
	MEMORY_LPDDR3,
	MEMORY_DDR4,
	MEMORY_COUNT,
};

struct memory_params {
	enum memory_type type;
	const void *dq_map;
	size_t dq_map_size;
	const void *dqs_map;
	size_t dqs_map_size;
	const void *rcomp_resistor;
	size_t rcomp_resistor_size;
	const void *rcomp_target;
	size_t rcomp_target_size;
	bool use_sec_spd;
};

void variant_memory_params(struct memory_params *p);
int variant_memory_sku(void);
void variant_devtree_update(void);
uint16_t variant_board_sku(void);

struct nhlt;
void variant_nhlt_init(struct nhlt *nhlt);
void variant_nhlt_oem_overrides(const char **oem_id, const char **oem_table_id,
				uint32_t *oem_revision);

#endif /* __BASEBOARD_VARIANTS_H__ */
