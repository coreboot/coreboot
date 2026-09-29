/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <commonlib/helpers.h>

uintptr_t cbmem_top_chipset(void)
{
	return 4ULL * GiB;
}
