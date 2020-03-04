/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 */

#include <arch/asm.h>

/* NOTE: When making changes to general ARM64 initialization, keep in mind that
 * there are other CPU entry points, using BOOTBLOCK_CUSTOM or entering the CPU
 * in a later stage (like Tegra). Changes should generally be put into
 * arm64_init_cpu so they can be shared between those instances. */

ENTRY(_start)
	/* Initialize PSTATE, SCTLR and caches to clean state, set up stack. */
	bl	arm64_init_cpu

	bl	main
ENDPROC(_start)
