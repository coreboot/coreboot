/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <types.h>
#include <lib.h>
#include <msr.h>
#include <amd_geodelx.h>

static const struct msrinit {
	u32 msrnum;
	struct msr msr;
} msr_table[] = {
  /* Setup access to cache under 1MB. */
  {CPU_RCONF_DEFAULT, {.hi = 0x24fffc02,.lo = 0x1000A000}}, // 0x00000-0xA0000
  {CPU_RCONF_A0_BF,   {.hi = 0x00000000,.lo = 0x00000000}}, // 0xA0000-0xBFFFF
  {CPU_RCONF_C0_DF,   {.hi = 0x00000000,.lo = 0x00000000}}, // 0xC0000-0xDFFFF
  {CPU_RCONF_E0_FF,   {.hi = 0x00000000,.lo = 0x00000000}}, // 0xE0000-0xFFFFF

  /* Setup access to memory under 1MB. Note: VGA hole at 0xA0000-0xBFFFF */
  {MSR_GLIU0_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
  {MSR_GLIU0_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
  {MSR_GLIU0_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF
  {MSR_GLIU1_BASE1,   {.hi = 0x20000000,.lo = 0x000fff80}}, // 0x00000-0x7FFFF
  {MSR_GLIU1_BASE2,   {.hi = 0x20000000,.lo = 0x080fffe0}}, // 0x80000-0x9FFFF
  {MSR_GLIU1_SHADOW,  {.hi = 0x2000FFFF,.lo = 0xFFFF0003}}, // 0xC0000-0xFFFFF
};

/**
  * Set up Geode LX registers for sane behaviour.
  */
void geodelx_msr_init(void)
{
}

/**
  * Disable Cache As RAM(CAR) after memory is setup.
  *  Geode can write back the cache contents to RAM and continue on.
  *  Assumes that anything in the cache was under 1MB.
  */
void disable_car(void)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(msr_table); i++)
		wrmsr(msr_table[i].msrnum, msr_table[i].msr);

	__asm__("wbinvd\n");
}
