/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbmem.h>
#include <soc/cpu.h>

uintptr_t cbmem_top_chipset(void)
{
	return get_fb_base_kb() * KiB;
}
