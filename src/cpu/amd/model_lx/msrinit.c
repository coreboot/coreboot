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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdlib.h>

struct msrinit {
	u32 msrnum;
	msr_t msr;
};

static const struct msrinit msr_table[] =
{
	{CPU_RCONF_DEFAULT, {.hi = 0x24fffc02,.lo = 0x1000A000}}, /* Setup access to cache under 1MB.
								   * Rom Properties: Write Serialize, WriteProtect.
								   * RomBase: 0xFFFC0
								   * SysTop to RomBase Properties: Write Serialize, Cache Disable.
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
};

static void msr_init(void)
{
	int i;
	for (i = 0; i < ARRAY_SIZE(msr_table); i++)
		wrmsr(msr_table[i].msrnum, msr_table[i].msr);
}


