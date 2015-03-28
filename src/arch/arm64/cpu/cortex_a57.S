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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <arch/asm.h>
#include "cortex_a57.h"

ENTRY(arm64_cpu_early_setup)
	mrs x0, CPUECTLR_EL1
	orr x0, x0, #(1 << SMPEN_SHIFT)
	msr CPUECTLR_EL1, x0
	isb
	ret
ENDPROC(arm64_cpu_early_setup)
