/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/cr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>

#include "getsec_mtrr_setup.inc"

#define MTRR_HIGH_MASK	$((1 << (CONFIG_CPU_ADDR_BITS - 32)) - 1)

#define NO_EVICT_MODE	0x2e0

.align 4
.text

/*
 * void getsec_sclean(const uint32_t acm_base, const uint32_t acm_size);
 */
.global getsec_sclean
getsec_sclean:
	/*
	 * At this point, it is certain that the BIOS ACM will be run.
	 * This requires tearing down CAR, which cannot be undone.
	 *
	 * From here onwards, the only way out is to reset the system.
	 */

	/* Enable SMXE, SSE and debug extensions */
	movl	%cr4, %eax
	orl	$(CR4_OSFXSR | CR4_DE | CR4_SMXE), %eax
	movl	%eax, %cr4

	/*
	 * Save arguments into SSE registers. We need to tear down CAR
	 * before launching the BIOS ACM, which will destroy the stack.
	 */
	movd	4(%esp), %xmm2			/* acm_base */
	movd	8(%esp), %xmm3			/* acm_size */

	/* Disable cache */
	movl	%cr0, %eax
	orl	$(CR0_CD | CR0_NE), %eax
	andl	$(~(CR0_NW)), %eax
	movl	%eax, %cr0

	/* Invalidate the cache */
	invd

	/* Disable MTRRs */
	movl	$(MTRR_DEF_TYPE_MSR), %ecx
	xorl	%eax, %eax
	xorl	%edx, %edx
	wrmsr

	/* Disable NEM, needs to be done in two steps */
	movl	$NO_EVICT_MODE, %ecx
	rdmsr
	andl	$~2, %eax			/* Clear NEM Run bit */
	wrmsr
	andl	$~1, %eax			/* Clear NEM Setup bit */
	wrmsr

	/* Invalidate the cache, again */
	invd

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
	incl	%ecx				/* MTRR_PHYS_MASK */
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
	movd	%xmm2, %eax			/* acm_base */
	movd	%xmm3, %ebx			/* acm_size */

	/* Round up to page size */
	addl	$(0xfff), %ebx
	andl	$(~0xfff), %ebx			/* Aligned to a page (4 KiB) */

	/* Use SSE registers to store local variables */
	movd	%eax, %xmm0
	movd	%ebx, %xmm1

	/*
	 * Important note: The MTRRs must cache less than a page (4 KiB)
	 * of unused memory after the BIOS ACM. Not doing so on Haswell
	 * will cause a TXT reset with Class Code 5, Major Error Code 2.
	 *
	 * The caller must have checked that there are enough variable
	 * MTRRs to cache the ACM size prior to invoking this routine.
	 */
	SET_UP_MTRRS_FOR_BIOS_ACM

	/* Enable variable MTRRs */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	orl	$MTRR_DEF_TYPE_EN, %eax
	wrmsr

	/* Enable cache - CR0_NW is and stays clear */
	movl	%cr0, %eax
	andl	$~(CR0_CD), %eax
	movl	%eax, %cr0

	/*
	 * Get function arguments.
	 * It's important to pass the exact ACM size as it's used by getsec to verify
	 * the integrity of ACM. Unlike the size for MTRR programming, which needs to
	 * be power of two.
	 *
	 * Note: Do not forget that CAR has been torn down, so the stack doesn't exist.
	 */
	movl	$2, %eax			/* GETSEC[ENTERACCS] */
	movd	%xmm2, %ebx			/* acm_base */
	movd	%xmm3, %ecx			/* acm_size */
	movl	$0, %edx			/* reserved, must be zero */
	movl	$0, %edi			/* must be zero */
	movl	$0, %esi			/* SCLEAN */

	getsec

	/*
	 * The platform state after SCLEAN is undefined. The only sane
	 * thing to do afterwards is to reset the platform. Note that
	 * the BIOS ACM should already reset the platform, so this code
	 * may not always be reached, but keep it here just to be sure.
	 */
#if 1
	movw	$0xcf8, %dx
	movl	$0x8000F8AC, %eax
	outl	%eax, %dx

	movw	$0xcfc, %dx
	inl	%dx, %eax
	andl	$~(1 << 20), %eax
	outl	%eax, %dx
#endif

	movw	$0xcf9, %dx
	movb	$0, %al
	outb	%al, %dx

	movw	$0xcf9, %dx
	movb	$0x0e, %al
	outb	%al, %dx

	cli

	hlt

	ret
