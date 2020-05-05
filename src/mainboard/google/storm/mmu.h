/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

extern u8 _rpm[];
extern u8 _erpm[];

enum dram_state {
	DRAM_INITIALIZED = 0,
	DRAM_NOT_INITIALIZED = 1,
};

void setup_dram_mappings(enum dram_state dram);
void setup_mmu(enum dram_state);
