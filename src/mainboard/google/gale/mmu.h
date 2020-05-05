/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>

extern u8 _wifi_imem_0[];	/* Linker script supplied */
extern u8 _ewifi_imem_0[];
extern u8 _wifi_imem_1[];
extern u8 _ewifi_imem_1[];
extern u8 _oc_imem[];
extern u8 _eoc_imem[];

enum dram_state {
	DRAM_INITIALIZED = 0,
	DRAM_NOT_INITIALIZED = 1,
};

void setup_dram_mappings(enum dram_state dram);
void setup_mmu(enum dram_state);
