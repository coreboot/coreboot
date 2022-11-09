/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>

uintptr_t cbmem_top_chipset(void)
{
	return (uintptr_t)4 * GiB;
}
