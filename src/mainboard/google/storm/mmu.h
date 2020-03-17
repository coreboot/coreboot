/*
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <types.h>

extern u8 _rpm[];
extern u8 _erpm[];

enum dram_state {
	DRAM_INITIALIZED = 0,
	DRAM_NOT_INITIALIZED = 1,
};

void setup_dram_mappings(enum dram_state dram);
void setup_mmu(enum dram_state);
