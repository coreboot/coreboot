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

/**
 * Write a value into memory.
 *
 * @param addr The memory address to write to.
 * @param value The value to write into the specified memory address.
 */
static void write_phys(unsigned long addr, unsigned long value)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	*ptr = value;
}

/**
 * Read a value from memory.
 *
 * @param addr The memory address to read from.
 * @return The value read from the specified memory address.
 */
static unsigned long read_phys(unsigned long addr)
{
	volatile unsigned long *ptr;
	ptr = (void *)addr;
	return *ptr;
}

/**
 * Fill the specified RAM area.
 *
 * The data which is written into RAM is the address of each memory location.
 * E.g., we write a value of 0x1234 into address 0x1234, we write 0x1235 into
 * memory address 0x1235, and so on.
 *
 * @param start The beginning of the RAM area.
 * @param stop The end of the RAM area.
 */
static void ram_fill(unsigned long start, unsigned long stop)
{
	unsigned long addr;

	printk(BIOS_DEBUG, "DRAM fill: %lx-%lx\n", start, stop);
	for (addr = start; addr < stop; addr += 4) {
		/* Display address being filled. */
		if (!(addr & 0xffff))
			printk(BIOS_DEBUG, "%lx\r", addr);
		write_phys(addr, addr);
	};
	/* Display final address. */
	printk(BIOS_DEBUG, "%lx\nDRAM filled.\n", addr);
}

/**
 * Verify the specified RAM area.
 *
 * This checks whether the specified RAM locations return the "correct" data
 * as written by ram_fill(). The value at address 0x1234 for example should
 * be 0x1234, the value of address 0x1235 should be 0x1235, and so on.
 *
 * @param start The beginning of the RAM area.
 * @param stop The end of the RAM area.
 */
static void ram_verify(unsigned long start, unsigned long stop)
{
	unsigned long addr, value;
	int i = 0;

	printk(BIOS_DEBUG, "DRAM verify: %lx-%lx\n", start, stop);
	for (addr = start; addr < stop; addr += 4) {
		/* Display address being tested. */
		if (!(addr & 0xffff))
			printk(BIOS_DEBUG, "%lx\r", addr);
		value = read_phys(addr);
		if (value != addr) {
			/* Display address with error. */
			printk(BIOS_ERR, "Fail @%lx Read value=%lx\n",
			       addr, value);
			i++;
			/* Abort after 256 verify errors. */
			if (i > 256) {
				printk(BIOS_ERR, "Aborting.\n");
				break;
			}
		}
	}

	/* Display final address. */
	printk(BIOS_DEBUG, "%lx\r", addr);

	if (i) {
		printk(BIOS_DEBUG, "\nDRAM did _NOT_ verify!\n");
	} else {
		printk(BIOS_DEBUG, "\nDRAM range verified.\n");
	}
}

/**
 * Check whether the specified RAM area verifies correctly, and thus whether
 * we can be reasonably confident that our DRAM setup is correct.
 *
 * This is much more of a "Is my DRAM properly configured?" test than
 * a "Is my DRAM faulty?" test, though.
 *
 * @param start The beginning of the RAM area.
 * @param stop The end of the RAM area.
 */
void ram_check(unsigned long start, unsigned long stop)
{
	printk(BIOS_DEBUG, "Testing DRAM: %lx-%lx\n", start, stop);
	ram_fill(start, stop);
	ram_verify(start, stop);
	printk(BIOS_DEBUG, "Done.\n");
}
