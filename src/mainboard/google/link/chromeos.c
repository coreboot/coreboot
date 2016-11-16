/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Google Inc.
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
#include <southbridge/intel/bd82x6x/pch.h>
#include <southbridge/intel/common/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	/* Write Protect: GPIO57 = PCH_SPI_WP_D */
	gpios->gpios[0].port = 57;
	gpios->gpios[0].polarity = ACTIVE_HIGH;
	gpios->gpios[0].value = get_write_protect_state();
	strncpy((char *)gpios->gpios[0].name,"write protect",
							GPIO_MAX_NAME_LENGTH);
	/* Recovery: the "switch" comes from the EC */
	gpios->gpios[1].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[1].polarity = ACTIVE_HIGH;
	gpios->gpios[1].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[1].name,"recovery", GPIO_MAX_NAME_LENGTH);

	/* Lid: the "switch" comes from the EC */
	gpios->gpios[2].port = -1;
	gpios->gpios[2].polarity = ACTIVE_HIGH;
	gpios->gpios[2].value = get_lid_switch();
	strncpy((char *)gpios->gpios[2].name,"lid", GPIO_MAX_NAME_LENGTH);

	/* Power Button: hard-coded as not pressed; we'll detect later presses
	 * via SMI. */
	gpios->gpios[3].port = -1;
	gpios->gpios[3].polarity = ACTIVE_HIGH;
	gpios->gpios[3].value = 0;
	strncpy((char *)gpios->gpios[3].name,"power", GPIO_MAX_NAME_LENGTH);

	/* Developer: a tricky case on Link, there is no switch */
	gpios->gpios[4].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[4].polarity = ACTIVE_HIGH;
	gpios->gpios[4].value = get_developer_mode_switch();
	strncpy((char *)gpios->gpios[4].name,"developer", GPIO_MAX_NAME_LENGTH);

	/* Did we load the VGA Option ROM? */
	gpios->gpios[5].port = -1; /* Indicate that this is a pseudo GPIO */
	gpios->gpios[5].polarity = ACTIVE_HIGH;
	gpios->gpios[5].value = gfx_get_init_done();
	strncpy((char *)gpios->gpios[5].name,"oprom", GPIO_MAX_NAME_LENGTH);
}
#endif

int get_write_protect_state(void)
{
	return get_gpio(57);
}

static const struct cros_gpio cros_gpios[] = {
	CROS_GPIO_REC_AL(9, CROS_GPIO_DEVICE_NAME),
	CROS_GPIO_WP_AH(57, CROS_GPIO_DEVICE_NAME),
};

void mainboard_chromeos_acpi_generate(void)
{
	chromeos_acpi_gpio_generate(cros_gpios, ARRAY_SIZE(cros_gpios));
}
