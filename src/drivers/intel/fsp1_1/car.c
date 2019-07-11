/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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

#include <arch/symbols.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <cpu/intel/romstage.h>
#include <cpu/x86/mtrr.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <fsp/memmap.h>
#include <program_loading.h>

/* platform_enter_postcar() determines the stack to use after
 * cache-as-ram is torn down as well as the MTRR settings to use,
 * and continues execution in postcar stage. */
void platform_enter_postcar(void)
{
	struct postcar_frame pcf;
	uintptr_t top_of_ram;

	if (postcar_frame_init(&pcf, 0))
		die("Unable to initialize postcar frame.\n");
	/* Cache the ROM as WP just below 4GiB. */
	postcar_frame_add_mtrr(&pcf, CACHE_ROM_BASE, CACHE_ROM_SIZE,
			       MTRR_TYPE_WRPROT);

	/* Cache RAM as WB from 0 -> CACHE_TMP_RAMTOP. */
	postcar_frame_add_mtrr(&pcf, 0, CACHE_TMP_RAMTOP, MTRR_TYPE_WRBACK);

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations. */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(&pcf, top_of_ram - 8*MiB, 16*MiB, MTRR_TYPE_WRBACK);

	run_postcar_phase(&pcf);
}

/* This is the romstage entry called from cpu/intel/car/romstage.c */
void mainboard_romstage_entry(unsigned long bist)
{
	/* Need to locate the current FSP_INFO_HEADER. The cache-as-ram
	 * is still enabled. We can directly access work buffer here. */
	struct prog fsp = PROG_INIT(PROG_REFCODE, "fsp.bin");

	if (prog_locate(&fsp))
		die_with_post_code(POST_INVALID_CBFS, "Unable to locate fsp.bin");

	/* This leaks a mapping which this code assumes is benign as
	 * the flash is memory mapped CPU's address space. */
	FSP_INFO_HEADER *fih = find_fsp((uintptr_t)rdev_mmap_full(prog_rdev(&fsp)));

	if (!fih)
		die("Invalid FSP header\n");

	cache_as_ram_stage_main(fih);
}
