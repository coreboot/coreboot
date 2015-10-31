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
 */

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/x86/gdt.h>

/* i386 lgdt argument */
struct gdtarg {
	u16 limit;
#ifdef __x86_64__
	u64 base;
#else
	u32 base;
#endif
} __attribute__((packed));

/* Copy GDT to new location and reload it.
 * FIXME: We only do this for BSP CPU.
 */
static void move_gdt(int is_recovery)
{
	void *newgdt;
	u16 num_gdt_bytes = (uintptr_t)&gdt_end - (uintptr_t)&gdt;
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

	gdtarg.base = (uintptr_t)newgdt;
	gdtarg.limit = num_gdt_bytes - 1;

	__asm__ __volatile__ ("lgdt %0\n\t" : : "m" (gdtarg));
	printk(BIOS_DEBUG, "ok\n");
}
RAMSTAGE_CBMEM_INIT_HOOK(move_gdt)
