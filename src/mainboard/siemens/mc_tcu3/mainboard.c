/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 * Copyright (C) 2011 The ChromiumOS Authors.  All rights reserved.
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

#include <types.h>
#include <string.h>
#include <device/device.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <console/console.h>
#if IS_ENABLED(CONFIG_VGA_ROM_RUN)
#include <x86emu/x86emu.h>
#endif
#include <pc80/mc146818rtc.h>
#include <arch/acpi.h>
#include <arch/io.h>
#include <arch/interrupt.h>
#include <boot/coreboot_tables.h>
#include <hwilib.h>
#include <i210.h>
#include "lcd_panel.h"


/** \brief This function will search for a MAC address which can be assigned
 *         to a MACPHY.
 * @param  dev     pointer to PCI device
 * @param  mac     buffer where to store the MAC address
 * @return cb_err  CB_ERR or CB_SUCCESS
 */
enum cb_err mainboard_get_mac_address(struct device *dev, uint8_t mac[6])
{
	uint8_t mac_adr[6];
	uint32_t i;

	/* Open main hwinfo block */
	if (hwilib_find_blocks("hwinfo.hex") != CB_SUCCESS)
		return CB_ERR;
	/* Get first MAC address from hwinfo. */
	if (hwilib_get_field(Mac1, mac_adr, sizeof(mac_adr)) != sizeof(mac_adr))
		return CB_ERR;
	/* Ensure the first MAC-Address is not completely 0x00 or 0xff */
	for (i = 0; i < 6; i++) {
		if (mac_adr[i] != 0xFF)
			break;
	}
	if (i == 6){
		return CB_ERR;
	}
	for (i = 0; i < 6; i++) {
		if (mac_adr[i] != 0x00)
			break;
	}
	if (i == 6){
		return CB_ERR;
	} else {
		memcpy(mac, mac_adr, 6);
		return CB_SUCCESS;
	}
}

/*
 * mainboard_enable is executed as first thing after enumerate_buses().
 * This is the earliest point to add customization.
 */
static void mainboard_enable(struct device *dev)
{
	setup_lcd_panel();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
