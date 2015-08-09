/*
 * This file is part of the coreboot project.
 *
 * Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <arch/asm.h>
#include <cpu/cortex_a57.h>

/*
 * It is observed that BTB contains stale data after power on reset. This could
 * lead to unexpected branching and crashes at random intervals during the boot
 * flow. Thus, invalidate the BTB immediately after power on reset.
 */
.macro t210_a57_fixup
	/*
	 * Invalidate BTB along with I$ to remove any stale entries
	 * from the branch predictor array.
	 */
	mrs	x0, CPUACTLR_EL1
	orr	x0, x0, #BTB_INVALIDATE
	msr	CPUACTLR_EL1, x0	/* invalidate BTB and I$ together */
	dsb	sy
	isb
	ic	iallu			/* invalidate */
	dsb	sy
	isb

	bic	x0, x0, #BTB_INVALIDATE
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
	b.eq	1000f
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

1000:
	/* Restore os lock */
	mov	x0, #1
	msr	oslar_el1, x0
.endm

ENTRY(stage_entry)
	t210_a57_fixup
	b	arm64_cpu_startup
ENDPROC(stage_entry)

ENTRY(tegra210_reset_handler)
	t210_a57_fixup
	b	arm64_cpu_startup_resume
ENDPROC(tegra210_reset_handler)
