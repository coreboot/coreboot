/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <arch/romstage.h>
#include <arch/symbols.h>
#include <cbmem.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <fsp/car.h>
#include <fsp/util.h>
#include <program_loading.h>

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations. */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 16*MiB, MTRR_TYPE_WRBACK);

}

/* This is the romstage entry called from cpu/intel/car/romstage.c */
void mainboard_romstage_entry(void)
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
