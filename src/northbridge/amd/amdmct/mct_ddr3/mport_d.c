/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
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

#include <inttypes.h>
#include <console/console.h>
#include <string.h>
#include "mct_d.h"
#include "mct_d_gcc.h"
#include "mwlc_d.h"

void AmdMemPCIRead(SBDFO loc, u32 *Value)
{
	/* Convert SBDFO into a CF8 Address */
	loc = (loc >> 4 & 0xFFFFFF00) | (loc & 0xFF) | ((loc & 0xF00) << 16);
	loc |= 0x80000000;

	outl(loc, 0xCF8);

	*Value = inl(0xCFC);
}

void AmdMemPCIWrite(SBDFO loc, u32 *Value)
{
	/* Convert SBDFO into a CF8 Address */
	loc = (loc >> 4 & 0xFFFFFF00) | (loc & 0xFF) | ((loc & 0xF00) << 16);
	loc |= 0x80000000;

	outl(loc, 0xCF8);
	outl(*Value, 0xCFC);
}
