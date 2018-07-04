/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011-2012 The ChromiumOS Authors.  All rights reserved.
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

#include <console/console.h>
#include <string.h>
#include <bootmode.h>
#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>

#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <ec/compal/ene932/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"


#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	u16 gpio_base = pci_read_config32(dev, GPIOBASE) & 0xfffe;
	u16 gen_pmcon_1 = pci_read_config32(dev, GEN_PMCON_1);

	if (!gpio_base)
		return;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO70 active high */
	gpios->gpios[0].port = 70;
	gpios->gpios[0].polarity = ACTIVE_LOW;
	gpios->gpios[0].value = !get_write_protect_state();
	strncpy((char *)gpios->gpios[0].name,"write protect", GPIO_MAX_NAME_LENGTH);

	/* Recovery: Virtual GPIO in the EC (Servo GPIO68 active low) */
	gpios->gpios[1].port = -1;
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid switch GPIO active high (open). */
	gpios->gpios[3].port = 15;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = get_lid_switch();
	strncpy((char *)gpios->gpios[3].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button */
	gpios->gpios[4].port = 101;
	gpios->gpios[4].polarity = ACTIVE_LOW;
	gpios->gpios[4].value = (gen_pmcon_1 >> 9) & 1;
	strncpy((char *)gpios->gpios[4].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Did we load the VGA Option ROM? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = gfx_get_init_done();
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);

}
#endif

int get_lid_switch(void)
{
	return get_gpio(15);
}

int get_write_protect_state(void)
{
	return !get_gpio(70);
}

int get_recovery_mode_switch(void)
{
	u8 gpio = !get_gpio(68);
	/* GPIO68, active low. For Servo support
	 * Treat as active high and let the caller invert if needed. */
	printk(BIOS_DEBUG, "REC MODE GPIO 68: %x\n", gpio);

	return gpio;
}

int parrot_ec_running_ro(void)
{
	return !get_gpio(68);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_DEV_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(70, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
