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

#include <console/console.h>
#include <program_loading.h>

void qemu_power8_main(void);

/* The qemu part of all this is very, very non-hardware like.
 * So it gets its own bootblock.
 */
void qemu_power8_main(void)
{
	if (CONFIG(BOOTBLOCK_CONSOLE)) {
		console_init();
	}

	run_romstage();
}
