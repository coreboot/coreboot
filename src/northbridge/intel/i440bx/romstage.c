/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/romstage.h>
#include <cbmem.h>
#include <southbridge/intel/i82371eb/i82371eb.h>
#include <northbridge/intel/i440bx/raminit.h>

void mainboard_romstage_entry(void)
{
	i82371eb_early_init();

	sdram_initialize();
	cbmem_initialize_empty();
}
