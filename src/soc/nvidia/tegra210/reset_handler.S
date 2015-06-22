/*
 * Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>

#define CPUACTLR_EL1 s3_1_c15_c2_0

CPU_RESET_ENTRY(tegra210_reset_handler)
	/*
	 * Invalidate BTB along with I$ to remove any stale entries
	 * from the branch predictor array.
	 */
	mrs	x0, CPUACTLR_EL1
	orr	x0, x0, #1
	msr	CPUACTLR_EL1, x0	/* invalidate BTB and I$ together */
	dsb	sy
	isb
	ic	iallu			/* invalidate */
	dsb	sy
	isb

	bic	x0, x0, #1
	msr	CPUACTLR_EL1, x0	/* restore original CPUACTLR_EL1 */
	dsb	sy
	isb

	.rept 7
	nop                            /* wait */
	.endr

	/*
	 * Extract OSLK bit and check if it is '1'. This bit remains '0'
	 * for A53. If '1', turn off regional clock gating and request
	 * warm reset.
	 */
	mrs	x0, oslsr_el1
	and	x0, x0, #2		/* extract oslk bit */
	mrs	x1, mpidr_el1
	bics	xzr, x0, x1, lsr #7	/* 0 if slow cluster */
	b.eq	__restore_oslock
	mov	x0, xzr
	msr	oslar_el1, x0		/* os lock stays 0 across warm reset */
	mov	x3, #3
	movz	x4, #0x8000, lsl #48
	msr	CPUACTLR_EL1, x4	/* turn off RCG */
	isb
	msr	rmr_el3, x3		/* request warm reset */
	isb
	dsb	sy
	wfi

	/*
	 * These nops are here so that speculative execution won't harm us
	 * before we are done warm reset.
	 */
	.rept 65
	nop
	.endr

__restore_oslock:
	mov	x0, #1
	msr	oslar_el1, x0

	b	arm64_cpu_startup_resume
ENDPROC(tegra210_reset_handler)
