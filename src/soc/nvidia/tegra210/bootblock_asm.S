/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Early initialization code for ARM architecture.
 *
 * This file is based off of the OMAP3530/ARM Cortex start.S file from Das
 * U-Boot, which itself got the file from armboot.
 */

#include <arch/asm.h>

#include "stack.S"

ENTRY(_start)
	/*
	 * Set the CPU to System mode with IRQ and FIQ disabled. Prefetch/Data
	 * aborts may happen early and crash before the abort handlers are
	 * installed, but at least the problem will show up near the code that
	 * causes it.
	 */
	msr	cpsr_cxf, #0xdf

	stack_init stack_top=_estack stack_bottom=_stack seed=1 func=tegra210_main
ENDPROC(_start)
