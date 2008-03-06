/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Ronald G. Minnich <rminnich@gmail.com>
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <cpu.h>
#include <console.h>

/**
 * Print an error message which says why the RAM initialization failed,
 * then halt the processor(s).
 *
 * @param why The reason why the RAM initialization failed.
 */
void ram_failure(const char *why)
{
	printk(BIOS_EMERG, "RAM failure: %s: Halting\n", why);
	hlt();
}

/* Northbridge or memory controller code must define these functions. */
void ram_set_registers(void *ctrl, int i);
int ram_set_spd_registers(void *ctrl, int i);
void ram_enable(int controllers, void *ctrl);

/**
 * ram_initialize() is is the main RAM init function.
 *
 * It sets basic registers that can not be detected, then does the SPD step.
 * Mainboards and other code can skip one of these steps by the expedient
 * of making it an empty function.
 *
 * @param controllers How many memory controllers there are.
 * @param ctrl Pointer to the mem control structure. This is a generic pointer,
 *             since the structure is wholly chip-dependent, and a survey of
 *             all the types makes it clear that a common struct is not
 *             possible. We can not use the device tree here as this code is
 *             run before the device tree is available. 
 */
void ram_initialize(int controllers, void *ctrl)
{
	int i;

	/* Set the registers we can set once to reasonable values. */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_INFO,
		       "Setting registers of RAM controller %d\n", i);
		ram_set_registers(ctrl, i);
	}

	/* Now setup those things we can auto-detect via SPD. */
	for (i = 0; i < controllers; i++) {
		printk(BIOS_INFO,
		       "Setting SPD based registers of RAM controller %d\n", i);
		ram_set_spd_registers(ctrl, i);
	}

	/* Now that everything is setup enable the RAM. Some chipsets do
	 * the work for us while on others we need to it by hand.
	 */
	printk(BIOS_DEBUG, "Enabling RAM\n");
	ram_enable(controllers, ctrl);

	/* RAM initialization is done. */
	printk(BIOS_DEBUG, "RAM enabled successfully\n");
}
