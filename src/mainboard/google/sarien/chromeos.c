/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <arch/acpi.h>
#include <arch/early_variables.h>
#include <boot/coreboot_tables.h>
#include <gpio.h>
#include <soc/gpio.h>
#include <variant/gpio.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <security/tpm/tss.h>
#include <device/device.h>
#include <intelblocks/pmclib.h>
#include <soc/pmc.h>
#include <soc/pci_devs.h>

enum rec_mode_state {
	REC_MODE_UNINITIALIZED,
	REC_MODE_NOT_REQUESTED,
	REC_MODE_REQUESTED,
};
static enum rec_mode_state saved_rec_mode CAR_GLOBAL;

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio chromeos_gpios[] = {
		{GPIO_PCH_WP, ACTIVE_HIGH, get_write_protect_state(),
		 "write protect"},
		{GPIO_REC_MODE, ACTIVE_LOW, get_recovery_mode_switch(),
		 "recovery"},
		{-1, ACTIVE_HIGH, get_lid_switch(), "lid"},
		{-1, ACTIVE_HIGH, 0, "power"},
		{-1, ACTIVE_HIGH, gfx_get_init_done(), "oprom"},
		{-1, ACTIVE_HIGH, 0, "EC in RW"},
	};
	lb_add_gpios(gpios, chromeos_gpios, ARRAY_SIZE(chromeos_gpios));
}

static int cros_get_gpio_value(int type)
{
	const struct cros_gpio *cros_gpios;
	size_t i, num_gpios = 0;

	cros_gpios = variant_cros_gpios(&num_gpios);

	for (i = 0; i < num_gpios; i++) {
		const struct cros_gpio *gpio = &cros_gpios[i];
		if (gpio->type == type) {
			int state = gpio_get(gpio->gpio_num);
			if (gpio->polarity == CROS_GPIO_ACTIVE_LOW)
				return !state;
			else
				return state;
		}
	}
	return 0;
}

void mainboard_chromeos_acpi_generate(void)
{
	const struct cros_gpio *cros_gpios;
	size_t num_gpios = 0;

	cros_gpios = variant_cros_gpios(&num_gpios);

	chromeos_acpi_gpio_generate(cros_gpios, num_gpios);
}

int get_write_protect_state(void)
{
	return cros_get_gpio_value(CROS_GPIO_WP);
}

int get_recovery_mode_switch(void)
{
	enum rec_mode_state state = car_get_var(saved_rec_mode);
	uint8_t recovery_button_state = 0;

	/* Check the global variable first. */
	if (state == REC_MODE_NOT_REQUESTED)
		return 0;
	else if (state == REC_MODE_REQUESTED)
		return 1;

	state = REC_MODE_NOT_REQUESTED;

	/* Read state from the GPIO controlled by servo. */
	if (cros_get_gpio_value(CROS_GPIO_REC))
		state = REC_MODE_REQUESTED;
	/* Read one-time recovery request from cr50. */
	else if (tlcl_cr50_get_recovery_button(&recovery_button_state)
		 == TPM_SUCCESS)
		state = recovery_button_state ?
			REC_MODE_REQUESTED : REC_MODE_NOT_REQUESTED;

	/* Store the state in case this is called again in verstage. */
	car_set_var(saved_rec_mode, state);

	return state == REC_MODE_REQUESTED;
}

int get_lid_switch(void)
{
	return 1;
}

void mainboard_cr50_update_reset(void)
{
#if ENV_RAMSTAGE
	/* Ensure system powers up after CR50 reset */
	pmc_set_afterg3(PCH_DEV_PMC, MAINBOARD_POWER_STATE_ON);
#endif
}
