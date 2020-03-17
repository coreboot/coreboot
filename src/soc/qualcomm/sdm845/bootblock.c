/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <soc/mmu.h>
#include <soc/clock.h>
#include <soc/qspi.h>

void bootblock_soc_init(void)
{
	clock_init();
	sdm845_mmu_init();
	quadspi_init(25 * MHz);
}
