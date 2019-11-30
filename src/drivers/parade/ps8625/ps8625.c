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

#include <device/i2c_simple.h>
#include <stdint.h>

#include "ps8625.h"

void parade_ps8625_bridge_setup(unsigned int bus, unsigned int chip_base,
				const struct parade_write *parade_writes,
				int parade_write_count)
{
	int i;

	for (i = 0; i < parade_write_count; i++) {
		const struct parade_write *w = &parade_writes[i];
		i2c_writeb(bus, chip_base + w->offset, w->reg, w->val);
	}
}
