/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <lib.h>
#include <console/console.h>

int primitive_memtest(uintptr_t base, uintptr_t size)
{
	uintptr_t *p;
	uintptr_t i;
	int bad = 0;

	printk(BIOS_SPEW, "Performing primitive memory test.\n");
	printk(BIOS_SPEW, "DRAM start: 0x%08x, DRAM size: 0x%08x", base, size);
	for (i = base; i < base + (size - 1) - sizeof(p); i += sizeof(p)) {
		if (i % 0x100000 == 0) {
			if ((i % 0x800000) == 0)
				printk(BIOS_SPEW, "\n");
			else if (i != 0)
				printk(BIOS_SPEW, " ");
			printk(BIOS_SPEW, "0x%08x", i);
		}
		p = (uintptr_t *)i;
		*p = i;
	}

	printk(BIOS_SPEW, "\n\nReading back DRAM content");
	for (i = base; i < base + (size - 1) - sizeof(p); i += sizeof(p)) {
		if (i % 0x100000 == 0) {
			if ((i % 0x800000) == 0)
				printk(BIOS_SPEW, "\n");
			else if (i != 0)
				printk(BIOS_SPEW, " ");
			printk(BIOS_SPEW, "0x%08x", i);
		}

		p = (uintptr_t *)i;
		if (*p != i) {
			printk(BIOS_SPEW, "\n0x%08zx: got 0x%zx\n", i, *p);
			bad++;
		}
	}

	printk(BIOS_SPEW, "\n");
	printk(BIOS_SPEW, "%d errors\n", bad);

	return bad;
}
