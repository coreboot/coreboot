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

/* POST_ZERO writes a POST_CODE at zero. Note that once we load %ds from %cs, "0" is at the start of this function!
 * This is debug only; you can't use it on systems with more than one core. It should be left off by default. 
 */
#define NO_POST_ZERO

#ifdef NO_POST_ZERO
#define POST_ZERO(x)
#else
#define POST_ZERO(x) movw	x, 0
#endif
	.text
	.globl _secondary_start, _secondary_start_end
_secondary_start:
1:
	.code16
	.balign 4096
	cli
	POST_ZERO($0xdead)
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/
	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */
	POST_ZERO($0)
	movl	$1b, %ebx
	POST_ZERO($1)
	POST_ZERO($2)

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, 2
	POST_ZERO($3)
	movw	%ax, %ds
	POST_ZERO($4)
	/* past this point, "0" means ds:0, i.e. cs:0, or the 
	 * segment part of the address. 
	 */

	data32	lgdt	gdtaddr  - _secondary_start
//	data32 lgdt %cs:gdtptr
	POST_ZERO($5)

	movl	%cr0, %eax
	POST_ZERO($6)
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	POST_ZERO($7)
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	POST_ZERO($8)
	movl	%eax, %cr0
	POST_ZERO($9)

	/* This jump pops us into 32-bit protected mode */
	data32 ljmp	$0x8, $secondary32
	POST_ZERO($0xa)
1:	
	.code32
secondary32:
	POST_ZERO($0x11)
	movw	$0x10, %ax
	movw	%ax, %ds
	/* having a post here for testing is useful. 
	 *If ds is bad for some reason, we'll reboot */
	POST_ZERO($0x13)
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs
	POST_ZERO($0x17)

	/* Load the Interrupt descriptor table */
	lidt	idtarg

	/* Set the stack pointer */
	movl	-4(%ebx),%esp
	movl	$0, -4(%ebx)
	/* tested to this point but not past it */
	/* AP sees the stack value set to 0 */

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
