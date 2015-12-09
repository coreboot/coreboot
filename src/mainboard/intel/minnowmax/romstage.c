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
 */

#include <soc/romstage.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <pc80/mc146818rtc.h>
#include <console/console.h>
#include <soc/gpio.h>
#include <soc/intel/fsp_baytrail/chip.h>

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
	u8 gpio5 = 0;

	/*
	 * Minnow Max Board
	 * Read SSUS gpio 5 to determine memory type
	 *                    0 : 1GB SKU uses 2Gb density memory
	 *                    1 : 2GB SKU uses 4Gb density memory
	 *
	 * devicetree.cb assumes 1GB SKU board
	 */
	configure_ssus_gpio(5, PAD_FUNC0 | PAD_PULL_DISABLE, PAD_VAL_INPUT);
	gpio5 = read_ssus_gpio(5);
	if (gpio5)
		UpdData->PcdMemoryParameters.DIMMDensity
		+= (DIMM_DENSITY_4G_BIT - DIMM_DENSITY_2G_BIT);
	printk(BIOS_NOTICE, "%s GB Minnowboard Max detected.\n",
			gpio5 ? "2 / 4" : "1" );
	/* Update XHCI UPD value if required */
	get_option(&use_xhci, "use_xhci_over_ehci");
	if ((use_xhci < 2) && (use_xhci != UpdData->PcdEnableXhci)) {
		UpdData->PcdEnableXhci = use_xhci;
		printk(FSP_INFO_LEVEL, "Xhci updated from CMOS:\t\t\t%s\n",
			UpdData->PcdEnableXhci?"Enabled":"Disabled");
	}

	return;
}
