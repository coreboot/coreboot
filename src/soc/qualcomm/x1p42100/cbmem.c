/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/addressmap.h>
#include <soc/symbols_common.h>

uintptr_t cbmem_top_chipset(void)
{
	return (uintptr_t)CBMEM_TOP;
}
