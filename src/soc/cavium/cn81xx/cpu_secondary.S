/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2016          Cavium, Inc. <support@cavium.com>
 * Copyright 2018-present  Facebook, Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>
#include <soc/addressmap.h>

// based on arm64_init_cpu
ENTRY(secondary_init)
	/* Initialize PSTATE (unmask all exceptions, select SP_EL0). */
	msr	SPSel, #0
	msr	DAIFClr, #0xf

	/* TODO: This is where we'd put non-boot CPUs into WFI if needed. */

	/* x22: SCTLR, return address: x23 (callee-saved by subroutine) */
	mov	x23, x30
	/* TODO: Assert that we always start running at EL3 */
	mrs	x22, sctlr_el3

	/* Activate ICache (12) already for speed during cache flush below. */
	orr	x22, x22, #(1 << 12)
	msr	sctlr_el3, x22
	isb

	/* Invalidate dcache */
	bl	dcache_invalidate_all

	/* Deactivate MMU (0), Alignment Check (1) and DCache (2) */
	and	x22, x22, # ~(1 << 0) & ~(1 << 1) & ~(1 << 2)
	/* Activate Stack Alignment (3) because why not */
	orr	x22, x22, #(1 << 3)
	/* Set to little-endian (25) */
	and	x22, x22, # ~(1 << 25)
	/* Deactivate write-xor-execute enforcement (19) */
	and	x22, x22, # ~(1 << 19)
	msr	sctlr_el3, x22

	/* Invalidate icache and TLB for good measure */
	ic	iallu
	tlbi	alle3
	dsb	sy
	isb

	/* Load core ID to x0 */
	mrs	x0, MPIDR_EL1
	and	x1, x0, # 0xf
	lsr	x0, x0, 4
	and	x0, x0, # 0xff0
	orr	x0, x0, x1

	/* Each core gets CONFIG_STACK_SIZE bytes of stack */
	mov	x2, # CONFIG_STACK_SIZE
	mul	x1, x0, x2
	/* Backup core id */
	mov	x22, x0
	ldr	x0, =_stack_sec
	add	x0, x1, x0 // x0 = CONFIG_STACK_SIZE * coreid + _stack_sec
	add	x1, x0, # CONFIG_STACK_SIZE // x1 = x0 + CONFIG_STACK_SIZE

	/* Initialize stack with sentinel value to later check overflow. */
	ldr	x2, =0xdeadbeefdeadbeef

1:
	stp	x2, x2, [x0], #16
	cmp	x0, x1
	bne	1b

	/* Leave a line of beef dead for easier visibility in stack dumps. */
	sub	sp, x0, #16

	/* Set arg0 to core id */
	mov	x0, x22

	/* Call C entry */
	bl	secondary_cpu_init

ENDPROC(secondary_init)
