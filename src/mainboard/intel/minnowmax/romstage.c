/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2013 Sage Electronic Engineering, LLC.
 * Copyright (C) 2014 Intel Corporation
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

#include <baytrail/romstage.h>
#include <drivers/intel/fsp/fsp_util.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include "chip.h"

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry()
{

}

/**
 * Get function disables - most of these will be done automatically
 * @param fd_mask
 * @param fd2_mask
 */
void get_func_disables(uint32_t *fd_mask, uint32_t *fd2_mask)
{

}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry()
{

}

void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	UPD_DATA_REGION *UpdData = FspRtBuffer->Common.UpdDataRgnPtr;
	u8 use_xhci = UpdData->PcdEnableXhci;

	/*
	 * Minnow Max Board	: 1GB SKU uses 2Gb density memory
	 *			  2GB SKU uses 4Gb densiry memory
	 *
	 * devicetree.cb assume 1GB SKU board
	*/
	if (CONFIG_MINNOWMAX_2GB_SKU)
		UpdData->PcdMemoryParameters.DIMMDensity
		+= (DIMM_DENSITY_4G_BIT - DIMM_DENSITY_2G_BIT);

	/* Update XHCI UPD value if required */
	get_option(&use_xhci, "use_xhci_over_ehci");
	if ((use_xhci < 2) && (use_xhci != UpdData->PcdEnableXhci)) {
		UpdData->PcdEnableXhci = use_xhci;
		printk(FSP_INFO_LEVEL, "Xhci updated from CMOS:\t\t\t%s\n",
			UpdData->PcdEnableXhci?"Enabled":"Disabled");
	}

	return;
}
