/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Lance Zhao <lijian.zhao@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include "ec.h"

#include <ec/google/chromeec/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

int get_lid_switch(void)
{
	/* Default to force open */
	return 1;
}

/* The dev-switch is virtual */
int get_developer_mode_switch(void)
{
	return 0;
}

int get_recovery_mode_switch(void)
{
	uint32_t ec_events;

	/* Recovery mode via ESC + Refresh + PWR ? */
	ec_events = google_chromeec_get_events_b();
	if (ec_events & EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY))
		return 1;

	return 0;
}

int get_write_protect_state(void)
{
	return 0;
}
