/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 * Copyright (C) 2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#define WAKEUP_BASE		0x600
#define RELOCATED(x)	(x - __wakeup + WAKEUP_BASE)

/* CR0 bits */
#define PE		(1 << 0)

	.code32
	.globl __wakeup
__wakeup:
	/* First prepare the jmp to the resume vector */
	mov	0x4(%esp), %eax	/* vector */
	/* last 4 bits of linear addr are taken as offset */
	andw	$0x0f, %ax
	movw	%ax, (__wakeup_offset)
	mov	0x4(%esp), %eax
	/* the rest is taken as segment */
	shr	$4, %eax
	movw	%ax, (__wakeup_segment)

	/* Then overwrite coreboot with our backed up memory */
	cld
	movl	8(%esp), %esi
	movl	12(%esp), %edi
	movl	16(%esp), %ecx
	shrl	$2, %ecx
	rep	movsl

	/* Activate the right segment descriptor real mode. */
	ljmp	$0x28, $RELOCATED(1f)
1:
.code16
	/* 16 bit code from here on... */

	/* Load the segment registers w/ properly configured
	 * segment descriptors. They will retain these
	 * configurations (limits, writability, etc.) once
	 * protected mode is turned off.
	 */
	mov	$0x30, %ax
	mov	%ax, %ds
	mov	%ax, %es
	mov	%ax, %fs
	mov	%ax, %gs
	mov	%ax, %ss

	/* Turn off protection */
	movl	%cr0, %eax
	andl	$~PE, %eax
	movl	%eax, %cr0

	/* Now really going into real mode */
	ljmp	$0, $RELOCATED(1f)
1:
	movw	$0x0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* This is a FAR JMP to the OS waking vector. The C code changed
	 * the address to be correct.
	 */
	.byte 0xea

__wakeup_offset = RELOCATED(.)
	.word 0x0000

__wakeup_segment = RELOCATED(.)
	.word 0x0000

	.globl __wakeup_size
__wakeup_size:
	.long . - __wakeup

