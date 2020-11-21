/* SPDX-License-Identifier: GPL-2.0-only */

/* For starting coreboot in protected mode */

#include <arch/rom_segs.h>
#include <cpu/x86/post_code.h>

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
