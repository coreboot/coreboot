/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 coresystems GmbH
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

#include <stdlib.h>
#include <cpu/amd/lxdef.h>
#include <cpu/x86/msr.h>
#include <northbridge/amd/lx/northbridge.h>

static const msrinit_t msr_table[] =
{
	{CPU_RCONF_DEFAULT, {.hi = 0x24fffc00,.lo = 0x0000A000}}, /* Setup access to cache under 1MB.
								   * Rom Properties: Write Serialize, WriteProtect.
								   * RomBase: 0xFFFC0
								   * SysTop to RomBase Properties: Write Back.
								   * SysTop: 0x000A0
								   * System Memory Properties:  (Write Back) */
	{CPU_RCONF_A0_BF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xA0000-0xBFFFF : (Write Back) */
	{CPU_RCONF_C0_DF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xC0000-0xDFFFF : (Write Back) */
	{CPU_RCONF_E0_FF,   {.hi = 0x00000000,.lo = 0x00000000}}, /* 0xE0000-0xFFFFF : (Write Back) */

	/* Setup access to memory under 1MB. Note: VGA hole at 0xA0000-0xBFFFF */
	{MSR_GLIU0_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
	{MSR_GLIU0_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
	{MSR_GLIU0_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF
	{MSR_GLIU1_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
	{MSR_GLIU1_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
	{MSR_GLIU1_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF

	/* Pre-setup access to memory above 1Mb. Here we set up about 500Mb of memory.
	 * It doesn't really matter in fact how much, however, because the only usage
	 * of this extended memory will be to host the ramstage stage at RAMBASE,
	 * currently 1Mb.
	 * These registers will be set to their correct value by the Northbridge init code.
	 *
	 * WARNING: if ramstage could not be loaded, these registers are probably
	 * incorrectly set here. You may comment the following two lines and set RAMBASE
	 * to 0x4000 to revert to the previous behavior for LX-boards.
	 */
	{MSR_GLIU0_SYSMEM,  {.hi = 0x2000001F,.lo = 0x6BF00100}}, // 0x100000-0x1F6BF000
	{MSR_GLIU1_SYSMEM,  {.hi = 0x2000001F,.lo = 0x6BF00100}}, // 0x100000-0x1F6BF000
};

void lx_msr_init(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(msr_table); i++)
		wrmsr(msr_table[i].index, msr_table[i].msr);
}
