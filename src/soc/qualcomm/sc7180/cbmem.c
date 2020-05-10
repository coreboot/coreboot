/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

void *cbmem_top_chipset(void)
{
	return (void *)((uintptr_t)4 * GiB);
}
