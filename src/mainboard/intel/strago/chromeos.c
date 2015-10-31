/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
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

#include <arch/io.h>
#include <device/device.h>
#include <device/pci.h>

#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
#include "ec.h"
#include <ec/google/chromeec/ec.h>
#endif
#include <rules.h>
#include <soc/gpio.h>
#include <string.h>
#include <vendorcode/google/chromeos/chromeos.h>

/* The WP status pin lives on GPIO_SSUS_6 which is pad 36 in the SUS well. */
#define WP_STATUS_PAD	36

#if ENV_RAMSTAGE
#include <boot/coreboot_tables.h>

#define GPIO_COUNT	6
#define ACTIVE_LOW	0
#define ACTIVE_HIGH	1

void fill_lb_gpios(struct lb_gpios *gpios)
{
	struct lb_gpio *gpio;

	gpios->size = sizeof(*gpios) + (GPIO_COUNT * sizeof(struct lb_gpio));
	gpios->count = GPIO_COUNT;

	gpio = gpios->gpios;
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "write protect",
		     get_write_protect_state());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "recovery",
		     recovery_mode_enabled());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "developer",
		     get_developer_mode_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "lid", get_lid_switch());
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "power", 0);
	fill_lb_gpio(gpio++, -1, ACTIVE_HIGH, "oprom", gfx_get_init_done());
}
#endif /* ENV_RAMSTAGE */

int get_lid_switch(void)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	u8 ec_switches;

	mec_io_bytes(0, EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES, 1,
		     &ec_switches, NULL);

	return !!(ec_switches & EC_SWITCH_LID_OPEN);
#else
	/* Default to force open. */
	return 1;
#endif
}

int get_developer_mode_switch(void)
{
	return 0;
}

int get_recovery_mode_switch(void)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	u8 ec_switches;
	u32 ec_events;
	mec_io_bytes(0, EC_LPC_ADDR_MEMMAP + EC_MEMMAP_SWITCHES, 1,
		     &ec_switches, NULL);

	/* If a switch is set, we don't need to look at events. */
	if (ec_switches & (EC_SWITCH_DEDICATED_RECOVERY))
		return 1;

	/* Else check if the EC has posted the keyboard recovery event. */
	ec_events = google_chromeec_get_events_b();

	return !!(ec_events &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
#else
	return 0;
#endif
}

int clear_recovery_mode_switch(void)
{
#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC)
	const uint32_t kb_rec_mask =
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY);
	/* Unconditionally clear the EC recovery request. */
	return google_chromeec_clear_events_b(kb_rec_mask);
#else
	return 0;
#endif
}

int get_write_protect_state(void)
{
	/*
	 * The vboot loader queries this function in romstage. The GPIOs have
	 * not been set up yet as that configuration is done in ramstage. The
	 * hardware defaults to an input but there is a 20K pulldown. Externally
	 * there is a 10K pullup. Disable the internal pull in romstage so that
	 * there isn't any ambiguity in the reading.
	 */
#if ENV_ROMSTAGE
	ssus_disable_internal_pull(WP_STATUS_PAD);
#endif

	/* WP is enabled when the pin is reading high. */
	return ssus_get_gpio(WP_STATUS_PAD);
}
