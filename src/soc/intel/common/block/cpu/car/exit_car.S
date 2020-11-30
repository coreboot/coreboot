/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cr.h>
#include <intelblocks/msr.h>

.text
.global chipset_teardown_car
chipset_teardown_car:

#if CONFIG(PAGING_IN_CACHE_AS_RAM)
	/*
	 * Since Page table is located in CAR, disable paging before CAR
	 * teardown. Also clear CR3 and CR4.PAE.
	 */
	mov	%cr0, %eax
	and	$(~(CR0_PG)), %eax
	mov	%eax, %cr0
	xor	%eax, %eax
	mov	%eax, %cr3
	mov	%cr4, %eax
	and	$(~(CR4_PAE)), %eax
	mov	%eax, %cr4
#endif

	/*
	 * Retrieve return address from stack as it will get trashed below if
	 * execution is utilizing the cache-as-ram stack.
	 */
#if ENV_X86_64
	pop	%rbx
#else
	pop	%ebx
#endif

	/* Disable MTRRs. */
	mov	$(MTRR_DEF_TYPE_MSR), %ecx
	rdmsr
	and	$(~(MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN)), %eax
	wrmsr

#if CONFIG(INTEL_CAR_NEM)
.global car_nem_teardown
car_nem_teardown:

	/* invalidate cache contents. */
	invd

	/* Knock down bit 1 then bit 0 of NEM control not combining steps. */
	mov	$(MSR_EVICT_CTL), %ecx
	rdmsr
	and	$(~(1 << 1)), %eax
	wrmsr
	and	$(~(1 << 0)), %eax
	wrmsr

#elif CONFIG(INTEL_CAR_CQOS)
.global car_cqos_teardown
car_cqos_teardown:

	/* Go back to all-evicting mode, set both masks to all-1s */
	mov	$MSR_L2_QOS_MASK(0), %ecx
	rdmsr
	mov	$~0, %al
	wrmsr

	mov	$MSR_L2_QOS_MASK(1), %ecx
	rdmsr
	mov	$~0, %al
	wrmsr

	/* Reset CLOS selector to 0 */
	mov	$IA32_PQR_ASSOC, %ecx
	rdmsr
	and	$~IA32_PQR_ASSOC_MASK, %edx
	wrmsr

#elif CONFIG(INTEL_CAR_NEM_ENHANCED)
.global car_nem_enhanced_teardown
car_nem_enhanced_teardown:

	/* invalidate cache contents. */
	invd

	/* Knock down bit 1 then bit 0 of NEM control not combining steps. */
	mov	$(MSR_EVICT_CTL), %ecx
	rdmsr
	and	$(~(1 << 1)), %eax
	wrmsr
	and	$(~(1 << 0)), %eax
	wrmsr

	/* Reset CLOS selector to 0 */
	mov	$IA32_PQR_ASSOC, %ecx
	rdmsr
	and	$~IA32_PQR_ASSOC_MASK, %edx
	wrmsr
#endif

	/* Return to caller. */
#if ENV_X86_64
	jmp	*%rbx
#else
	jmp	*%ebx
#endif
