/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Stefan Reinauer <stepan@coresystems.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include <arch/types.h>
#include <arch/io.h>
#include <console/loglevel.h>

int printk(int msg_level, const char *fmt, ...);
void die(const char *msg);
static void post_code(u8 value);


int main(void)
{
	printk(BIOS_INFO, "RAM init code started\n");

	printk(BIOS_INFO, "Nothing to do.");

	return 0;
}

static void post_code(u8 value)
{
	outb(value, 0x80);
}

