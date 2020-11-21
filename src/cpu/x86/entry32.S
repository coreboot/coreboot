/* SPDX-License-Identifier: GPL-2.0-or-later */

/* For starting coreboot in protected mode */

/*
 * This is the modern bootblock. It prepares the system for C environment runtime
 * setup. The actual setup is done by hardware-specific code.
 *
 * It provides a bootflow similar to other architectures, and thus is considered
 * to be the modern approach.
 *
 */

#include <arch/rom_segs.h>
#include <cpu/x86/cr.h>
#include <cpu/x86/post_code.h>

.section .init, "ax", @progbits

	.code32
/*
 *	When we come here we are in protected mode.
 *	NOTE aligned to 4 so that we are sure that the prefetch
 *	cache will be reloaded.
 */
	.align	4

.globl __protected_start
__protected_start:
	/* Save the BIST value */
	movl	%eax, %ebp

#if !CONFIG(NO_EARLY_BOOTBLOCK_POSTCODES)
	post_code(POST_ENTER_PROTECTED_MODE)
#endif

	movw	$ROM_DATA_SEG, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %ss
	movw	%ax, %fs
	movw	%ax, %gs

	/* Restore the BIST value to %eax */
	movl	%ebp, %eax

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
