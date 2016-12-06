/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2012 ChromeOS Authors
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

#define __SIMPLE_DEVICE__

#include <arch/cpu.h>
#include <arch/io.h>
#include <cbmem.h>
#include <cpu/cpu.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include "sandybridge.h"

#if (CONFIG_SMM_TSEG_SIZE < 0x800000)
# error "CONFIG_SMM_TSEG_SIZE must at least be 8MiB"
#endif
#if ((CONFIG_SMM_TSEG_SIZE & (CONFIG_SMM_TSEG_SIZE - 1)) != 0)
# error "CONFIG_SMM_TSEG_SIZE is not a power of 2"
#endif

static uintptr_t smm_region_start(void)
{
	/* Base of TSEG is top of usable DRAM */
	uintptr_t tom = pci_read_config32(PCI_DEV(0,0,0), TSEG);
	return tom;
}

void *cbmem_top(void)
{
	return (void *) smm_region_start();
}

static inline u32 *stack_push(u32 *stack, u32 value)
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
	u32 *slot;
	u32 mtrr_mask_upper;
	u32 top_of_ram;

	/* Top of stack needs to be aligned to a 4-byte boundary. */
	slot = (void *)romstage_ram_stack_top();
	num_mtrrs = 0;

	/* The upper bits of the MTRR mask need to set according to the number
	 * of physical address bits. */
	mtrr_mask_upper = (1 << (cpu_phys_address_size() - 32)) - 1;

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
	slot = stack_push(slot, ~(CACHE_ROM_SIZE - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, ~(CACHE_ROM_SIZE - 1) | MTRR_TYPE_WRPROT);
	num_mtrrs++;

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~(CACHE_TMP_RAMTOP - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, 0 | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	top_of_ram = (uint32_t)cbmem_top();
	/* Cache 8MiB below the top of ram. On sandybridge systems the top of
	 * ram under 4GiB is the start of the TSEG region. It is required to
	 * be 8MiB aligned. Set this area as cacheable so it can be used later
	 * for ramstage before setting up the entire RAM as cacheable. */
	slot = stack_push(slot, mtrr_mask_upper); /* upper mask */
	slot = stack_push(slot, ~((8 << 20) - 1) | MTRR_PHYS_MASK_VALID);
	slot = stack_push(slot, 0); /* upper base */
	slot = stack_push(slot, (top_of_ram - (8 << 20)) | MTRR_TYPE_WRBACK);
	num_mtrrs++;

	/* Cache 8MiB at the top of ram. Top of ram on sandybridge systems
	 * is where the TSEG region resides. However, it is not restricted
	 * to SMM mode until SMM has been relocated. By setting the region
	 * to cacheable it provides faster access when relocating the SMM
	 * handler as well as using the TSEG region for other purposes. */
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
