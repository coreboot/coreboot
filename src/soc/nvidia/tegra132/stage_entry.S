/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/asm.h>

ENTRY(stage_entry)
	/* Initialize PSTATE, SCTLR and caches to clean state, set up stack. */
	bl	arm64_init_cpu

	/* Jump to Tegra-specific C entry point. */
	bl	ramstage_entry
ENDPROC(stage_entry)
