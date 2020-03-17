/*
 * This file is part of the coreboot project.
 *
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

#include <arch/romstage.h>
#include <cbmem.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <soc/iosf.h>

static uintptr_t smm_region_start(void)
{
	return (iosf_bunit_read(BUNIT_SMRRL) << 20);
}

static size_t smm_region_size(void)
{
	return CONFIG_SMM_TSEG_SIZE;
}

void *cbmem_top_chipset(void)
{
	return (void *) smm_region_start();
}

void smm_region(uintptr_t *start, size_t *size)
{
	*start = (iosf_bunit_read(BUNIT_SMRRL) & 0xFFFF) << 20;
	*size = smm_region_size();
}

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;

	/* Cache at least 8 MiB below the top of ram, and at most 8 MiB
	 * above top of the ram. This satisfies MTRR alignment requirement
	 * with different TSEG size configurations.
	 */
	top_of_ram = ALIGN_DOWN((uintptr_t)cbmem_top(), 8*MiB);
	postcar_frame_add_mtrr(pcf, top_of_ram - 8*MiB, 16*MiB,
			       MTRR_TYPE_WRBACK);
}
