/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/msr.h>
#include <arch/ram_segs.h>

#include "getsec_mtrr_setup.inc"

.macro PUSH_MSR x
	movl	$(\x), %ecx
	rdmsr
	push	%eax
	push	%edx
.endm

.macro POP_MSR x
	movl	$(\x), %ecx
	pop	%edx
	pop	%eax
	wrmsr
.endm

.macro CLEAR_MSR x
	movl	$(\x), %ecx
	xorl	%edx, %edx
	xorl	%eax, %eax
	wrmsr
.endm

.align 4
.text

/*
 * See "SAFER MODE EXTENSIONS REFERENCE."
 * Chapter "GETSEC[ENTERACCS] - Execute Authenticated Chipset Code" for reference.
 * Intel 64 and IA-32 Architectures Software Developer Manuals Vol 2D
 *
 * void getsec_enteraccs(uint32_t esi,
 *                       uint32_t acm_base,
 *                       uint32_t acm_size);
 */
.global getsec_enteraccs
getsec_enteraccs:

	/* Backup current register state */
	pushl	%ebp
	movl	%esp, %ebp

	pushal

	movl	%cr0, %eax
	pushl	%eax
	movl	%cr4, %eax
	pushl	%eax

	/* Pushed 10 32bit registers */

	/* Reserve space on stack for GDT */
	subl	$8, %esp

	PUSH_MSR MTRR_DEF_TYPE_MSR
	PUSH_MSR IA32_MISC_ENABLE
	PUSH_MSR MTRR_FIX_64K_00000
	PUSH_MSR MTRR_FIX_16K_80000
	PUSH_MSR MTRR_FIX_16K_A0000
	PUSH_MSR MTRR_FIX_4K_C0000
	PUSH_MSR MTRR_FIX_4K_C8000
	PUSH_MSR MTRR_FIX_4K_D0000
	PUSH_MSR MTRR_FIX_4K_D8000
	PUSH_MSR MTRR_FIX_4K_E0000
	PUSH_MSR MTRR_FIX_4K_F0000
	PUSH_MSR MTRR_FIX_4K_F8000

	/* Push variable MTRRs in ascending order */

	xorl	%ebx, %ebx
	jmp	cond_push_var_mtrrs

body_push_var_mtrrs:

	movl	%ebx, %ecx
	shll	%ecx
	addl	$(MTRR_PHYS_BASE(0)), %ecx
	rdmsr
	push	%eax
	push	%edx
	incl	%ecx	/* MTRR_PHYS_MASK */
	rdmsr
	push	%eax
	push	%edx

	incl	%ebx

cond_push_var_mtrrs:

	movl	$(MTRR_CAP_MSR), %ecx
	rdmsr
	andl	$(0xff), %eax
	cmp	%ebx, %eax
	jg	body_push_var_mtrrs

	/*
	 * Disable cache.
	 * Chapter 2.2.4.3
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	movl	%cr0, %eax
	orl	$(CR0_CD | CR0_NW), %eax
	movl	%eax, %cr0

	/* Disable all MTRRs */
	movl	$(MTRR_DEF_TYPE_MSR), %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	/*
	 * Clear fixed MTRRs.
	 * Chapter 2.2.5.1
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	CLEAR_MSR MTRR_FIX_64K_00000
	CLEAR_MSR MTRR_FIX_16K_80000
	CLEAR_MSR MTRR_FIX_16K_A0000
	CLEAR_MSR MTRR_FIX_4K_C0000
	CLEAR_MSR MTRR_FIX_4K_C8000
	CLEAR_MSR MTRR_FIX_4K_D0000
	CLEAR_MSR MTRR_FIX_4K_D8000
	CLEAR_MSR MTRR_FIX_4K_E0000
	CLEAR_MSR MTRR_FIX_4K_F0000
	CLEAR_MSR MTRR_FIX_4K_F8000

	/*
	 * Clear variable MTRRs
	 * Chapter 2.2.5.1
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	movl	$(MTRR_CAP_MSR), %ecx
	rdmsr
	andl	$(0xff), %eax
	movl	%eax, %ebx

	xorl	%eax, %eax
	xorl	%edx, %edx

	jmp	cond_clear_var_mtrrs

body_clear_var_mtrrs:

	decl	%ebx
	movl	%ebx, %ecx
	shll	%ecx
	addl	$(MTRR_PHYS_BASE(0)), %ecx
	wrmsr
	incl	%ecx	/* MTRR_PHYS_MASK */
	wrmsr

