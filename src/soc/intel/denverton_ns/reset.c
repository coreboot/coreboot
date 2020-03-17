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
#include <fsp/util.h>

void chipset_handle_reset(uint32_t status)
{
	switch (status) {
	case FSP_STATUS_RESET_REQUIRED_5: /* Global Reset */
		die("Global Reset not implemented!\n");
		break;
	default:
		printk(BIOS_ERR, "unhandled reset type %x\n", status);
		die("unknown reset type");
		break;
	}
}
