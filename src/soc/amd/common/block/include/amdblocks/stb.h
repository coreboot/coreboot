/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_STB_H
#define AMD_BLOCK_STB_H

#include <types.h>

#define AMD_STB_PMI_0			0x30600

#define AMD_STB_COREBOOT_POST_PREFIX	0xBA000000
#define AMD_STB_COREBOOT_MARKER		0xBAADF00D

struct stb_entry_struct {
	uint32_t ts;
	uint32_t val;
};

void write_stb_to_console(void);
void add_stb_to_timestamp_buffer(void);

#endif /* AMD_BLOCK_STB_H */
