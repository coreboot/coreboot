/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2002 Linux Networx
 * (Written by Eric Biederman <ebiederman@lnxi.com> for Linux Networx)
 * Copyright (C) 2004 Ollie Lo
 * Copyright (C) 2005 YingHai Lu
 * Copyright (C) Copyright (C) 2005-2007 Stefan Reinauer <stepan@openbios.org>
 * Copyright (C) 2009 Ronald G. Minnich <rminnich@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */
	.text
	.globl _secondary_start, _secondary_start_end
_secondary_start:
1:
	.code16
	.balign 4096
	cli
	movl	$1b, %ebx
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/
	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, %ds

	data32	lgdt	gdtaddr  - _secondary_start

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	ljmpl	$0x10, $1f
1:	
	.code32
secondary32:
	movw	$0x18, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Load the Interrupt descriptor table */
	lidt	idtarg

	/* Set the stack pointer */
	movl	-4(%ebx),%esp
	movl	$0, -4(%ebx)

	call	secondary_cpu_init
1:	hlt
	jmp	1b
	.align 4

gdt_limit = gdt_end - gdt - 1		/* Compute the table limit. */

gdt:
gdtptr:
	.word	gdt_end - gdt -1	/* Compute the table limit. */
	.long	gdt			/* We know the offset. */
	.word	0

	/* selgdt 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

	/* selgdt 0x18, flat code segment for CAR */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00

	/* selgdt 0x20, flat data segment for CAR */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00
gdt_end:


	gdtaddr:
	.word   gdt_limit	/* the table limit */
	.long   gdt             /* we know the offset */
idtarg:
        .word   _idt_end - _idt - 1     /* limit */
        .long   _idt
        .word   0       
_idt:
        .fill   20, 8, 0        # idt is unitiailzed
_idt_end:


_secondary_start_end:
.code32
