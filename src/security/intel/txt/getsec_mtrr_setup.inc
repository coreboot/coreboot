/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>

#define MTRR_HIGH_MASK $((1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1)

/*
 * Configure the MTRRs to cache the BIOS ACM. No general-purpose
 * registers are preserved. Inputs are taken from SSE registers:
 *
 *    %xmm0: BIOS ACM base
 *    %xmm1: BIOS ACM size
 *
 * These two SSE registers are not preserved, but the others are.
 */
.macro SET_UP_MTRRS_FOR_BIOS_ACM

	/* Get the number of variable MTRRs */
	movl	$(MTRR_CAP_MSR), %ecx
	rdmsr
	andl	$(0xff), %eax

	/* Initialize ECX */
	movl	$(MTRR_PHYS_BASE(0)), %ecx

	jmp	cond_allocate_var_mtrrs

body_allocate_var_mtrrs:

	/* Program MTRR base */
	xorl	%edx, %edx
	movd	%xmm0, %eax
	orl	$(MTRR_TYPE_WRBACK), %eax
	wrmsr
	incl	%ecx				/* Move index to MTRR_PHYS_MASK */

	/* Temporarily transfer MSR index to EDX so that CL can be used */
	movl	%ecx, %edx

	/* Determine next size to cache */
	bsr	%ebx, %ecx
	movl	$(1), %ebx
	shl	%cl, %ebx			/* Can only use CL here */

	/* Restore ECX */
	movl	%edx, %ecx

	/* Update saved base address */
	addl	%ebx, %eax
	movd	%eax, %xmm0

	/* Update saved remaining size */
	movd	%xmm1, %eax
	subl	%ebx, %eax
	movd	%eax, %xmm1

	/* Program MTRR mask */
	movl	MTRR_HIGH_MASK, %edx
	xorl	%eax, %eax
	subl	%ebx, %eax			/* %eax = 4GIB - size to cache */
	orl	$(MTRR_PHYS_MASK_VALID), %eax
	wrmsr
	incl	%ecx				/* Move index to next MTRR_PHYS_BASE */

cond_allocate_var_mtrrs:

	/* Check if we still need to cache something */
	movd	%xmm1, %ebx
	andl	%ebx, %ebx

	jnz	body_allocate_var_mtrrs

.endm
