/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbmem.h>
#include <symbols.h>
#include <ramdetect.h>
#include <commonlib/device_tree.h>
#include <mcall.h>

uintptr_t cbmem_top_chipset(void)
{
	uint64_t top;

	top = fdt_get_memory_top((void *)HLS()->fdt);
	ASSERT_MSG(top, "Failed reading memory range from FDT");

	return MIN(top, (uint64_t)4 * GiB - 1);
}
