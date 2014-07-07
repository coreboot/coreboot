/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
 * Copyright (C) 2014 Sage Electronic Engineering, LLC.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <cbmem.h>
#include <device/device.h>
#include <baytrail/baytrail.h>
#include <baytrail/iosf.h>
#include <cpu/x86/msr.h>
#include <drivers/intel/fsp/fsp_util.h>

unsigned long get_top_of_ram(void)
{
	/*
	 * Calculate the top of usable (low) DRAM.
	 * The FSP's reserved memory sits just below the SMM region,
	 * allowing calculation of the top of usable memory.
	 *
	 * The entire memory map is shown in northcluster.c
	 */
	u32 tom = iosf_bunit_read(BUNIT_BMBOUND);
	u32 bsmmrrl = iosf_bunit_read(BUNIT_SMRRL) << 20;
	if (bsmmrrl) {
		tom = bsmmrrl;
	}
	tom -= FSP_RESERVE_MEMORY_SIZE;

	return (unsigned long) tom;
}
