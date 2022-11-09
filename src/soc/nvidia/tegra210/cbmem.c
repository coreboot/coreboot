/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/addressmap.h>

uintptr_t cbmem_top_chipset(void)
{
	static uintptr_t addr;

	if (addr == 0) {
		uintptr_t begin_mib;
		uintptr_t end_mib;

		memory_in_range_below_4gb(&begin_mib, &end_mib);
		/* Make sure we consume everything up to 4GIB. */
		if (end_mib == 4096)
			addr = ~(uint32_t)0;
		else
			addr = end_mib << 20;
	}

	return addr;
}
