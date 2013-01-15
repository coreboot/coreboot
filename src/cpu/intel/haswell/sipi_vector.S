/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
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
microcode_ptr:
.long 0
msr_table_ptr:
.long 0
msr_count:
.long 0
c_handler:
.long 0
c_handler_arg:
.long 0
apic_to_cpu_num:
.fill CONFIG_MAX_CPUS,1,0xff

.text
.code16
.global ap_start
ap_start:
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
	 * rmodule so ap_start can be used as a proxy for the load address. */
	movl	$(gdtaddr), %ebx
	sub	$(ap_start), %ebx

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

	/* The CPU number is calculated by reading the initial APIC id. */
	mov	$1, %eax
	cpuid
	/* Default APIC id in ebx[31:24]. Move it to bl. */
	bswap	%ebx
	mov	$(apic_to_cpu_num), %eax
	xor	%ecx, %ecx

1:
	cmp	(%eax, %ecx, 1), %bl
	je	1f
	inc	%ecx
	cmp	$CONFIG_MAX_CPUS, %ecx
	jne	1b

	/* This is bad. No CPU number found. However, the BSP should have setup
	 * the AP handler properly. Just park the CPU. */
	mov	$0x80, %dx
	movw	$0xdead, %ax
	outw	%ax, %dx
	jmp	halt_jump
1:
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
	jz	1f /* Bypass if no microde exists. */

	/* Get the Microcode version. */
	mov	$1, %eax
	cpuid
	mov	$IA32_BIOS_SIGN_ID, %ecx
	rdmsr
	/* If something already loaded skip loading again. */
	test	%edx, %edx
	jnz	1f

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

1:
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

	/* c_handler(cpu_num, *c_handler_arg) */
	push	c_handler_arg
	push	%esi	/* cpu_num */
	mov	c_handler, %eax
	call	*%eax
halt_jump:
	hlt
	jmp	halt_jump
