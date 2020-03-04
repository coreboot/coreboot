/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <types.h>
#include <string.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
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
} __packed;

/*
 * Copy GDT to new location and reload it.
 * FIXME: We only do this for BSP CPU.
 */
static void move_gdt(int is_recovery)
{
	void *newgdt;
	u16 num_gdt_bytes;
	struct gdtarg gdtarg;

	/* ramstage is already in high memory. No need to use a new gdt. */
	if (CONFIG(RELOCATABLE_RAMSTAGE))
		return;

	newgdt = cbmem_find(CBMEM_ID_GDT);
	num_gdt_bytes = (uintptr_t)&gdt_end - (uintptr_t)&gdt;
	if (!newgdt) {
		newgdt = cbmem_add(CBMEM_ID_GDT, ALIGN_UP(num_gdt_bytes, 512));
		if (!newgdt) {
			printk(BIOS_ERR, "Error: Could not relocate GDT.\n");
			return;
		}
		memcpy((void *)newgdt, &gdt, num_gdt_bytes);
	}
	printk(BIOS_DEBUG, "Moving GDT to %p...", newgdt);

	gdtarg.base = (uintptr_t)newgdt;
	gdtarg.limit = num_gdt_bytes - 1;

	__asm__ __volatile__ ("lgdt %0\n\t" : : "m" (gdtarg));
	printk(BIOS_DEBUG, "ok\n");
}
RAMSTAGE_CBMEM_INIT_HOOK(move_gdt)
