/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>

.section ".module_parameters", "aw", @progbits
/* stack_top indicates the stack to pull MTRR information from. */
.global post_car_stack_top
post_car_stack_top:
.long	0
.long	0

.text
.global _start
_start:
	/* Assume stack alignment doesn't matter here as chipset_teardown_car
	   is expected to be implemented in assembly. */

	/* Migrate GDT to this text segment */
	call	gdt_init

#ifdef	__x86_64__
	mov	%rdi, _cbmem_top_ptr
#else
	/* The return argument is at 0(%esp), the calling argument at 4(%esp) */
	movl	4(%esp), %eax
	movl	%eax, _cbmem_top_ptr
#endif

	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	call	chipset_teardown_car

	/* Enable caching if not already enabled. */
	mov	%cr0, %eax
	and	$(~(CR0_CD | CR0_NW)), %eax
	mov	%eax, %cr0

	/* Ensure cache is clean. */
	invd

	/* Set up new stack. */
	mov	post_car_stack_top, %esp

	/*
	 * Honor variable MTRR information pushed on the stack with the
	 * following layout:
	 *
	 * Offset: Value
	 *   ...
	 *   0x14: MTRR mask 0 63:32
	 *   0x10: MTRR mask 0 31:0
	 *   0x0c: MTRR base 0 63:32
	 *   0x08: MTRR base 0 31:0
	 *   0x04: Number of variable MTRRs to set
	 *   0x00: Number of variable MTRRs to clear
	 */

#if CONFIG(SOC_SETS_MSRS)

	mov	%esp, %ebp
	/* Need to align stack to 16 bytes at the call instruction. Therefore
	   account for the 1 push. */
	andl	$0xfffffff0, %esp
	sub	$12, %esp
	push	%ebp
	call	soc_set_mtrrs
	/* Ignore fixing up %esp since we're setting it a new value. */

	/* eax: new top_of_stack with setup_stack_and_mtrrs data removed */
	movl	%eax, %esp
	/* Align stack to 16 bytes at call instruction. */
	andl	$0xfffffff0, %esp
	call	soc_enable_mtrrs
#else /* CONFIG_SOC_SETS_MSRS */
	/* Clear variable MTRRs. */
	pop	%ebx	/* Number to clear */
	test	%ebx, %ebx
	jz	2f
	xor	%eax, %eax
	xor	%edx, %edx
	mov	$(MTRR_PHYS_BASE(0)), %ecx
1:
	wrmsr
	inc	%ecx
	wrmsr
	inc	%ecx
	dec	%ebx
	jnz	1b
2:

	/* Set Variable MTRRs based on stack contents. */
	pop	%ebx	/* Number to set. */
	test	%ebx, %ebx
	jz	2f
	mov	$(MTRR_PHYS_BASE(0)), %ecx
1:
	/* Write MTRR base. */
	pop	%eax
	pop	%edx
	wrmsr
	inc	%ecx
	/* Write MTRR mask. */
	pop	%eax
	pop	%edx
	wrmsr
	inc	%ecx

	dec	%ebx
	jnz	1b
2:

	/* Enable MTRR. */
	mov	$(MTRR_DEF_TYPE_MSR), %ecx
	rdmsr
	/* Make default type uncacheable. */
	and	$(~(MTRR_DEF_TYPE_MASK)), %eax
	or	$(MTRR_DEF_TYPE_EN), %eax
	wrmsr
#endif /* CONFIG_SOC_SETS_MSRS */

	/* Align stack to 16 bytes at call instruction. */
	andl	$0xfffffff0, %esp
	/* Call into main for postcar. */
	call main
	/* Should never return. */
1:
	jmp	1b
