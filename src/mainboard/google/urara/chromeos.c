/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Technologies
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

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <console/console.h>
#include <gpio.h>

int get_write_protect_state(void)
{
	printk(BIOS_ERR, "%s unsupported, but called\n", __func__);
	return 0;
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	printk(BIOS_ERR, "%s unsupported, but called\n", __func__);
}
