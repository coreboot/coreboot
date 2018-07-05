/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <southbridge/intel/i82801gx/nvs.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <ec/acpi/ec.h>
#include "dock.h"
#include "smi.h"

#define GPE_EC_SCI	12

#define LVTMA_BL_MOD_LEVEL 0x7af9 /* ATI Radeon backlight level */

static void mainboard_smm_init(void)
{
	printk(BIOS_DEBUG, "initializing SMI\n");
	/* Enable 0x1600/0x1600 register pair */
	ec_set_bit(0x00, 0x05);
}

static void mainboard_smi_brightness_down(void)
{
	u8 *bar;
	if ((bar = (u8 *)pci_read_config32(PCI_DEV(1, 0, 0), 0x18))) {
		printk(BIOS_DEBUG, "bar: %08X, level %02X\n",  (unsigned int)bar,
			*(bar+LVTMA_BL_MOD_LEVEL));
		*(bar+LVTMA_BL_MOD_LEVEL) &= 0xf0;
		if (*(bar+LVTMA_BL_MOD_LEVEL) > 0x10)
			*(bar+LVTMA_BL_MOD_LEVEL) -= 0x10;
	}
}

static void mainboard_smi_brightness_up(void)
{
	u8 *bar;
	if ((bar = (u8 *)pci_read_config32(PCI_DEV(1, 0, 0), 0x18))) {
		printk(BIOS_DEBUG, "bar: %08X, level %02X\n",  (unsigned int )bar,
			*(bar+LVTMA_BL_MOD_LEVEL));
		*(bar+LVTMA_BL_MOD_LEVEL) |= 0x0f;
		if (*(bar+LVTMA_BL_MOD_LEVEL) < 0xf0)
			*(bar+LVTMA_BL_MOD_LEVEL) += 0x10;
	}
}

int mainboard_io_trap_handler(int smif)
{
	static int smm_initialized;

	if (!smm_initialized) {
		mainboard_smm_init();
		smm_initialized = 1;
	}

	switch (smif) {
	case SMI_DOCK_CONNECT:
		/* If there's an legacy I/O module present, we're not
		 * allowed to connect the Docking LPC Bus, as both Super I/O
		 * chips are using 0x2e as base address.
		 */
		if (legacy_io_present())
			break;

		if (!dock_connect()) {
			/* set dock LED to indicate status */
			ec_write(0x0c, 0x08);
			ec_write(0x0c, 0x89);
		} else {
			/* blink dock LED to indicate failure */
			ec_write(0x0c, 0xc8);
			ec_write(0x0c, 0x09);
		}
		break;

	case SMI_DOCK_DISCONNECT:
		dock_disconnect();
		ec_write(0x0c, 0x09);
		ec_write(0x0c, 0x08);
		break;

	case SMI_BRIGHTNESS_UP:
		mainboard_smi_brightness_up();
		break;

	case SMI_BRIGHTNESS_DOWN:
		mainboard_smi_brightness_down();
		break;

	default:
		return 0;
	}

	/* On success, the IO Trap Handler returns 1
	 * On failure, the IO Trap Handler returns a value != 1 */
	return 1;
}

static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %02x\n", event);

	switch(event) {
		/* brightness up */
		case 0x14:
			mainboard_smi_brightness_up();
			break;
		/* brightness down */
		case 0x15:
			mainboard_smi_brightness_down();
			break;
		/* Fn-F9 Key */
		case 0x18:
		/* power loss */
		case 0x27:
		/* undock event */
		case 0x50:
			mainboard_io_trap_handler(SMI_DOCK_DISCONNECT);
			break;
		/* dock event */
		case 0x37:
			mainboard_io_trap_handler(SMI_DOCK_CONNECT);
			break;
		default:
			break;
	}
}

void mainboard_smi_gpi(u32 gpi)
{
	if (gpi & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

int mainboard_smi_apmc(u8 data)
{
	switch(data) {
		case APM_CNT_ACPI_ENABLE:
			/* use 0x1600/0x1604 to prevent races with userspace */
			ec_set_ports(0x1604, 0x1600);
			/* route H8SCI to SCI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
			break;
		case APM_CNT_ACPI_DISABLE:
			/* we have to use port 0x62/0x66, as 0x1600/0x1604 doesn't
			   provide a EC query function */
			ec_set_ports(0x66, 0x62);
			/* route H8SCI# to SMI */
			gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
			break;
		default:
			break;
	}
	return 0;
}
