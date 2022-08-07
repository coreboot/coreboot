/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef CPU_X86_BIST_H
#define CPU_X86_BIST_H

#include <console/console.h>
#include <stdint.h>

static inline void report_bist_failure(u32 bist)
{
	if (bist != 0) {
		printk(BIOS_EMERG, "BIST failed: %08x", bist);
		die("\n");
	}
}

#endif /* CPU_X86_BIST_H */
