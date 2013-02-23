/*
 * This file is part of the coreboot project.
 *
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

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <lib.h>
#include <console/console.h>

// Global Descriptor Table, defined in c_start.S
extern char gdt;
extern char gdt_end;

/* i386 lgdt argument */
struct gdtarg {
	u16 limit;
	u32 base;
} __attribute__((packed));

// Copy GDT to new location and reload it
void move_gdt(void)
{
	void *newgdt;
	u16 num_gdt_bytes = &gdt_end - &gdt;
	struct gdtarg gdtarg;

	newgdt = cbmem_find(CBMEM_ID_GDT);
	if (!newgdt) {
		newgdt = cbmem_add(CBMEM_ID_GDT, ALIGN(num_gdt_bytes, 512));
		if (!newgdt) {
			printk(BIOS_ERR, "Error: Could not relocate GDT.\n");
			return;
		}
		printk(BIOS_DEBUG, "Moving GDT to %p...", newgdt);
		memcpy((void*)newgdt, &gdt, num_gdt_bytes);
	}

	gdtarg.base = (u32)newgdt;
	gdtarg.limit = num_gdt_bytes - 1;

	__asm__ __volatile__ ("lgdt %0\n\t" : : "m" (gdtarg));
	printk(BIOS_DEBUG, "ok\n");
}

