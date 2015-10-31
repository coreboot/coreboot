/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The Chromium OS Authors. All rights reserved.
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

#ifndef __ARCH_EBDA_H
#define __ARCH_EBDA_H

#define X86_BDA_SIZE		0x200
#define X86_BDA_BASE		(void *)0x400
#define X86_EBDA_SEGMENT	(void *)0x40e
#define X86_EBDA_LOWMEM		(void *)0x413

#define DEFAULT_EBDA_LOWMEM	(1024 << 10)
#define DEFAULT_EBDA_SEGMENT	0xF600
#define DEFAULT_EBDA_SIZE	0x400

void setup_ebda(u32 low_memory_size, u16 ebda_segment, u16 ebda_size);
void setup_default_ebda(void);

#endif
