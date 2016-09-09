/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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
#include <stdlib.h>

#include <arch/io.h>
#include <arch/exception.h>
#include <cbmem.h>
#include <console/console.h>
#include <program_loading.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/mmu_operations.h>
#include <soc/sdram.h>

extern struct mvmap2315_sdram_params ddr_params;

void main(void)
{
	console_init();

	exception_init();

	clock_init();

	sdram_init(&ddr_params);

	write32((void *)MVMAP2315_BOOTBLOCK_CB1, 0x4);

	while (read32((void *)MVMAP2315_BOOTBLOCK_CB2) != 0x4)
		;

	mvmap2315_mmu_init();

	cbmem_initialize_empty();

	run_ramstage();
}
