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

#include <cf9_reset.h>
#include <console/console.h>
#include <fsp/util.h>

void chipset_handle_reset(uint32_t status)
{
	/* Do a hard reset if Quark FSP ever requests a reset */
	printk(BIOS_ERR, "Unknown reset type %x\n", status);
	full_reset();
}
