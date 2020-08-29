/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/cache.h>

.section ".module_parameters", "aw", @progbits
/* stack_top indicates the stack to pull MTRR information from. */
.global post_car_stack_top
post_car_stack_top:
.long	0
.long	0

#if defined(__x86_64__)
.code64
.macro pop_eax_edx
	pop	%rax
	mov	%rax, %rdx
	shr	$32, %rdx
.endm
.macro pop_ebx_esi
	pop	%rbx
	mov	%rbx, %rsi
	shr	$32, %rsi
.endm
#else
.code32
.macro pop_eax_edx
	pop	%eax
	pop	%edx
.endm
.macro pop_ebx_esi
	pop	%ebx
	pop	%esi
.endm
#endif

.text
.global _start
_start:
	/* Assume stack alignment doesn't matter here as chipset_teardown_car
	   is expected to be implemented in assembly. */

	/* Migrate GDT to this text segment */
#if defined(__x86_64__)
	call	gdt_init64
#else
	call	gdt_init
#endif

#ifdef	__x86_64__
	mov	%rdi, _cbmem_top_ptr
#else
	/* The return argument is at 0(%esp), the calling argument at 4(%esp) */
	movl	4(%esp), %eax
	movl	%eax, _cbmem_top_ptr
#endif
	/* Make sure _cbmem_top_ptr hits dram before invd */
	movl	$1, %eax
	cpuid
	btl	$CPUID_FEATURE_CLFLUSH_BIT, %edx
	jnc	skip_clflush
	clflush	_cbmem_top_ptr

skip_clflush:
	/* chipset_teardown_car() is expected to disable cache-as-ram. */
	call	chipset_teardown_car

	/* Enable caching if not already enabled. */
#ifdef  __x86_64__
	mov	%cr0, %rax
	and     $(~(CR0_CD | CR0_NW)), %eax
	mov	%rax, %cr0
#else
	mov	%cr0, %eax
	and	$(~(CR0_CD | CR0_NW)), %eax
	mov	%eax, %cr0
#endif
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
#if defined(__x86_64__)
	mov	%rbp, %rdi
#else
	sub	$12, %esp
	push	%ebp
#endif

	call	soc_set_mtrrs
	/* Ignore fixing up %esp since we're setting it a new value. */

	/* eax: new top_of_stack with setup_stack_and_mtrrs data removed */
	movl	%eax, %esp
	/* Align stack to 16 bytes at call instruction. */
	andl	$0xfffffff0, %esp
	call	soc_enable_mtrrs
#else /* CONFIG_SOC_SETS_MSRS */
	/* Clear variable MTRRs. */
	pop_ebx_esi	/* ebx: Number to clear, esi: Number to set */
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
	test	%esi, %esi
	jz	2f
	mov	$(MTRR_PHYS_BASE(0)), %ecx
1:
	/* Write MTRR base. */
	pop_eax_edx
	wrmsr
	inc	%ecx
	/* Write MTRR mask. */
	pop_eax_edx
	wrmsr
	inc	%ecx

	dec	%esi
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
