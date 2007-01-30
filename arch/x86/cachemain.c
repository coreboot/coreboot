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
#include <lar.h>
#include "config.h"

static void post_code(u8 value)
{
	outb(value, 0x80);
}

static void stop_ap(void)
{
	// nothing yet
	post_code(0xf0);
}

static void enable_superio(void)
{
	// nothing yet
	post_code(0xf1);
}

static void enable_rom(void)
{
	// nothing here yet
	post_code(0xf2);
}

void stage1_main(u32 bist)
{
	int ret;
	struct mem_file archive, result;

	post_code(0x02);

	// before we do anything, we want to stop if we dont run 
	// on the bootstrap processor.
	if (bist==0) {
		// stop secondaries
		stop_ap();
	}

	// We have cache as ram running and can start executing code in C.
	//
	
	enable_superio();

	//
	uart_init();	// initialize serial port
	console_init(); // print banner

	if (bist!=0) {
		die("BIST FAILED: %08x", bist);
	}

	// enable rom
	enable_rom();
	
	// location and size of image.
	
	// FIXME this should be defined in the VPD area
	// but NOT IN THE CODE.
	
	archive.len=LINUXBIOS_ROMSIZE_KB*1024;
	archive.start=(void *)(0UL-archive.len);

	// FIXME check integrity


	// fixme: choose an initram image (normal/fallback)
	// find first initram
	ret=find_file(&archive, "normal/initram", &result);
	if(!ret) {
		void (*initram)(void);
		initram=(result.start);
		printk(BIOS_INFO, "Jumping to RAM init code at 0x%08x\n",
				result.start);
		initram();

	}

	die ("FATAL: No initram module found\n");

}


