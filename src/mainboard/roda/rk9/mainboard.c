/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 secunet Security Networks AG
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

#include <stdint.h>
#include <stdlib.h>
#include <arch/io.h>
#include <device/device.h>
#include <console/console.h>
#include <drivers/intel/gma/int15.h>
#include <pc80/keyboard.h>
#include <ec/acpi/ec.h>

static void ec_setup(void)
{
	/* Thermal limits?  Values are from ectool's RAM dump. */
	ec_write(0xd1, 0x57); /* CPUH */
	ec_write(0xd2, 0xc9); /* CPUL */
	ec_write(0xd4, 0x64); /* SYSH */
	ec_write(0xd5, 0xc9); /* SYSL */

	send_ec_command(0x04); /* Set_SMI_Enable */
	send_ec_command(0xab); /* Set_ACPI_Disable */
	send_ec_command(0xac); /* Clr_SYS_Flag? well, why not? */
	send_ec_command(0xad); /* Set_Thml_Value */
}

static void mainboard_enable(struct device *dev)
{
	ec_setup();
	/* LCD panel type is SIO GPIO40-43.
	   It's controlled by a DIP switch but was always
	   set to 4 while only values of 5 and 6 worked. */
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, (inb(0x60f) & 0x0f) + 1);

	/* We have no driver for the embedded controller since the firmware
	   does most of the job. Hence, initialize keyboards here. */
	pc_keyboard_init(NO_AUX_DEVICE);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
