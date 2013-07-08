/*
 * This file is part of the coreboot project.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <stdlib.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/lxdef.h>

/**************************************************************************
 *
 *	pcideadlock
 *
 *	Bugtool #465 and #609
 *	PCI cache deadlock
 *	There is also fix code in cache and PCI functions. This bug is very is pervasive.
 *
 **************************************************************************/
static void pcideadlock(void)
{
	msr_t msr;

	/*
	 * forces serialization of all load misses. Setting this bit prevents the
	 * DM pipe from backing up if a read request has to be held up waiting
	 * for PCI writes to complete.
	 */
	msr = rdmsr(CPU_DM_CONFIG0);
	msr.lo |= DM_CONFIG0_LOWER_MISSER_SET;
	wrmsr(CPU_DM_CONFIG0, msr);

	/* write serialize memory hole to PCI. Need to unWS when something is
	 * shadowed regardless of cachablility.
	 */
	msr.lo = 0x021212121;
	msr.hi = 0x021212121;
	wrmsr(CPU_RCONF_A0_BF, msr);
	wrmsr(CPU_RCONF_C0_DF, msr);
	wrmsr(CPU_RCONF_E0_FF, msr);
}

/****************************************************************************/
/***/
/**	DisableMemoryReorder*/
/***/
/**	PBZ 3659:*/
/**	 The MC reordered transactions incorrectly and breaks coherency.*/
/**	 Disable reordering and take a potential performance hit.*/
/**	 This is safe to do here and not in MC init since there is nothing*/
/**	 to maintain coherency with and the cache is not enabled yet.*/
/***/
/****************************************************************************/
static void disablememoryreadorder(void)
{
	msr_t msr;

	msr = rdmsr(MC_CF8F_DATA);
	msr.hi |= CF8F_UPPER_REORDER_DIS_SET;
	wrmsr(MC_CF8F_DATA, msr);
}

/* For cpu version C3. Should be the only released version */
void cpubug(void)
{
	pcideadlock();
	disablememoryreadorder();
	printk(BIOS_DEBUG, "Done cpubug fixes \n");
}
