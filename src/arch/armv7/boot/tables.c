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

/*
 * TODO: "High" tables are a convention used on x86. Maybe we can
 * clean up that naming at some point.
 */
uint64_t high_tables_base = 0;
uint64_t high_tables_size;

void cbmem_arch_init(void)
{
}

struct lb_memory *write_tables(void)
{
	unsigned long table_pointer;

	if (!high_tables_base) {
		printk(BIOS_ERR, "ERROR: coreboot_tables_base is not set.\n");
		// Are there any boards without?
		// Stepan thinks we should die() here!
	}

	printk(BIOS_DEBUG, "high_tables_base: %llx.\n", high_tables_base);

#define MAX_COREBOOT_TABLE_SIZE (8 * 1024)
	post_code(0x9d);

	table_pointer = (unsigned long)cbmem_add(CBMEM_ID_CBTABLE,
						MAX_COREBOOT_TABLE_SIZE);
	if (table_pointer) {
		unsigned long new_table_pointer;
		new_table_pointer = write_coreboot_table(table_pointer,
							high_tables_size);
		if (table_pointer > (table_pointer + MAX_COREBOOT_TABLE_SIZE)) {
			printk(BIOS_ERR, "%s: coreboot table didn't fit (%lx)\n",
				   __func__, new_table_pointer - table_pointer);
		}
		printk(BIOS_DEBUG, "coreboot table: %ld bytes.\n",
			new_table_pointer - table_pointer);
	}
#if 0
	else {
		/* The coreboot table must be in 0-4K or 960K-1M */
		rom_table_end = write_coreboot_table(
				     low_table_start, low_table_end,
				     rom_table_start, rom_table_end);
	}
#endif

	post_code(0x9e);

	// Remove before sending upstream
	cbmem_list();

	return get_lb_mem();
}
