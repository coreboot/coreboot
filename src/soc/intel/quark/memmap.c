/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015-2016 Intel Corporation.
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

#include <arch/cpu.h>
#include <arch/romstage.h>
#include <cbmem.h>
#include <soc/reg_access.h>

void fill_postcar_frame(struct postcar_frame *pcf)
{
	uintptr_t top_of_ram;
	uintptr_t top_of_low_usable_memory;

	/* Locate the top of RAM */
	top_of_low_usable_memory = (uintptr_t) cbmem_top();
	top_of_ram = ALIGN(top_of_low_usable_memory, 16 * MiB);

	/* Cache postcar and ramstage */
	postcar_frame_add_mtrr(pcf, top_of_ram - (16 * MiB), 16 * MiB,
		MTRR_TYPE_WRBACK);

	/* Cache RMU area */
	postcar_frame_add_mtrr(pcf, (uintptr_t) top_of_low_usable_memory,
		0x10000, MTRR_TYPE_WRTHROUGH);

	/* Cache ESRAM */
	postcar_frame_add_mtrr(pcf, 0x80000000, 0x80000, MTRR_TYPE_WRBACK);

	pcf->skip_common_mtrr = 1;
	/* Cache SPI flash - Write protect not supported */
	postcar_frame_add_romcache(pcf, MTRR_TYPE_WRTHROUGH);
}
