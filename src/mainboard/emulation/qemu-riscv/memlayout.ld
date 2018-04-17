/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <memlayout.h>

#include <arch/header.ld>

SECTIONS
{
	DRAM_START(0x0)
	BOOTBLOCK(0x0, 64K)
	ROMSTAGE(0x20000, 128K)
	STACK(0x40000, 0x3ff00)
	PRERAM_CBMEM_CONSOLE(0x80000, 8K)
	RAMSTAGE(0x100000, 16M)
}
