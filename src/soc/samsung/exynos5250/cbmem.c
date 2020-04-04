/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <stddef.h>
#include <cbmem.h>
#include <soc/cpu.h>

void *cbmem_top_chipset(void)
{
	return (void *)(get_fb_base_kb() * KiB);
}
