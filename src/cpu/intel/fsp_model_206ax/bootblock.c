/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
 * Foundation, Inc.
 */

#include <stdint.h>
#include <arch/cpu.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <arch/io.h>

#include <cpu/intel/microcode/microcode.c>
#include "model_206ax.h"

static void bootblock_cpu_init(void)
{
	intel_update_microcode_from_cbfs();
}
