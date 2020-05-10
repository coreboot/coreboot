/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PS8625_H__
#define __PS8625_H__

#include <stdint.h>

struct parade_write {
	uint8_t offset;
	uint8_t reg;
	uint8_t val;
};

void parade_ps8625_bridge_setup(unsigned int bus, unsigned int chip_base,
				const struct parade_write *,
				int parade_write_count);

#endif
