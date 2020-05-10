/* SPDX-License-Identifier: GPL-2.0-only */

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
