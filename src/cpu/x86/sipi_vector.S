/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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
 * Foundation, Inc.
 */

/* The SIPI vector is responsible for initializing the APs in the sytem. It
 * loads microcode, sets up MSRs, and enables caching before calling into
 * C code. */

/* These segment selectors need to match the gdt entries in c_start.S. */
#define CODE_SEG 0x10
#define DATA_SEG 0x18

#define IA32_UPDT_TRIG 0x79
#define IA32_BIOS_SIGN_ID 0x8b

.section ".module_parameters", "aw", @progbits
ap_start_params:
gdtaddr:
.word 0 /* limit */
.long 0 /* table */
.word 0 /* unused */
idt_ptr:
.long 0
stack_top:
.long 0
stack_size:
.long 0
microcode_lock:
.long 0
microcode_ptr:
.long 0
msr_table_ptr:
.long 0
msr_count:
.long 0
c_handler:
.long 0
ap_count:
.long 0

.text
.code16
.global _start
_start:
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, %ds

	/* The gdtaddr needs to be releative to the data segment in order
	 * to properly dereference it. The .text section comes first in an
	 * rmodule so _start can be used as a proxy for the load address. */
	movl	$(gdtaddr), %ebx
	sub	$(_start), %ebx

	data32 lgdt (%ebx)

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	ljmpl	$CODE_SEG, $1f
1:
	.code32
	movw	$DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Load the Interrupt descriptor table */
	mov	idt_ptr, %ebx
	lidt	(%ebx)

	/* Obtain cpu number. */
	movl	ap_count, %eax
1:
	movl	%eax, %ecx
	inc	%ecx
	lock cmpxchg %ecx, ap_count
	jnz	1b

	/* Setup stacks for each CPU. */
	movl	stack_size, %eax
	mul	%ecx
	movl	stack_top, %edx
	subl	%eax, %edx
	mov	%edx, %esp
	/* Save cpu number. */
	mov	%ecx, %esi

	/* Determine if one should check microcode versions. */
	mov	microcode_ptr, %edi
	test	%edi, %edi
	jz	microcode_done /* Bypass if no microde exists. */

	/* Get the Microcode version. */
	mov	$1, %eax
	cpuid
	mov	$IA32_BIOS_SIGN_ID, %ecx
	rdmsr
	/* If something already loaded skip loading again. */
	test	%edx, %edx
	jnz	microcode_done

	/* Determine if parallel microcode loading is allowed. */
	cmp	$0xffffffff, microcode_lock
	je	load_microcode

	/* Protect microcode loading. */
lock_microcode:
	lock bts $0, microcode_lock
	jc	lock_microcode

load_microcode:
	/* Load new microcode. */
	mov	$IA32_UPDT_TRIG, %ecx
	xor	%edx, %edx
	mov	%edi, %eax
	/* The microcode pointer is passed in pointing to the header. Adjust
	 * pointer to reflect the payload (header size is 48 bytes). */
	add	$48, %eax
	pusha
	wrmsr
	popa

	/* Unconditionally unlock microcode loading. */
	cmp	$0xffffffff, microcode_lock
	je	microcode_done

	xor	%eax, %eax
	mov	%eax, microcode_lock

microcode_done:
	/*
	 * Load MSRs. Each entry in the table consists of:
	 * 0: index,
	 * 4: value[31:0]
	 * 8: value[63:32]
	 */
	mov	msr_table_ptr, %edi
	mov	msr_count, %ebx
	test	%ebx, %ebx
	jz	1f
load_msr:
	mov	(%edi), %ecx
	mov	4(%edi), %eax
	mov	8(%edi), %edx
	wrmsr
	add	$12, %edi
	dec	%ebx
	jnz	load_msr

1:
	/* Enable caching. */
	mov	%cr0, %eax
	and	$0x9fffffff, %eax /* CD, NW = 0 */
	mov	%eax, %cr0

	/* c_handler(cpu_num) */
	push	%esi	/* cpu_num */
	mov	c_handler, %eax
	call	*%eax
halt_jump:
	hlt
	jmp	halt_jump
