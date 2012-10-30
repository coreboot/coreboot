/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

/*
 * +--------------------------------+
 * | SMM Handler C Code             |
 * +--------------------------------+ 0x14000
 * | SMM Handler Heap               |
 * +--------------------------------+ 0x10000
 * |  Save State Map Node 0         |
 * |  Save State Map Node 1         |
 * |  Save State Map Node 2         |
 * |  Save State Map Node 3         |
 * |  ...                           |
 * +--------------------------------+ 0xf000
 * |                                |
 * |                                |
 * | EARLY DATA (lock, vectors)     |
 * +--------------------------------+ 0x8400
 * | SMM Entry Node 0 (+ stack)     |
 * +--------------------------------+ 0x8000
 * | SMM Entry Node 1 (+ stack)     |
 * | SMM Entry Node 2 (+ stack)     |
 * | SMM Entry Node 3 (+ stack)     |
 * | ...                            |
 * +--------------------------------+ 0x7400
 * |                                |
 * | SMM Handler Assembly Stub      |
 * |                                |
 * +--------------------------------+ TSEG
 *
 */

#define LAPIC_ID	0xfee00020
#define SMM_STACK_SIZE	(0x400 - 0x10)

/* Values for the xchg lock */
#define SMI_LOCKED	0
#define SMI_UNLOCKED	1

#define __PRE_RAM__
#if CONFIG_NORTHBRIDGE_INTEL_SANDYBRIDGE || CONFIG_NORTHBRIDGE_INTEL_IVYBRIDGE
#include <northbridge/intel/sandybridge/sandybridge.h>
#define TSEG_BAR (DEFAULT_PCIEXBAR | TSEG)
#elif CONFIG_NORTHBRIDGE_INTEL_HASWELL
#include <northbridge/intel/haswell/haswell.h>
#define TSEG_BAR (DEFAULT_PCIEXBAR | TSEG)
#else
#error "Northbridge must define TSEG_BAR."
#endif

/* initially SMM is some sort of real mode. Let gcc know
 * how to treat the SMM handler stub
 */

.section ".handler", "a", @progbits

.code16

/**
 * SMM code to enable protected mode and jump to the
 * C-written function void smi_handler(u32 smm_revision)
 *
 * All the bad magic is not all that bad after all.
 */
smm_handler_start:
	movl	$(TSEG_BAR), %eax	/* Get TSEG base from PCIE */
	addr32	movl (%eax), %edx	/* Save TSEG_BAR in %edx */
	andl	$~1, %edx		/* Remove lock bit */

	/* Obtain lock */
	movl	%edx, %ebx
	addl	$(smm_lock), %ebx
	movw	$SMI_LOCKED, %ax
	addr32	xchg %ax, (%ebx)
	cmpw	$SMI_UNLOCKED, %ax

	/* Proceed if we got the lock */
	je	smm_check_prot_vector

	/* If we did not get the lock, wait for release */
wait_for_unlock:
	pause
	addr32	movw (%ebx), %ax
	cmpw	$SMI_LOCKED, %ax
	je	wait_for_unlock
	rsm

smm_check_prot_vector:
	/* See if we need to adjust protected vector */
	movl	%edx, %eax
	addl	$(smm_prot_vector), %eax
	addr32	movl (%eax), %ebx
	cmpl	$(smm_prot_start), %ebx
	jne	smm_check_gdt_vector

	/* Adjust vector with TSEG offset */
	addl	%edx, %ebx
	addr32	movl %ebx, (%eax)

smm_check_gdt_vector:
	/* See if we need to adjust GDT vector */
	movl	%edx, %eax
	addl	$(smm_gdt_vector + 2), %eax
	addr32	movl (%eax), %ebx
	cmpl	$(smm_gdt - smm_handler_start), %ebx
	jne	smm_load_gdt

	/* Adjust vector with TSEG offset */
	addl	%edx, %ebx
	addr32	movl %ebx, (%eax)

