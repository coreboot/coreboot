/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <boot/coreboot_tables.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <ec/google/chromeec/ec_commands.h>
#include <gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

//enum {
//	ACTIVE_LOW = 0,
//	ACTIVE_HIGH = 1
//};

void fill_lb_gpios(struct lb_gpios *gpios)
{
	int count = 0;

	/* Write Protect: active low */
	gpios->gpios[count].port = GPIO(R1);
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = gpio_get(GPIO(R1));
	strncpy((char *)gpios->gpios[count].name, "write protect",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Recovery: active high */
	gpios->gpios[count].port = -1;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = get_recovery_mode_switch();
	strncpy((char *)gpios->gpios[count].name, "recovery",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Lid: active high */
	gpios->gpios[count].port = GPIO(R4);
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "lid", GPIO_MAX_NAME_LENGTH);
	count++;

	/* Power: active low */
	gpios->gpios[count].port = GPIO(Q0);
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "power",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Developer: virtual GPIO active high */
	gpios->gpios[count].port = -1;
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = get_developer_mode_switch();
	strncpy((char *)gpios->gpios[count].name, "developer",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* EC in RW: active high */
	gpios->gpios[count].port = GPIO(U4);
	gpios->gpios[count].polarity = ACTIVE_HIGH;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "EC in RW",
		GPIO_MAX_NAME_LENGTH);
	count++;

	/* Reset: active low (output) */
	gpios->gpios[count].port = GPIO(I5);
	gpios->gpios[count].polarity = ACTIVE_LOW;
	gpios->gpios[count].value = -1;
	strncpy((char *)gpios->gpios[count].name, "reset",
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

	ec_events = google_chromeec_get_events_b();
	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_write_protect_state(void)
{
	return !gpio_get(GPIO(R1));
}
