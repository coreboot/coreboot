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
#include <mainboard/addressmap.h>

//Stages start after CBFS in DRAM
#define STAGES_START (QEMU_VIRT_DRAM + CONFIG_ROM_SIZE)

SECTIONS
{
	DRAM_START(QEMU_VIRT_DRAM)
	BOOTBLOCK(QEMU_VIRT_DRAM, 64K)
	// CBFS goes here
	STACK(STAGES_START, 4K)
	ROMSTAGE(STAGES_START + 64K, 128K)
	PRERAM_CBMEM_CONSOLE(STAGES_START + 192K, 8K)
	RAMSTAGE(STAGES_START + 200K, 16M)
}