smm_load_gdt:
	movl    $(smm_gdt_vector), %ebx
	addl	%edx, %ebx        /* TSEG base in %edx */
	data32  lgdt (%ebx)

	movl    %cr0, %eax
	andl    $0x1FFAFFD1, %eax /* CD,NW,PG,AM,WP,NE,TS,EM,MP = 0 */
	orl     $0x1, %eax        /* PE = 1 */
	movl    %eax, %cr0

	/* Enable protected mode */
	movl	$(smm_prot_vector), %eax
	addl	%edx, %eax
	data32	ljmp *(%eax)

.code32
smm_prot_start:
	/* Use flat data segment */
	movw    $0x10, %ax
	movw    %ax, %ds
	movw    %ax, %es
	movw    %ax, %ss
	movw    %ax, %fs
	movw    %ax, %gs

	/* Get this CPU's LAPIC ID */
	movl	$LAPIC_ID, %esi
	movl	(%esi), %ecx
	shr	$24, %ecx

	/* calculate stack offset by multiplying the APIC ID
	 * by 1024 (0x400), and save that offset in ebp.
	 */
	shl	$10, %ecx
	movl	%ecx, %ebp

	/* We put the stack for each core right above
	 * its SMM entry point. Core 0 starts at SMM_BASE + 0x8000,
	 * we spare 0x10 bytes for the jump to be sure.
	 */
	movl	$0x8010, %eax	/* core 0 address */
	addl	%edx, %eax	/* addjust for TSEG */
	subl	%ecx, %eax	/* subtract offset, see above */
	movl	%eax, %ebx	/* Save bottom of stack in ebx */

	/* clear stack */
	cld
	movl	%eax, %edi
	movl	$(SMM_STACK_SIZE >> 2), %ecx
	xorl	%eax, %eax
	rep	stosl

	/* set new stack */
	addl	$SMM_STACK_SIZE, %ebx
	movl	%ebx, %esp

	/* Get SMM revision */
	movl	$0xfefc, %ebx	/* core 0 address */
	addl	%edx, %ebx	/* addjust for TSEG */
	subl	%ebp, %ebx	/* subtract core X offset */
	movl	(%ebx), %eax
	pushl	%eax

	/* Call 32bit C handler */
	call	smi_handler

	/* Release lock */
	movl	$(TSEG_BAR), %eax	/* Get TSEG base from PCIE */
	movl	(%eax), %ebx		/* Save TSEG_BAR in %ebx */
	andl	$~1, %ebx		/* Remove lock bit */
	addl	$(smm_lock), %ebx
	movw	$SMI_UNLOCKED, %ax
	xchg	%ax, (%ebx)

	/* To return, just do rsm. It will "clean up" protected mode */
	rsm

smm_gdt:
	/* The first GDT entry can not be used. Keep it zero */
	.long	0x00000000, 0x00000000

	/* gdt selector 0x08, flat code segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x9b, 0xcf, 0x00 /* G=1 and 0x0f, 4GB limit */

	/* gdt selector 0x10, flat data segment */
	.word	0xffff, 0x0000
	.byte	0x00, 0x93, 0xcf, 0x00

smm_gdt_end:

.section ".earlydata", "a", @progbits

.code16

.align	4, 0xff

smm_lock:
	.word	SMI_UNLOCKED

.align	4, 0xff

smm_prot_vector:
	.long	smm_prot_start
	.short	8

.align	4, 0xff

smm_gdt_vector:
	.word	smm_gdt_end - smm_gdt - 1
	.long	smm_gdt - smm_handler_start

.section ".jumptable", "a", @progbits

/* This is the SMM jump table. All cores use the same SMM handler
 * for simplicity. But SMM Entry needs to be different due to the
 * save state area. The jump table makes sure all CPUs jump into the
 * real handler on SMM entry.
 */

/* This code currently supports up to 16 CPU cores. If more than 16 CPU cores
 * shall be used, below table has to be updated, as well as smm_tseg.ld
 */

/* When using TSEG do a relative jump and fix up the CS later since we
 * do not know what our TSEG base is yet.
 */

.code16
jumptable:
	/* core 15 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 14 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 13 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 12 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 11 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 10 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 9 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 8 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 7 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 6 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 5 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 4 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 3 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 2 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 1 */
	jmp smm_handler_start
.align 1024, 0x00
	/* core 0 */
	jmp smm_handler_start
.align 1024, 0x00
