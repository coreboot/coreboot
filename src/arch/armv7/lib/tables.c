/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2003 Eric Biederman
 * Copyright (C) 2005 Steve Magnani
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <arch/coreboot_tables.h>
#include <string.h>
#include <cbmem.h>
#include <lib.h>

uint64_t high_tables_base = 0;
uint64_t high_tables_size;

void cbmem_arch_init(void)
{
}

struct lb_memory *write_tables(void)
{
	unsigned long low_table_start, low_table_end;
	unsigned long rom_table_start, rom_table_end;

	/* Even if high tables are configured, some tables are copied both to
	 * the low and the high area, so payloads and OSes don't need to know
	 * about the high tables.
	 */
	unsigned long high_table_pointer;

	if (!high_tables_base) {
		printk(BIOS_ERR, "ERROR: High Tables Base is not set.\n");
		// Are there any boards without?
		// Stepan thinks we should die() here!
	}

	printk(BIOS_DEBUG, "High Tables Base is %llx.\n", high_tables_base);

	rom_table_start = 0xf0000;
	rom_table_end =   0xf0000;

	/* Start low addr at 0x500, so we don't run into conflicts with the BDA
	 * in case our data structures grow beyound 0x400. Only multiboot, GDT
	 * and the coreboot table use low_tables.
	 */
	low_table_start = 0;
	low_table_end = 0x500;

#define MAX_COREBOOT_TABLE_SIZE (8 * 1024)
	post_code(0x9d);

	high_table_pointer = (unsigned long)cbmem_add(CBMEM_ID_CBTABLE, MAX_COREBOOT_TABLE_SIZE);

	if (high_table_pointer) {
		unsigned long new_high_table_pointer;

		/* Also put a forwarder entry into 0-4K */
		new_high_table_pointer = write_coreboot_table(low_table_start, low_table_end,
				high_tables_base, high_table_pointer);

		if (new_high_table_pointer > (high_table_pointer +
					MAX_COREBOOT_TABLE_SIZE))
			printk(BIOS_ERR, "%s: coreboot table didn't fit (%lx)\n",
				   __func__, new_high_table_pointer -
				   high_table_pointer);

                printk(BIOS_DEBUG, "coreboot table: %ld bytes.\n",
				new_high_table_pointer - high_table_pointer);
	} else {
		/* The coreboot table must be in 0-4K or 960K-1M */
		rom_table_end = write_coreboot_table(
				     low_table_start, low_table_end,
				     rom_table_start, rom_table_end);
	}

	post_code(0x9e);

	// Remove before sending upstream
	cbmem_list();

	return get_lb_mem();
}
