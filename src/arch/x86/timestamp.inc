/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * Store the initial timestamp for booting in mmx registers. This works
 * because the bootblock isn't being compiled with MMX support so mm1 and
 * mm2 will be preserved into romstage.
 */
	.code32

.global stash_timestamp
stash_timestamp:

	/* Save the BIST value */
	movl	%eax, %ebx

	finit
	rdtsc
	movd	%ebx, %mm0
	movd	%eax, %mm1
	movd	%edx, %mm2

	/* Restore the BIST value to %eax */
	movl	%ebx, %eax
