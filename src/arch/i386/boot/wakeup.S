/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 Rudolf Marek <r.marek@assembler.cz>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License v2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
	.text
	.code32
	.globl acpi_jmp_to_realm_wakeup
	/* This function does the PM -> RM switch, but
	it can run >1MB even in real mode */

acpi_jmp_to_realm_wakeup:
	mov	0x4(%esp), %eax
	/* last 4 bits of linear addr are taken as offset */
	andw	$0x0f, %ax
	movw	%ax, (jmp_off)
	mov	0x4(%esp), %eax
	/* the rest is taken as segment */
	shr	$4, %eax
	movw	%ax, (jmp_seg)
	lgdt	gdtaddr_wakeup
	movl	$0x008,%eax
	mov	%eax,%ds
	movl	%eax,%es
	movl	%eax,%ss
	movl	%eax,%fs
	movl	%eax,%gs
	ljmp $0x0010,$reload_cs
        .code16gcc
reload_cs:
	/* switch off PE */
	movl	%cr0, %eax
	andb	$0xfe,%al
	movl	%eax, %cr0
	movw	$0x0, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs
	/* far jump to OS waking vector. The linear addr is changed to SEG and OFFSET
	   check ACPI specs or above code for details */
	.byte 0xea
jmp_off:
	.word 0
jmp_seg:
	.word 0

.code32
gdt_wakeup_limit = gdt_wakeup_end - gdt_wakeup - 1	/* compute the table limit */
	gdtaddr_wakeup:
	.word   gdt_wakeup_limit	/* the table limit */
	.long   gdt_wakeup             /* we know the offset */

	 .data

	/* This is the gdt for GCC part of coreboot.
	 * It is different from the gdt in ROMCC/ASM part of coreboot
	 * which is defined in entry32.inc */
gdt_wakeup:
	/* selgdt 0, unused */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x00, 0x00, 0x00

	/* selgdt 8, flat data segment 16bit */
	.word	0x0000, 0x0000		/* dummy */
	.byte	0x00, 0x93, 0x8f, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */	

	/* selgdt 0x10, flat code segment 16bit */ 
	.word	0xffff, 0x0000		
	.byte	0x00, 0x9b, 0x8f, 0x00 /* G=1 and 0x0f, So we get 4Gbytes for limit */	

gdt_wakeup_end:

	.previous
.code32
