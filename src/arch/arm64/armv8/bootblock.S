/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2013  Google Inc.
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
	 * Assumes 64-byte alignment.
	 */
	.skip 128

reset:
	/*
	 * Set the cpu to SVC32 mode and unmask aborts. Aborts might happen
	 * before logging is turned on and may crash the machine, but at least
	 * the problem will show up near the code that causes it.
	 */
	/* FIXME: Not using supervisor mode, does it apply for aarch64? */

	msr	daifclr, #0xc /* Unmask Debug and System exceptions */
	msr	daifset, #0x3 /* Mask IRQ, FIQ */

	bl	arm_init_caches

	/*
	 * Initialize the stack to a known value. This is used to check for
	 * stack overflow later in the boot process.
	 */
	ldr	x0, .Stack
	ldr	x1, .Stack_size
	sub	x0, x0, x1
	ldr	x1, .Stack
	ldr	x2, =0xdeadbeefdeadbeef
init_stack_loop:
	str	x2, [x0]
	add	x0, x0, #8
	cmp	x0, x1
	bne	init_stack_loop

/* Set stackpointer in internal RAM to call bootblock main() */
call_bootblock:
	ldr	x0, .Stack /* Set up stack pointer */
	mov	sp, x0
	ldr	x0, =0x00000000

	sub	sp, sp, #16

	/*
	 * Switch to EL2 already because Linux requires to be
	 * in EL1 or EL2, see its "Booting AArch64 Linux" doc
	 */
	bl	switch_el3_to_el2
	bl	main

.align 3
.Stack:
	.word CONFIG_STACK_TOP
.align 3
.Stack_size:
	.word CONFIG_STACK_SIZE
	.section ".id", "a", %progbits

	.globl __id_start
__id_start:
ver:
	.asciz COREBOOT_VERSION
vendor:
	.asciz CONFIG_MAINBOARD_VENDOR
part:
	.asciz CONFIG_MAINBOARD_PART_NUMBER
.long __id_end - ver  /* Reverse offset to the vendor id */
.long __id_end - vendor  /* Reverse offset to the vendor id */
.long __id_end - part    /* Reverse offset to the part number */
.long CONFIG_ROM_SIZE    /* Size of this romimage */
	.globl __id_end

__id_end:
.previous
