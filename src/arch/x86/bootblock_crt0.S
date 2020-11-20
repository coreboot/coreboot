/* SPDX-License-Identifier: GPL-2.0-or-later */
/*
 * This is the modern bootblock. It prepares the system for C environment runtime
 * setup. The actual setup is done by hardware-specific code.
 *
 * It provides a bootflow similar to other architectures, and thus is considered
 * to be the modern approach.
 *
 */

#include <cpu/x86/cr.h>

.section .init._start, "ax", @progbits

/*
 * Include the old code for reset vector and protected mode entry. That code has
 * withstood the test of time.
 */
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
	movd	%eax, %mm0

	/* Get an early timestamp */
	rdtsc
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
