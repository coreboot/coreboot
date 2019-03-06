/*
 * This is the modern bootblock. It is used by platforms which select
 * C_ENVIRONMENT_BOOTBLOCK, and it prepares the system for C environment runtime
 * setup. The actual setup is done by hardware-specific code.
 *
 * It provides a bootflow similar to other architectures, and thus is considered
 * to be the modern approach.
 *
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cpu/x86/cr.h>

/*
 * Include the old code for reset vector and protected mode entry. That code has
 * withstood the test of time.
 */
#include <arch/x86/prologue.inc>
#include <cpu/x86/16bit/entry16.inc>
#include <cpu/x86/16bit/reset16.inc>
#include <cpu/x86/32bit/entry32.inc>

#if CONFIG(BOOTBLOCK_DEBUG_SPINLOOP)

	/* Wait for a JTAG debugger to break in and set EBX non-zero */
	xor	%ebx, %ebx

debug_spinloop:
	cmp	$0, %ebx
	jz	debug_spinloop
#endif

bootblock_protected_mode_entry:

#if !CONFIG(USE_MARCH_586)
	/* MMX registers required here */

	/* BIST result in eax */
	movl	%eax, %ebx

	/* Get an early timestamp */
	rdtsc
	movd	%ebx, %mm0
	movd	%eax, %mm1
	movd	%edx, %mm2
#endif

#if CONFIG(SSE)
enable_sse:
	mov	%cr4, %eax
	or	$CR4_OSFXSR, %ax
	mov	%eax, %cr4
#endif /* CONFIG(SSE) */

	/* We're done. Now it's up to platform-specific code */
	jmp	bootblock_pre_c_entry
