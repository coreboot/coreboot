/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

uintptr_t cbmem_top_chipset(void)
{
	/*
	 * Smallest reported to be working (but not officially supported) DIMM is
	 * 4GB. This means that we always have at least as much available. Last
	 * 256MB are reserved for hostboot/coreboot (OCC and HOMER images).
	 *
	 * TODO: implement this properly after RAM is detected.
	 */
	return 4ull * GiB - 256 * MiB;
}
