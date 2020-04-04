/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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

	/* Initialize PSTATE, SCTLR and caches to clean state, set up stack. */
	bl	arm64_init_cpu

	/* Jump to Tegra-specific C entry point. */
	bl	ramstage_entry
ENDPROC(stage_entry)
