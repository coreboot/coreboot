/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <boardid.h>
#include <bootmode.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <string.h>

#include "gpio.h"

void setup_chromeos_gpios(void)
{
	gpio_input(WRITE_PROTECT);
	gpio_input_pullup(EC_IN_RW);
	gpio_input_pullup(EC_IRQ);
	gpio_input(LID);
	gpio_input_pullup(POWER_BUTTON);
	if (board_id() + CONFIG_BOARD_ID_ADJUSTMENT < 5)
		gpio_output(EC_SUSPEND_L, 1);
}

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{WRITE_PROTECT.id, ACTIVE_LOW,
			gpio_get(WRITE_PROTECT), "write protect"},
		{-1, ACTIVE_HIGH, get_recovery_mode_switch(), "recovery"},
		{LID.id, ACTIVE_HIGH, -1, "lid"},
		{POWER_BUTTON.id, ACTIVE_HIGH, -1, "power"},
		{EC_IN_RW.id, ACTIVE_HIGH, -1, "EC in RW"},
		{EC_IRQ.id, ACTIVE_LOW, -1, "EC interrupt"},
		{CR50_IRQ.id, ACTIVE_HIGH, -1, "TPM interrupt"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

int get_write_protect_state(void)
{
	return !gpio_get(WRITE_PROTECT);
}
