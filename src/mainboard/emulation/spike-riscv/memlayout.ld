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
        STACK(8M, 64K)
	ROMSTAGE(8M + 64K, 128K)
	PRERAM_CBMEM_CONSOLE(8M + 192k, 8K)
	RAMSTAGE(8M + 200K, 256K)
}
