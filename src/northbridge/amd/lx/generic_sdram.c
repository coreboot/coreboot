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

#include <console/console.h>
#include <northbridge/amd/lx/raminit.h>

/* Setup SDRAM */
void sdram_initialize(int controllers, const struct mem_controller *ctrl)
{
	int i;
	/* Set the registers we can set once to reasonable values */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram1.%02x\n", i);
		sdram_set_registers(ctrl + i);
	}

	/* Now setup those things we can auto detect */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_DEBUG, "Ram2.%02x\n", i);
		sdram_set_spd_registers(ctrl + i);
	}

	/* Now that everything is setup enable the SDRAM.
	 * Some chipsets do the work for us while on others
	 * we need to it by hand.
	 */
	printk(BIOS_DEBUG, "Ram3\n");

	sdram_enable(controllers, ctrl);

	printk(BIOS_DEBUG, "Ram4\n");
}