cond_clear_var_mtrrs:

	cmpl	$0, %ebx
	jnz	body_clear_var_mtrrs

	/*
	 * Setup BIOS ACM as WB
	 * Chapter A.1.1
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */

	/* Determine size of AC module */
	movl	12(%ebp), %eax			/* %eax = acmbase */
	movl	$1, %ebx
	movl	16(%ebp), %ebx			/* %ebx = acmsize */

	/* Round up to page size */
	addl	$(0xfff), %ebx
	andl	$(~0xfff), %ebx			/* Aligned to a page (4 KiB) */

	/*
	 * Use XMM to store local variables. This code will need to be
	 * used in romstage, and CAR will have been torn down by then.
	 */
	movd	%eax, %xmm0			/* XMM0: Base address of next MTRR */
	movd	%ebx, %xmm1			/* XMM1: Remaining size to cache */

	/*
	 * Important note: The MTRRs must cache less than a page (4 KiB)
	 * of unused memory after the BIOS ACM. Failure to do so will
	 * result in a TXT reset with Class Code 5, Major Error Code 2.
	 *
	 * The caller must have checked that there are enough variable
	 * MTRRs to cache the ACM size prior to invoking this routine.
	 */
	SET_UP_MTRRS_FOR_BIOS_ACM

	/*
	 * Now that the variable MTRRs have been set up, enable them.
	 */
	movl	$(MTRR_DEF_TYPE_MSR), %ecx
	rdmsr
	orl	$(MTRR_DEF_TYPE_EN), %eax
	wrmsr

	/* Enable cache - GPF# if not done */
	movl	%cr0, %eax
	andl	$(~(CR0_CD | CR0_NW)), %eax
	movl	%eax, %cr0

	/* Enable Numeric error - GPE# if not done */
	movl	%cr0, %eax
	orl	$(CR0_NE), %eax
	movl	%eax, %cr0

	/* Enable SMX and FXSTORE - for getsec */
	movl	%cr4, %eax
	orl	$(CR4_SMXE | CR4_OSFXSR), %eax
	movl	%eax, %cr4

	/*
	 * Save GDT
	 * Chapter A.1.2
	 * Intel TXT Software Development Guide (Document: 315168-015)
	 */
	sgdt	-48(%ebp)

	/* Backup stack pointer */
	movd	%esp, %xmm0
	movd	%ebp, %xmm1

	/* Backup %gs used by cpu_info() */
	movl	%gs, %eax
	movd	%eax, %xmm2

	/*
	 * Get function arguments.
	 * It's important to pass the exact ACM size as it's used by getsec to verify
	 * the integrity of ACM. Unlike the size for MTRR programming, which needs to
	 * be power of two.
	 *
	 * The following assembly code is based on tboot's tboot/include/txt/smx.h.
	 */
	movl	8(%ebp), %esi	/* flags */
	movl	12(%ebp), %ebx	/* acm_base */
	movl	16(%ebp), %ecx	/* acm_size */

	movl	$0, %edx	/* reserved, must be zero */
	movl	$0, %edi	/* must be zero */
	movl	$2, %eax	/* GetSec[ENTERACCS] */

	getsec

	/* Restore stack pointer */
	movd	%xmm0, %esp
	movd	%xmm1, %ebp

	/* Reload GDT */
	lgdt	-48(%ebp)

	/* Set cs */
	ljmp	$RAM_CODE_SEG, $1f
1:
	/* Fix segment registers */
	movl	$RAM_DATA_SEG, %eax
	movl	%eax, %ds
	movl	%eax, %es
	movl	%eax, %ss
	movl	%eax, %fs
	/* Restore %gs used by cpu_info */
	movd	%xmm2, %eax
	movl	%eax, %gs

	/* Disable cache */
	movl	%cr0, %eax
	orl	$(CR0_CD | CR0_NW), %eax
	movl	%eax, %cr0

	/* Pop variable MTRRs in descending order */

	movl	$(MTRR_CAP_MSR), %ecx
	rdmsr
	andl	$(0xff), %eax
	movl	%eax, %ebx

	jmp	cond_pop_var_mtrrs

body_pop_var_mtrrs:

	decl	%ebx
	movl	%ebx, %ecx
	shll	%ecx
	addl	$(MTRR_PHYS_MASK(0)), %ecx
	pop	%edx
	pop	%eax
	wrmsr
	decl	%ecx	/* MTRR_PHYS_BASE */
	pop	%edx
	pop	%eax
	wrmsr

cond_pop_var_mtrrs:

	cmpl	$0, %ebx
	jne	body_pop_var_mtrrs

	POP_MSR MTRR_FIX_4K_F8000
	POP_MSR MTRR_FIX_4K_F0000
	POP_MSR MTRR_FIX_4K_E0000
	POP_MSR MTRR_FIX_4K_D8000
	POP_MSR MTRR_FIX_4K_D0000
	POP_MSR MTRR_FIX_4K_C8000
	POP_MSR MTRR_FIX_4K_C0000
	POP_MSR MTRR_FIX_16K_A0000
	POP_MSR MTRR_FIX_16K_80000
	POP_MSR MTRR_FIX_64K_00000
	POP_MSR IA32_MISC_ENABLE
	POP_MSR MTRR_DEF_TYPE_MSR

	/* Enable cache */
	movl	%cr0, %eax
	andl	$(~(CR0_CD | CR0_NW)), %eax
	movl	%eax, %cr0

	/* Pop GDT */
	addl	$8, %esp

	popl	%eax
	movl	%eax, %cr4
	popl	%eax
	movl	%eax, %cr0

	popal

	movl	%ebp, %esp
	popl	%ebp

	ret
