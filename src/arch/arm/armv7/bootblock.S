/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * This file is based off of the OMAP3530/ARM Cortex start.S file from Das
 * U-Boot, which itself got the file from armboot.
 */

/* Early initialization code for ARMv7 architecture. */

#include <arch/asm.h>

.arm
	/*
	 * Just in case the maskrom or the vendor basic firmware passes on a
	 * parameter when calling the bootblock, store it here for handling by C
	 * code.
	 */
	.section .bss, "aw" @nobits
	.global maskrom_param
maskrom_param:
	.word 0

ENTRY(_start)
	/*
	 * Set the CPU to System mode with IRQ and FIQ disabled. Prefetch/Data
	 * aborts may happen early and crash before the abort handlers are
	 * installed, but at least the problem will show up near the code that
	 * causes it.
	 */
	msr	cpsr_cxf, #0xdf
	bl	_thumb_start
ENDPROC(_start)

.thumb
ENTRY(_thumb_start)

	/* Preserve the maskrom passed value, if any */
	mov	r10, r0

	bl	arm_init_caches

	/*
	 * From Cortex-A Series Programmer's Guide:
	 * Only CPU 0 performs initialization. Other CPUs go into WFI
	 * to do this, first work out which CPU this is
	 * this code typically is run before any other initialization step
	 */
	mrc p15, 0, r1, c0, c0, 5	@ Read Multiprocessor Affinity Register
	and r1, r1, #0x3 @ Extract CPU ID bits
	cmp r1, #0
	bne wait_for_interrupt		@ If this is not core0, wait

	/*
	 * Initialize the stack to a known value. This is used to check for
	 * stack overflow later in the boot process.
	 */
	ldr	r0, =_stack
	ldr	r1, =_estack
	ldr	r2, =0xdeadbeef
init_stack_loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	bne	init_stack_loop

call_bootblock:

	/* Restore parameter passed in by maskrom/vendor firmware. */
	ldr	r0, =maskrom_param
	str	r10, [r0]

	/* Set stackpointer in internal RAM to call bootblock main() */
	ldr	sp, =_estack
	ldr	r0,=0x00000000
	/*
	 * The current design of cpu_info places the struct at the top of the
	 * stack. Free enough space to accommodate for that, but make sure it's
	 * 8-byte aligned for ABI compliance.
	 */
	sub	sp, sp, #16
	bl	main

wait_for_interrupt:
	wfi
	mov	pc, lr			@ back to my caller
ENDPROC(_thumb_start)
