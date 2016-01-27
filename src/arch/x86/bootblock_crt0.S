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
 */

#define CR0_MP		(1 << 1)
#define CR0_EM		(1 << 2)

#define CR4_OSFXSR	(1 << 9)
#define CR4_OSXMMEXCPT	(1 << 10)

/*
 * Include the old code for reset vector and protected mode entry. That code has
 * withstood the test of time.
 */
#include <arch/x86/prologue.inc>
#include <cpu/x86/16bit/entry16.inc>
#include <cpu/x86/16bit/reset16.inc>
#include <cpu/x86/32bit/entry32.inc>


bootblock_protected_mode_entry:
	/* Save BIST result */
	movd	%eax, %mm0
	/* Save an early timestamp */
	rdtsc
	movd	%eax, %mm1
	movd	%edx, %mm2

#if !IS_ENABLED(CONFIG_SSE)
enable_sse:
	mov	%cr0, %eax
	and	$~CR0_EM, %ax		/* Clear coprocessor emulation CR0.EM */
	or	$CR0_MP, %ax		/* Set coprocessor monitoring  CR0.MP */
	mov	%eax, %cr0
	mov	%cr4, %eax
	or	$(CR4_OSFXSR | CR4_OSXMMEXCPT), %ax
	mov	%eax, %cr4
#endif /* IS_ENABLED(CONFIG_SSE) */

	/* We're done. Now it's up to platform-specific code */
	jmp	bootblock_pre_c_entry
