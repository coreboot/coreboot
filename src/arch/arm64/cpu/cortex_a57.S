/*
 * This file is part of the coreboot project.
 *
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
#include <arch/cache_helpers.h>
#include <cpu/cortex_a57.h>

ENTRY(arm64_cpu_early_setup)
	mrs x0, CPUECTLR_EL1
	orr x0, x0, #(1 << SMPEN_SHIFT)
	msr CPUECTLR_EL1, x0
	isb
	ret
ENDPROC(arm64_cpu_early_setup)

/*
 * CPU power down sequence as per A57/A53/A72 TRM
 *
 * x0 - L2 flush by HW(0) or SW(1), if system/HW driven L2 flush is supported
 *
 */
#if IS_ENABLED(CONFIG_ARCH_ARM64_CORTEX_A57_POWER_DOWN_SUPPORT)
ENTRY(cortex_a57_cpu_power_down)
	/* Store L2 cache flush request */
	mov	x13, x0

	/* 1. Stop allocations to our data cache */
	mrs	x0, sctlr_el1
	bic	x0, x0, #1 << 2		// clear SCTLR.C
	msr	sctlr_el1, x0
	isb

	mrs	x0, sctlr_el3
	bic	x0, x0, #1 << 2		// clear SCTLR.C
	msr	sctlr_el3, x0
	isb

	mrs	x0, midr_el1
	ubfx	x0, x0, #4, #12
	cmp	x0, #CORTEX_A53_PN
	b.eq	a53

	/* 2. Disable L2 prefetch */
	mrs	x0, CPUECTLR_EL1	// CPUECTLR_EL1
	/* CPUECTLR[38], disable table walk descriptor access L2 prefetch */
	orr	x0, x0, #1 << 38
	/*
	 * CPUECTLR[36:35] L2 instruction fetch prefetch distance
	 * 0 => disable instruction prefetch
	 */
	bic	x0, x0, #3 << 35
	/*
	 * CPUECTLR[33:32] L2 load/store prefetch distance
	 * 0 => disable instruction prefetch
	 */
	bic	x0, x0, #3 << 32
	msr	CPUECTLR_EL1, x0

	/* 3. ISB to ensure ectlr write is complete */
	isb

	/* 4. DSB to ensure prior prefetches are complete */
	dsb	sy

a53:
	/* 5. Clean and invalidate L1 and L2 if X13 == 1 */
	mov	x0, #DCCISW
	cmp	x13, #1
	bne	1f
	bl	flush_dcache_all
	b	2f
1:
	bl	flush_dcache_louis
2:

	/* 6. Leave coherency, clear SMPEN */
	mrs	x0, CPUECTLR_EL1
	bic	x0, x0, #(1 << SMPEN_SHIFT)
	msr	CPUECTLR_EL1, x0

	/* 7. Set the DBGOSDLR.DLK, Double lock control bit */
	mrs	x0, osdlr_el1
	orr	x0, x0, #OSDLR_DBL_LOCK_BIT
	msr	osdlr_el1, x0

	/*
	 * 9. Execute an ISB instruction to ensure that all of the
	 * System register changes from the previous steps have
	 * been committed.
	 */
	isb

	/*
	 * 10. Execute a DSB instruction to ensure that all
	 * instruction cache, TLB, and branch predictor
	 * maintenance operations issued by any processor in the
	 * multiprocessor before the SMPEN bit was cleared have
	 * completed.
	 */
	dsb	sy

	/* 11. wfi */
3:	wfi

	/* we never return here */
	b	3b
ENDPROC(cortex_a57_cpu_power_down)
#endif
