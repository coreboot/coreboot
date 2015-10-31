/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

#include "board.h"

#define GPIO_WP		GPIO(7, A, 6)
#define GPIO_LID	GPIO(0, A, 6)
#define GPIO_POWER	GPIO(0, A, 5)
#define GPIO_RECOVERY	GPIO(0, B, 1)
#define GPIO_ECINRW	GPIO(0, A, 7)
#define GPIO_ECIRQ	GPIO(7, A, 7)

void setup_chromeos_gpios(void)
{
	gpio_input(GPIO_WP);
	gpio_input_pullup(GPIO_LID);
	gpio_input(GPIO_POWER);
	gpio_input_pullup(GPIO_RECOVERY);
	gpio_input(GPIO_ECIRQ);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	int count = 0;

	/* Write Protect: active low */
	gpios->gpios[count].port = GPIO_WP.raw;
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = gpio_get(GPIO_WP);
	strncpy((char *)gpios->gpios[count].name, "write protect",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Recovery: active low */
	gpios->gpios[count].port = GPIO_RECOVERY.raw;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[count].name, "recovery",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Lid: active high */
	gpios->gpios[count].port = GPIO_LID.raw;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "lid", GPIO_MAX_NAME_LENGTH);
	count++;

	/* Power:GPIO active high */
	gpios->gpios[count].port = GPIO_POWER.raw;
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "power",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Developer: GPIO active high */
	gpios->gpios[count].port = -1;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = get_developer_mode_switch();
	strncpy((char *)gpios->gpios[count].name, "developer",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* EC in RW: GPIO active high */
	gpios->gpios[count].port = GPIO_ECINRW.raw;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "EC in RW",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* EC interrupt: GPIO active high */
	gpios->gpios[count].port = GPIO_ECIRQ.raw;
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "EC interrupt",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Reset: GPIO active high (output) */
	gpios->gpios[count].port = GPIO_RESET.raw;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "reset",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Backlight: GPIO active high (output) */
	gpios->gpios[count].port = GPIO_BACKLIGHT.raw;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "backlight",
		GPIO_MAX_NAME_LENGTH);
	count++;

	gpios->size = sizeof(*gpios) + (count * sizeof(struct lb_gpio));
	gpios->count = count;

	printk(BIOS_ERR, "Added %d GPIOS size %d\n", count, gpios->size);
}

int get_developer_mode_switch(void)
{
	return 0;
}

int get_recovery_mode_switch(void)
{
	uint32_t ec_events;

	/* The GPIO is active low. */
	if (!gpio_get(GPIO_RECOVERY))
		return 1;

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO_WP);
}
