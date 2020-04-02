/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _AMD_SB_RESET_H_
#define _AMD_SB_RESET_H_

#include <arch/cache.h>
#include <console/console.h>
#include <halt.h>

/* Implement the bare reset, e.g. write to cf9. */
void do_soft_reset(void);

/* Prepare for reset, run do_soft_reset(), halt. */
static inline __noreturn void soft_reset(void)
{
	printk(BIOS_INFO, "%s() called!\n", __func__);
	dcache_clean_all();
	do_soft_reset();
	halt();
}

#endif	/* _AMD_SB_RESET_H_ */
