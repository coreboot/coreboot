/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/lapic_def.h>
#include <arch/ram_segs.h>

	.text
	.globl _secondary_start, _secondary_start_end, _secondary_gdt_addr
	.balign 4096
_secondary_start:
	.code16
	cli
	xorl	%eax, %eax
	movl	%eax, %cr3    /* Invalidate TLB*/

	/* On hyper threaded cpus, invalidating the cache here is
	 * very very bad.  Don't.
	 */

	/* setup the data segment */
	movw	%cs, %ax
	movw	%ax, %ds

	lgdtl	gdtaddr  - _secondary_start

	movl	%cr0, %eax
	andl	$0x7FFAFFD1, %eax /* PG,AM,WP,NE,TS,EM,MP = 0 */
	orl	$0x60000001, %eax /* CD, NW, PE = 1 */
	movl	%eax, %cr0

	ljmpl	$RAM_CODE_SEG, $__ap_protected_start

	/* This will get filled in by C code. */
_secondary_gdt_addr:
gdtaddr:
	.word   0	/* the table limit */
#if ENV_X86_64
	.quad   0
#else
	.long   0	/* we know the offset */
#endif

_secondary_start_end:

ap_protected_start:
	.code32
	lgdt	gdtaddr
	ljmpl	$RAM_CODE_SEG, $__ap_protected_start

__ap_protected_start:

	movw	$RAM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Load the Interrupt descriptor table */
	lidt	idtarg

#if ENV_X86_64
	/* entry64.inc preserves ebx. */
	#include <cpu/x86/64bit/entry64.inc>
	mov	secondary_stack, %rsp
	andl	$0xfffffff0, %esp
	mov	secondary_cpu_index, %rdi
#else
	/* Set the stack pointer, and flag that we are done */
	xorl	%eax, %eax
	movl	secondary_stack, %esp

	andl	$0xfffffff0, %esp
	sub	$12, %esp /* maintain 16-byte alignment for the call below */
	movl	secondary_cpu_index, %edi
	pushl	%edi
	movl	%eax, secondary_stack
#endif

	call	secondary_cpu_init
1:	hlt
	jmp	1b

.code32
