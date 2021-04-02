/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __BOOTBLOCK_COMMON_H
#define __BOOTBLOCK_COMMON_H

#include <arch/cpu.h>
#include <main_decl.h>
#include <timestamp.h>
#include <types.h>

/*
 * These are defined as weak no-ops that can be overridden by mainboard/SoC.
 * The 'early' variants are called prior to console initialization. Also, the
 * SoC functions are called prior to the mainboard functions.
 */
void decompressor_soc_init(void);
void bootblock_mainboard_early_init(void);
void bootblock_mainboard_init(void);
void bootblock_soc_early_init(void);
void bootblock_soc_init(void);

/*
 * C code entry point for the boot block.
 */
asmlinkage void bootblock_c_entry(uint64_t base_timestamp);
asmlinkage void bootblock_c_entry_bist(uint64_t base_timestamp, uint32_t bist);

/* To be used when APs execute through bootblock too. */
asmlinkage void ap_bootblock_c_entry(void);

void bootblock_main_with_basetime(uint64_t base_timestamp);
void bootblock_main_with_timestamp(uint64_t base_timestamp,
	struct timestamp_entry *timestamps, size_t num_timestamps);

/* This is the argument structure passed from decompressor to bootblock. */
struct bootblock_arg {
	uint64_t base_timestamp;
	void *metadata_hash_anchor;
	uint32_t num_timestamps;
	struct timestamp_entry timestamps[];
};

#endif	/* __BOOTBLOCK_COMMON_H */
