/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Kyösti Mälkki <kyosti.malkki@gmail.com>
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
#include <arch/romstage.h>

#include <southbridge/intel/i82801dx/i82801dx.h>
#include <northbridge/intel/e7505/raminit.h>

void mainboard_romstage_entry(void)
{
	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	i82801dx_early_init();

	sdram_initialize();

	cbmem_recovery(0);
}
