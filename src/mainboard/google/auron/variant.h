/*
 * This file is part of the coreboot project.
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

#ifndef VARIANT_H
#define VARIANT_H

#include <arch/io.h>
#include <device/device.h>
#include <soc/romstage.h>

int variant_smbios_data(device_t dev, int *handle, unsigned long *current);
void variant_romstage_entry(struct romstage_params *rp);
void lan_init(void);

#endif
