/*
 * Early initialization code for ARMv7 architecture.
 *
 * This file is based off of the OMAP3530/ARM Cortex start.S file from Das
 * U-Boot, which itself got the file from armboot.
 *
 * Copyright (c) 2004	Texas Instruments <r-woodruff2@ti.com>
 * Copyright (c) 2001	Marius Gröger <mag@sysgo.de>
 * Copyright (c) 2002	Alex Züpke <azu@sysgo.de>
 * Copyright (c) 2002	Gary Jennejohn <garyj@denx.de>
 * Copyright (c) 2003	Richard Woodruff <r-woodruff2@ti.com>
 * Copyright (c) 2003	Kshitij <kshitij@ti.com>
 * Copyright (c) 2006-2008 Syed Mohammed Khasim <x0khasim@ti.com>
 * Copyright (c) 2013   The Chromium OS Authors
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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

.section ".start", "a", %progbits
.globl _start
_start: b	reset
	.balignl 16,0xdeadbeef

_cbfs_master_header:
	/* The CBFS master header is inserted by cbfstool at the first
	 * aligned offset after the above anchor string is found.
	 * Hence, we leave some space for it.
	 */
	.skip 128			@ Assumes 64-byte alignment

reset:
	/*
	 * Set the cpu to SVC32 mode and unmask aborts. Aborts might happen
	 * before logging is turned on and may crash the machine, but at least
	 * the problem will show up near the code that causes it.
	 */
	mrs	r0, cpsr
	bic	r0, r0, #0x1f
	orr	r0, r0, #0xd3
	bic	r0, r0, #0x100
	msr	cpsr_cxsf,r0

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
	ldr	r0, .Stack
	ldr	r1, .Stack_size
	sub	r0, r0, r1
	ldr	r1, .Stack
	ldr	r2, =0xdeadbeef
init_stack_loop:
	str	r2, [r0]
	add	r0, #4
	cmp	r0, r1
	bne	init_stack_loop

/* Set stackpointer in internal RAM to call board_init_f */
call_bootblock:
	ldr	sp, .Stack /* Set up stack pointer */
	bic	sp, sp, #7 /* 8-byte alignment for ABI compliance */
	ldr	r0,=0x00000000
	/*
	 * Use "bl" instead of "b" even though we do not intend to return.
	 * "bl" gets compiled to "blx" if we're transitioning from ARM to
	 * Thumb. However, "b" will not and GCC may attempt to create a
	 * wrapper which is currently broken.
	 */
	bl	main

wait_for_interrupt:
	wfi
	mov	pc, lr			@ back to my caller

/* we do it this way because it's a 32-bit constant and
 * in some cases too far away to be loaded as just an offset
 * from IP
 */
.align 2
.Stack:
	.word CONFIG_STACK_TOP
.align 2
.Stack_size:
	.word CONFIG_STACK_SIZE
