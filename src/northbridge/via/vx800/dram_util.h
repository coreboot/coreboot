/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 One Laptop per Child, Association, Inc.
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

#ifndef __DRAM_UTIL_H__
#define __DRAM_UTIL_H__

#define STEPSPAN	0x1000	//the span when test memory in spare mode
#define TESTCOUNT	0x4	// the test count in each range when test memory in spare mode
#define TEST_PATTERN	0x5A5A5A5A	//the test pattern

typedef enum __DRAM_TEST_MODE {
	EXTENSIVE,
	SPARE,
	MAXMODE
} DRAM_TEST_MODE;

void WaitMicroSec(UINTN MicroSeconds);

void via_write_phys(u32 addr, u32 value);

u32 via_read_phys(u32 addr);

u32 DimmRead(u32 x);

BOOLEAN DramBaseTest(u32 BaseAdd, u32 Length,
		     DRAM_TEST_MODE mode, BOOLEAN PrintFlag);

void DumpRegisters(INTN DevNum, INTN FuncNum);

void dumpnorth(u8 Func);
#endif
