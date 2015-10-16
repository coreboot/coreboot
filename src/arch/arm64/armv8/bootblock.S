/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>

/* NOTE: When making changes to general ARM64 initialization, keep in mind that
 * there are other CPU entry points, using BOOTBLOCK_CUSTOM or entering the CPU
 * in a later stage (like Tegra). Changes should generally be put into
 * arm64_cpu_init so they can be shared between those instances. */

ENTRY(_start)
	/* Initialize PSTATE, SCTLR and caches to clean state, set up stack. */
	bl	arm64_init_cpu

	bl	main
ENDPROC(_start)
