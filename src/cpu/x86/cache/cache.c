/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <cpu/x86/cache.h>

void x86_enable_cache(void)
{
	post_code(POST_ENABLING_CACHE);
	printk(BIOS_INFO, "Enabling cache\n");
	enable_cache();
}
