/* SPDX-License-Identifier: GPL-2.0-only */

	/* Preserve BIST. */
	movl	%eax, %ebp

	/* Enable SSE instructions. */
	movl	%cr4, %eax
	orl	$(1 << 9), %eax
	movl	%eax, %cr4

	/* Restore BIST. */
	movl	%ebp, %eax
