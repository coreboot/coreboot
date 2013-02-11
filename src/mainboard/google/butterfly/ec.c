/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <ec/quanta/ene_kb3940q/ec.h>
#include "ec.h"

void butterfly_ec_init(void)
{
	printk(BIOS_DEBUG, "Butterfly EC Init\n");

	/* Report EC info */
	/* EC version: 6 bytes */
	printk(BIOS_DEBUG,"  EC version: %c%c%c%c%c%c\n",
	       ec_mem_read(EC_FW_VER0), ec_mem_read(EC_FW_VER1),
	       ec_mem_read(EC_FW_VER2), ec_mem_read(EC_FW_VER3),
	       ec_mem_read(EC_FW_VER4), ec_mem_read(EC_FW_VER5));

	/* Disable wake on USB, LAN & RTC */
	/* Enable Wake from Keyboard */
	ec_mem_write(EC_EC_PSW, EC_PSW_IKB);

}
