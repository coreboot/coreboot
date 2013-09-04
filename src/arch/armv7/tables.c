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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <cpu/cpu.h>
#include <boot/tables.h>
#include <boot/coreboot_tables.h>
#include <string.h>
#include <cbmem.h>
#include <lib.h>

#define MAX_COREBOOT_TABLE_SIZE (8 * 1024)

void __attribute__((weak)) get_cbmem_table(uint64_t *base, uint64_t *size)
{
	printk(BIOS_WARNING, "WARNING: you need to define get_cbmem_table for your board\n");
	*base = 0;
	*size = 0;
}

void cbmem_arch_init(void)
{
}

struct lb_memory *write_tables(void)
{
	unsigned long table_pointer, new_table_pointer;

	cbmem_base_check();

	post_code(0x9d);

	table_pointer = (unsigned long)cbmem_add(CBMEM_ID_CBTABLE,
						MAX_COREBOOT_TABLE_SIZE);
	if (!table_pointer) {
		printk(BIOS_ERR, "Could not add CBMEM for coreboot table.\n");
		return NULL;
	}

	new_table_pointer = write_coreboot_table(0UL, 0UL,
				table_pointer, table_pointer);

	if (new_table_pointer > (table_pointer + MAX_COREBOOT_TABLE_SIZE)) {
		printk(BIOS_ERR, "coreboot table didn't fit (%lx/%x bytes)\n",
			   new_table_pointer - table_pointer, MAX_COREBOOT_TABLE_SIZE);
	}

	printk(BIOS_DEBUG, "coreboot table: %ld bytes.\n",
			new_table_pointer - table_pointer);

	post_code(0x9e);

	/* Print CBMEM sections */
	cbmem_list();

	return get_lb_mem();
}
