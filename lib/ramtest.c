/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2001 Tyson Sawyer, irobot
 * Copyright (C) 2008 Ronald G. Minnich
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include <types.h>
#include <lib.h>
#include <console.h>


static void write_phys(unsigned long addr, unsigned long value)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	*ptr = value;
}

static unsigned long read_phys(unsigned long addr)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	return *ptr;
}

static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	/* 
	 * Fill.
	 */
	printk(BIOS_DEBUG, "DRAM fill: %lx-%lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		/* Display address being filled */
		if (!(addr & 0xffff)) {
			printk(BIOS_DEBUG, "%lx\r", addr);
		}
		write_phys(addr, addr);
	};
	/* Display final address */
	printk(BIOS_DEBUG, "%lx\nDRAM filled\n", addr);
}

static void ram_verify(unsigned long start, unsigned long stop)
{
	unsigned long addr;
	int i = 0;
	/* 
	 * Verify.
	 */
	printk(BIOS_DEBUG, "DRAM verify: %lx-%lx\n", start, stop);
	for(addr = start; addr < stop ; addr += 4) {
		unsigned long value;
		/* Display address being tested */
		if (!(addr & 0xffff)) {
			printk(BIOS_DEBUG, "%lx\r", addr);
		}
		value = read_phys(addr);
		if (value != addr) {
			/* Display address with error */
			printk(BIOS_ERR, "Fail @%lx Read value=%lx\n", 
				addr, value);
			i++;
			if(i>256) {
				printk(BIOS_ERR, "Aborting.\n");
				break;
			}
		}
	}
	/* Display final address */
	printk(BIOS_DEBUG, "%lx\r", addr);
	if (i) {
		printk(BIOS_DEBUG, "\nDRAM did _NOT_ verify!\n");
	}
	else {
		printk(BIOS_DEBUG, "\nDRAM range verified.\n");
	}
}


void ram_check(unsigned long start, unsigned long stop)
{
	/*
	 * This is much more of a "Is my DRAM properly configured?"
	 * test than a "Is my DRAM faulty?" test.  Not all bits
	 * are tested.   -Tyson
	 */
	printk(BIOS_DEBUG, "Testing DRAM: %lx-%lx\n", start, stop);
	ram_fill(start, stop);
	ram_verify(start, stop);
	printk(BIOS_DEBUG, "Done.\n");
}

