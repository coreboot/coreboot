/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2011 The Chromium OS Authors. All rights reserved.
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

#include <types.h>
#include <cpu/x86/msr.h>
#include "model_206ax.h"

#define IA32_PLATFORM_ID		0x17

int get_platform_id(void)
{
	msr_t msr;

	msr = rdmsr(IA32_PLATFORM_ID);
	/* Read Platform Id Bits 52:50 */
	return (msr.hi >> 18) & 0x7;
}
