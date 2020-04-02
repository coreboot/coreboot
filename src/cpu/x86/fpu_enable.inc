/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

__fpu_start:
	/* Preserve BIST. */
	movl	%eax, %ebp

	/*
	 * Clear the CR0[2] bit (the "Emulation" flag, EM).
	 *
	 * This indicates that the processor has an (internal or external)
	 * x87 FPU, i.e. floating point operations will be executed by the
	 * hardware (and not emulated in software).
	 *
	 * Additionally, if this bit is not cleared, MMX/SSE instructions won't
	 * work, i.e., they will trigger an invalid opcode exception (#UD).
	 */
	movl	%cr0, %eax
	andl	$~(1 << 2), %eax
	movl	%eax, %cr0

	/* Restore BIST. */
	movl	%ebp, %eax
