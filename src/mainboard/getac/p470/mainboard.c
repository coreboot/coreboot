/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <device/pci_ops.h>
#include <drivers/intel/gma/int15.h>


#include "ec_oem.c"

#include "mainboard.h"

#define MAX_LCD_BRIGHTNESS	0xd8

static void ec_enable(void)
{
	u16 keymap;
	/* Enable Hotkey SCI */

	/* Fn key map; F1 = [0] ... F12 = [11] */
	keymap = 0x5f1;
	send_ec_oem_command(0x45);
	send_ec_oem_data(0x09); // SCI
	// send_ec_oem_data(0x08); // SMI#
	send_ec_oem_data(keymap >> 8);
	send_ec_oem_data(keymap & 0xff);

	/* Enable Backlight */
	ec_write(0x17, MAX_LCD_BRIGHTNESS);

	/* Notify EC system is in ACPI mode */
	send_ec_oem_command(0x5e);
	send_ec_oem_data(0xea);
	send_ec_oem_data(0x0c);
	send_ec_oem_data(0x01);
}

static void mainboard_init(struct device *dev)
{
	ec_enable();
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_TXT_STRETCH, 0, 3);
}

// mainboard_enable is executed as first thing after
// enumerate_buses(). Is there no mainboard_init()?
static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->write_acpi_tables = mainboard_write_acpi_tables;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
