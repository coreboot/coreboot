/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <cbmem.h>
#include <symbols.h>
#include <commonlib/device_tree.h>
#include <console/console.h>

DECLARE_REGION(fdt_pointer)
uintptr_t cbmem_top_chipset(void)
{
	const uint64_t top = fdt_get_memory_top((void *) *((uintptr_t *)_fdt_pointer));

	if (top == 0) {
		/* corrupted FDT? */
		die("Could not find top of memory in FDT!");
	}

	printk(BIOS_DEBUG, "%s: 0x%llx\n", __func__, top);
	return (uintptr_t)top;
}
