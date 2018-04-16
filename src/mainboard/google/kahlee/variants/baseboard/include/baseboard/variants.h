/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google Inc.
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

#include <stddef.h>
#include <soc/smi.h>
#include <soc/southbridge.h>

const struct sci_source *get_gpe_table(size_t *num);
uint8_t variant_memory_sku(void);
/* Return board SKU. Limited to uint8_t, so it fits into 3 decimal digits */
uint8_t variant_board_sku(void);
int variant_mainboard_read_spd(uint8_t spdAddress, char *buf, size_t len);
int variant_get_xhci_oc_map(uint16_t *usb_oc_map);
int variant_get_ehci_oc_map(uint16_t *usb_oc_map);
const struct soc_amd_stoneyridge_gpio *variant_early_gpio_table(size_t *size);
const struct soc_amd_stoneyridge_gpio *variant_gpio_table(size_t *size);
void variant_romstage_entry(int s3_resume);

#endif /* __BASEBOARD_VARIANTS_H__ */
