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
#include <ec/quanta/ene_kb3940q/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"

#define WP_GPIO		6
#define DEVMODE_GPIO	54
#define FORCE_RECOVERY_MODE	0

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
	u16 gpio_base = pci_read_config16(dev, GPIOBASE) & 0xfffe;

	int lidswitch = 0;
	if (!gpio_base)
		return;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO active Low */
	gpios->gpios[0].port = WP_GPIO;
	gpios->gpios[0].polarity = ACTIVE_LOW;
	gpios->gpios[0].value = !get_write_protect_state();
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: virtual GPIO active high */
	gpios->gpios[1].port = -1;
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* lid switch value from EC */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = get_lid_switch();
	strncpy((char *)gpios->gpios[3].name,"lid", GPIO_MAX_NAME_LENGTH);
	printk(BIOS_DEBUG,"LID SWITCH FROM EC: %x\n", lidswitch);

	/* Power Button - Hardcode Low as power button may still be pressed
	   when read here.*/
	gpios->gpios[4].port = -1;
	gpios->gpios[4].polarity = ACTIVE_HIGH;
	gpios->gpios[4].value = 0;
	strncpy((char *)gpios->gpios[4].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Was VGA Option ROM loaded? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = gfx_get_init_done();
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);

}
#endif

int get_write_protect_state(void)
{
	return !get_gpio(WP_GPIO);
}

int get_lid_switch(void)
{
	return (ec_mem_read(EC_HW_GPI_STATUS) >> EC_GPI_LID_STAT_BIT) & 1;
}

int get_recovery_mode_switch(void)
{
	int ec_rec_mode = 0;

#if FORCE_RECOVERY_MODE
	printk(BIOS_DEBUG,"FORCING RECOVERY MODE.\n");
	return 1;
#endif


#ifndef __PRE_RAM__
	if (ec_mem_read(EC_CODE_STATE) == EC_COS_EC_RO) {
		ec_rec_mode = 1;
	}
	printk(BIOS_DEBUG,"RECOVERY MODE FROM EC: %x\n", ec_rec_mode);
#endif

	return ec_rec_mode;
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_DEV_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(WP_GPIO, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
