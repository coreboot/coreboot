/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015-2016 Intel Corp.
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

#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/mtrr.h>
#include <fsp/memmap.h>
#include <fsp/romstage.h>
#include <fsp/stack.h>
#include <stdlib.h>
#include <program_loading.h>

/*
 * setup_stack_and_mtrrs() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use.
 */
void *setup_stack_and_mtrrs(void)
{
	size_t alignment;
	uint32_t aligned_ram;
	uint32_t mtrr_mask_upper;
	uint32_t max_mtrrs;
	uint32_t num_mtrrs;
	uint32_t *slot;

	/* Display the MTRRs */
	soc_display_mtrrs();

	/* Top of stack needs to be aligned to a 8-byte boundary. */
	slot = (void *)romstage_ram_stack_top();
	num_mtrrs = 0;
	max_mtrrs = soc_get_variable_mtrr_count(NULL);

	/*
	 * The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits.
	 */
	mtrr_mask_upper = (1 << ((cpuid_eax(0x80000008) & 0xff) - 32)) - 1;
	alignment = mmap_region_granularity();
	aligned_ram = ALIGN_DOWN(romstage_ram_stack_bottom(), alignment);

	/*
	 * The order for each MTRR is value then base with upper 32-bits of
	 * each value coming before the lower 32-bits. The reasoning for
	 * this ordering is to create a stack layout like the following:
	 *
	 *  +36: MTRR mask 1 63:32
	 *  +32: MTRR mask 1 31:0
	 *  +28: MTRR base 1 63:32
	 *  +24: MTRR base 1 31:0
	 *  +20: MTRR mask 0 63:32
	 *  +16: MTRR mask 0 31:0
	 *  +12: MTRR base 0 63:32
	 *   +8: MTRR base 0 31:0
	 *   +4: Number of MTRRs to setup (described above)
	 *   +0: Number of variable MTRRs to clear
	 */

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	slot = stack_push32(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push32(slot, ~(CACHE_TMP_RAMTOP - 1)
		| MTRR_PHYS_MASK_VALID);
	slot = stack_push32(slot, 0); /* upper base */
	slot = stack_push32(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/*
	 *     +-------------------------+  Top of RAM (aligned)
	 *     | System Management Mode  |
	 *     |      code and data      |  Length: CONFIG_TSEG_SIZE
	 *     |         (TSEG)          |
	 *     +-------------------------+  SMM base (aligned)
	 *     |                         |
	 *     | Chipset Reserved Memory |  Length: Multiple of CONFIG_TSEG_SIZE
	 *     |                         |
	 *     +-------------------------+  top_of_ram (aligned)
	 *     |                         |
	 *     |       CBMEM Root        |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |   FSP Reserved Memory   |
	 *     |                         |
	 *     +-------------------------+
	 *     |                         |
	 *     |  Various CBMEM Entries  |
	 *     |                         |
	 *     +-------------------------+  top_of_stack (8 byte aligned)
	 *     |                         |
	 *     |   stack (CBMEM Entry)   |
	 *     |                         |
	 *     +-------------------------+
	 */

	/*
	 * Cache the stack and the other CBMEM entries as well as part or all
	 * of the FSP reserved memory region.
	 */
	slot = stack_push32(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push32(slot, ~(alignment - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push32(slot, 0); /* upper base */
	slot = stack_push32(slot, aligned_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

#if IS_ENABLED(CONFIG_HAVE_SMI_HANDLER)
	void *smm_base;
	size_t smm_size;
	uint32_t tseg_base;

	/*
	 * Cache the TSEG region at the top of ram. This region is not
	 * restricted to SMM mode until SMM has been relocated. By setting
	 * the region to cacheable it provides faster access when relocating
	 * the SMM handler as well as using the TSEG region for other purposes.
	 */
	smm_region(&smm_base, &smm_size);
	tseg_base = (uint32_t)smm_base;
	slot = stack_push32(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push32(slot, ~(alignment - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push32(slot, 0); /* upper base */
	slot = stack_push32(slot, tseg_base | MTRR_TYPE_WRBACK);
	num_mtrrs++;
#endif

	/* Cache the ROM as WP just below 4GiB. */
	slot = stack_push32(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push32(slot, ~(CONFIG_ROM_SIZE - 1)
		| MTRR_PHYS_MASK_VALID);
	slot = stack_push32(slot, 0); /* upper base */
	slot = stack_push32(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Validate the MTRR usage */
	if (num_mtrrs > max_mtrrs) {
		printk(BIOS_ERR, "MTRRs: max = %d, used = %d, available=%d",
			max_mtrrs, num_mtrrs, max_mtrrs - num_mtrrs);
		die("ERROR - MTRR use count incorrect!\n");
	}

	/*
	 * Save the number of MTRRs to setup and clear.  Return the stack
	 * location pointing to the number of MTRRs.
	 */
	slot = stack_push32(slot, num_mtrrs);
	slot = stack_push32(slot, max_mtrrs);
	return slot;
}
