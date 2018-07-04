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

#include <string.h>
#include <bootmode.h>
#include <arch/io.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>

#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"
#include <ec/quanta/it8518/ec.h>

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	7

void fill_lb_gpios(struct lb_gpios *gpios)
{
	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO7 */
	gpios->gpios[0].port = 7;
	gpios->gpios[0].polarity = ACTIVE_LOW;
	gpios->gpios[0].value = !get_write_protect_state();
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);

	/* Recovery: Virtual switch */
	gpios->gpios[1].port = -1;
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid Switch: Virtual switch */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = get_lid_switch();
	strncpy((char *)gpios->gpios[3].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button: Virtual switch */
	gpios->gpios[4].port = -1;
	gpios->gpios[4].polarity = ACTIVE_HIGH;
	gpios->gpios[4].value = 0; /* Hard-code to de-asserted */
	strncpy((char *)gpios->gpios[4].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Was VGA Option ROM loaded? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = gfx_get_init_done();
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);

	/* EC is in RW mode when it isn't in recovery mode. */
	gpios->gpios[6].port = -1;
	gpios->gpios[6].polarity = ACTIVE_HIGH;
	gpios->gpios[6].value = !get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[6].name,"ec_in_rw", GPIO_MAX_NAME_LENGTH);
}
#endif

int get_write_protect_state(void)
{
	return !get_gpio(7);
}

int get_lid_switch(void)
{
	/* hard-code to open */
	return 1;
}

/*
 * The recovery-switch is virtual on Stout and is handled via the EC.
 * Stout recovery mode is only valid if RTC_PWR_STS is set and the EC
 * indicated the recovery keys were pressed. We use a global flag for
 * rec_mode to be used after RTC_POWER_STS has been cleared. This function
 * is complicated by romstage support, which can't use a global variable.
 * Note, rec_mode is the only time the EC is in RO mode, otherwise, RW.
 */
int get_recovery_mode_switch(void)
{
#ifdef __PRE_RAM__
	pci_devfn_t dev = PCI_DEV(0, 0x1f, 0);
#else
	static int ec_in_rec_mode = 0;
	static int ec_rec_flag_good = 0;
	struct device *dev = dev_find_slot(0, PCI_DEVFN(0x1f, 0));
#endif

	u8 ec_status = ec_read(EC_STATUS_REG);
	u8 reg8 = pci_read_config8(dev, GEN_PMCON_3);

	printk(BIOS_SPEW,"%s:  EC status:%#x   RTC_BAT: %x\n",
			__func__, ec_status, reg8 & RTC_BATTERY_DEAD);

#ifdef __PRE_RAM__
	return (((reg8 & RTC_BATTERY_DEAD) != 0) &&
	         ((ec_status & 0x3) == EC_IN_RECOVERY_MODE));
#else
	if (!ec_rec_flag_good) {
		ec_in_rec_mode = (((reg8 & RTC_BATTERY_DEAD) != 0) &&
		                     ((ec_status & 0x3) == EC_IN_RECOVERY_MODE));
		ec_rec_flag_good = 1;
	}
	return ec_in_rec_mode;
#endif
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_REC_AH(CROS_GPIO_VIRTUAL, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AL(7, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
