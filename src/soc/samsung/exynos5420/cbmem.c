/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/cpu.h>

void *cbmem_top_chipset(void)
{
	return (void *)(get_fb_base_kb() * KiB);
}
