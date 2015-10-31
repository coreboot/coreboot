/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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
#include <device/device.h>
#include <device/pci.h>
#include <soc/gpio.h>

/* Compile-time settings for developer and recovery mode. */
#define DEV_MODE_SETTING 1
#define REC_MODE_SETTING 0

#ifndef __PRE_RAM__
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "write protect", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "recovery", REC_MODE_SETTING);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "developer", DEV_MODE_SETTING);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "lid", 1); // force open
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "power", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "oprom", gfx_get_init_done());
}
#endif

int get_developer_mode_switch(void)
{
	return DEV_MODE_SETTING;
}

int get_recovery_mode_switch(void)
{
	return REC_MODE_SETTING;
}

int get_write_protect_state(void)
{
	return 0;
}
