/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <arch/cpu.h>
#include <arch/early_variables.h>
#include <console/console.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <soc/romstage.h>
#include <program_loading.h>

static inline uint32_t *stack_push(u32 *stack, u32 value)
{
	stack = &stack[-1];
	*stack = value;
	return stack;
}

/* setup_stack_and_mtrrs() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
void *setup_stack_and_mtrrs(void)
{
	int num_mtrrs;
	uint32_t *slot;
	uint32_t mtrr_mask_upper;
	uint32_t top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	slot = (void *)romstage_ram_stack_top();
	num_mtrrs = 0;

	/* The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits. */
	mtrr_mask_upper = (1 << ((cpuid_eax(0x80000008) & 0xff) - 32)) - 1;

	/* The order for each MTRR is value then base with upper 32-bits of
	 * each value coming before the lower 32-bits. The reasoning for
	 * this ordering is to create a stack layout like the following:
	 *   +0: Number of MTRRs
	 *   +4: MTRR base 0 31:0
	 *   +8: MTRR base 0 63:32
	 *  +12: MTRR mask 0 31:0
	 *  +16: MTRR mask 0 63:32
	 *  +20: MTRR base 1 31:0
	 *  +24: MTRR base 1 63:32
	 *  +28: MTRR mask 1 31:0
	 *  +32: MTRR mask 1 63:32
	 */

	/* Cache the ROM as WP just below 4GiB. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CONFIG_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = (uint32_t)cbmem_top();
	/* Cache 8MiB below the top of ram. The top of RAM under 4GiB is the
	 * start of the TSEG region. It is required to be 8MiB aligned. Set
	 * this area as cacheable so it can be used later for ramstage before
	 * setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of ram. Top of RAM is where the TSEG
	 * region resides. However, it is not restricted to SMM mode until
	 * SMM has been relocated. By setting the region to cacheable it
	 * provides faster access when relocating the SMM handler as well
	 * as using the TSEG region for other purposes. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, top_of_ram | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Save the number of MTRRs to setup. Return the stack location
	 * pointing to the number of MTRRs. */
	slot = stack_push(slot, num_mtrrs);

	return slot;
}
