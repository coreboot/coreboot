/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013-2014 Sage Electronic Engineering, LLC.
 * Copyright (C) 2016 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

.equ		IA32_PQR_ASSOC,			0x0c8f

	/* Disable MTRR by clearing the IA32_MTRR_DEF_TYPE MSR E flag. */
	movl	$MTRR_DEF_TYPE_MSR, %ecx
	rdmsr
	andl	$(~MTRR_DEF_TYPE_EN), %eax
	wrmsr

	/* Invalidate Cache */
	invd

	/*
	 * Disable No-Eviction Mode Run State by clearing
	 * NO_EVICT_MODE MSR 2E0h bit [1] = 0
	 */
	movl	$0x000002E0, %ecx
	rdmsr
	andl	$~(0x2), %eax
	wrmsr

	/*
	 * Disable No-Eviction Mode Setup State by clearing
	 * NO_EVICT_MODE MSR 2E0h bit [0] = 0
	 */
	rdmsr
	andl	$~(0x1), %eax
	wrmsr

	/*
	 * Set IA32_PQR_ASSOC = 0x00
	 * This step guarantees that no protected way remain in LLC cache,
	 * all the ways are open for the evictions.
	 */
	movl	$IA32_PQR_ASSOC, %ecx
	movl	$0x00, %eax
	xorl	%edx, %edx
	wrmsr
