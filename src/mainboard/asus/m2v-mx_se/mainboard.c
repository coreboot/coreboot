/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
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
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <boot/tables.h>
#include "chip.h"

/* in arch/i386/boot/tables.c */
extern uint64_t high_tables_base, high_tables_size;

int add_mainboard_resources(struct lb_memory *mem)
{
#if HAVE_HIGH_TABLES == 1
	printk_debug("Adding high table area\n");
	lb_add_memory_range(mem, LB_MEM_TABLE,
		high_tables_base, high_tables_size);
#endif
#if HAVE_ACPI_RESUME == 1
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		_RAMBASE, ((CONFIG_LB_MEM_TOPK<<10) - _RAMBASE));
	lb_add_memory_range(mem, LB_MEM_RESERVED,
		DCACHE_RAM_BASE, DCACHE_RAM_SIZE);
#endif
	return 0;
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("ASUS M2V-MX SE Mainboard")
};
