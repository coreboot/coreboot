/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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
 */

#include <cpu/x86/mtrr.h>
#include <cpu/x86/cr.h>
#include <soc/cpu.h>

.text
.global chipset_teardown_car
chipset_teardown_car:
	/*
	 * Retrieve return address from stack as it will get trashed below if
	 * execution is utilizing the cache-as-ram stack.
	 */
	pop	%ebx

	/* Disable MTRRs. */
	mov	$(MTRR_DEF_TYPE_MSR), %ecx
	rdmsr
	and	$(~(MTRR_DEF_TYPE_EN | MTRR_DEF_TYPE_FIX_EN)), %eax
	wrmsr

#if IS_ENABLED(CONFIG_CAR_CQOS)
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
	mov	$MSR_IA32_PQR_ASSOC, %ecx
	rdmsr
	and	$~IA32_PQR_ASSOC_MASK, %edx
	wrmsr
#endif
	/* invalidate cache contents. */
	invd

#if IS_ENABLED(CONFIG_CAR_NEM)
	/* Knock down bit 1 then bit 0 of NEM control not combining steps. */
	mov	$(MSR_EVICT_CTL), %ecx
	rdmsr
	and	$(~(1 << 1)), %eax
	wrmsr
	and	$(~(1 << 0)), %eax
	wrmsr
#endif

	/* Return to caller. */
	jmp	*%ebx
