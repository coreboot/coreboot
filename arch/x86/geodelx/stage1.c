/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2006 Indrek Kruusa <indrek.kruusa@artecdesign.ee>
 * Copyright (C) 2006 Ronald G. Minnich <rminnich@gmail.com>
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
#include <console.h>
#include <device/device.h>
#include <device/pci.h>
#include <string.h>
#include <msr.h>
#include <io.h>
#include <amd_geodelx.h>
#include <spd.h>

/**
 * Set up Geode LX registers for sane behaviour.
 *
 * Set all low memory (under 1MB) to write back. Do some setup for Cache
 * as Ram (CAR) as well.
 */
void geodelx_msr_init(void)
{
	struct msr msr;

	/* Setup access to the cache for under 1MB. */
	msr.hi = 0x24fffc02;
	msr.lo = 0x1000A000;	/* 0-A0000 write back */
	wrmsr(CPU_RCONF_DEFAULT, msr);

	msr.hi = 0x0;		/* write back */
	msr.lo = 0x0;
	wrmsr(CPU_RCONF_A0_BF, msr);
	wrmsr(CPU_RCONF_C0_DF, msr);
	wrmsr(CPU_RCONF_E0_FF, msr);

	/* Setup access to the cache for under 640K. */
	/* Note: Memory controller not setup yet. */
	msr.hi = 0x20000000;
	msr.lo = 0x000fff80;	/* 0-0x7FFFF */
	wrmsr(MSR_GLIU0_BASE1, msr);

	msr.hi = 0x20000000;
	msr.lo = 0x080fffe0;	/* 0x80000-0x9FFFF */
	wrmsr(MSR_GLIU0_BASE2, msr);

	msr.hi = 0x20000000;
	msr.lo = 0x000fff80;	/* 0-0x7FFFF */
	wrmsr(MSR_GLIU1_BASE1, msr);

	msr.hi = 0x20000000;
	msr.lo = 0x080fffe0;	/* 0x80000-0x9FFFF */
	wrmsr(MSR_GLIU0_BASE2, msr);
}
