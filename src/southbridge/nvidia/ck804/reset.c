/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2004 Tyan Computer
 * Written by Yinghai Lu <yhlu@tyan.com> for Tyan Computer.
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

#define __SIMPLE_DEVICE__

#include <arch/io.h>
#include <reset.h>

#include "../../../northbridge/amd/amdk8/reset_test.c"

void hard_reset(void)
{
	set_bios_reset();
	/* Try rebooting through port 0xcf9. */
	outb((0 << 3) | (0 << 2) | (1 << 1), 0xcf9);
	outb((0 << 3) | (1 << 2) | (1 << 1), 0xcf9);
}
