/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2018 Siemens AG
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

#include <bootstate.h>
#include <console/console.h>
#include <hwilib.h>
#include <baseboard/variants.h>
#include <variant/ptn3460.h>

void variant_mainboard_final(void)
{
	int status;

	/*
	 * Set up the DP2LVDS converter.
	 * ptn3460_init() may only be executed after i2c bus init.
	 */
	status = ptn3460_init("hwinfo.hex");
	if (status)
		printk(BIOS_ERR, "LCD: Set up PTN with status 0x%x\n", status);
	else
		printk(BIOS_INFO, "LCD: Set up PTN was successful.\n");
}
