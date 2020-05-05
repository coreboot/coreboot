/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * TZ expects the ARM core to be in 'ARM' mode. However, coreboot seems
 * to be compiled in mixed thumb/arm mode. Hence create a glue function
 * to invoke TZ.
 */

#include <arch/asm.h>

/*
 * Force ARM mode. Else this gets assembled with mixed ARM and
 * Thumb instructions. We set up everything and jump to TZBSP
 * using the 'blx' instruction. For 'blx' if the last bit of the
 * destination address is zero, it switches to ARM mode. Since,
 * we are already in ARM mode, nothing to switch as such.
 *
 * However, when TZBSP returns, the CPU is still in ARM mode.
 * If the assembler inserts Thumb instructions between the point
 * of return from TZBSP to the 'bx' instruction we are hosed.
 * Hence forcing ARM mode.
 *
 * Rest of the code can be compiled in mixed ARM/Thumb mode.
 * Since tz_init_wrapper is being forced as an ARM symbol,
 * callers will use 'blx' to come here forcing a switch to
 * ARM mode. The wrapper does its job and returns back to the
 * Thumb caller.
 */
.arm
/*
 * int tz_init_wrapper(int, int, void *);
 */
ENTRY(tz_init_wrapper)
	.global tz_init_wrapper

	/*
	 *      r0 = tz_arg1
	 *      r1 = tz_arg2
	 *      r2 = tz_load_addr
	 */

	/*
	 * Per the AAPCS
	 *	r0, r1, r2, r3, r12 can be clobbered
	 *	r4, r5, r6, r7, r8, r9, r10, r11 have to be preserved
	 *
	 * Following code clobbers
	 *	r0 - Setting return value to zero
	 *	r1 - For doing a thumb return
	 *	r3 - Passing 'SP' from current mode to 'svc' mode
	 *	r4 - To save & restore CPSR
	 *
	 * Per AAPCS, save and restore r4, rest are 'clobberable' :)
	 * The invoked code takes care of saving and restoring the other
	 * preserved registers (i.e. r5 - r11)
	 *
	 * Stack Usage:
	 *	SP ->	| LR	|	(Lower address)
	 *		| r4	|
	 *		| CPSR	|
	 *		|-------|
	 *		|   .   |
	 *		|   .   |
	 *		|   .   |	(Higher address)
	 */

	sub	sp, sp, #12	/* Allocate stack frame */
	str	lr, [sp]	/* Save return address */
	str	r4, [sp, #4]	/* Use r4 to hold the new CPSR value */

	mov	r3, sp		/* Get current stack pointer */

	mrs	r4, CPSR	/* save CPSR */
	str	r4, [sp, #8]

	bic	r4, r4, 0x1f	/* Clear mode bits */
	orr	r4, r4, 0x13	/* 'svc' mode */
	msr	cpsr_cxf, r4	/* Switch to Supervisor mode. */
	mov	sp, r3		/* Use the same stack as the previous mode */

	blx	r2		/* Jump to TZ in ARM mode */

	nop			/* back from TZ, in ARM mode */

	ldr	r4, [sp, #8]	/* restore CPSR */
	msr	cpsr_cxf, r4

	ldr	r4, [sp, #4]	/* restore r4 */

	ldr	lr, [sp]	/* saved return address */
	add	sp, sp, #12	/* free stack frame */

	bx	lr		/* back to thumb caller */

ENDPROC(tz_init_wrapper)
