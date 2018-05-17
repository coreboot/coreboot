/*
 * This file is part of the coreboot project.
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

// Use simple device model for this file even in ramstage
#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <program_loading.h>
#include "e7505.h"

void *cbmem_top(void)
{
	pci_devfn_t mch = PCI_DEV(0, 0, 0);
	uintptr_t tolm;

	/* This is at 128 MiB boundary. */
	tolm = pci_read_config16(mch, TOLM) >> 11;
	tolm <<= 27;

	return (void *)tolm;
}

#define ROMSTAGE_RAM_STACK_SIZE 0x5000

/* setup_stack_and_mtrrs() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use. */
void *setup_stack_and_mtrrs(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;

	if (postcar_frame_init(&pcf, ROMSTAGE_RAM_STACK_SIZE))
		die("Unable to initialize postcar frame.\n");

	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE,
		MTRR_TYPE_WRPROT);

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	postcar_frame_add_mtrr(&pcf, 0, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

	/* Cache CBMEM region as WB. */
	top_of_ram = (uintptr_t)cbmem_top();
	postcar_frame_add_mtrr(&pcf, top_of_ram - 8*MiB, 8*MiB,
		MTRR_TYPE_WRBACK);

	/* Save the number of MTRRs to setup. Return the stack location
	 * pointing to the number of MTRRs.
	 */
	return postcar_commit_mtrrs(&pcf);
}
