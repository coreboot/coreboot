/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Google Inc.
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

#include <bootmode.h>
#include <cbmem.h>
#include <ec/google/chromeec/ec.h>

#if IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC_LPC)
int get_lid_switch(void)
{
	if (!IS_ENABLED(CONFIG_LID_SWITCH))
		return -1;

	return !!(google_chromeec_get_switches() & EC_SWITCH_LID_OPEN);
}
#endif

int get_recovery_mode_switch(void)
{
	/* Check for dedicated recovery switch first. */
	if (IS_ENABLED(CONFIG_EC_GOOGLE_CHROMEEC_LPC) &&
	    (google_chromeec_get_switches() & EC_SWITCH_DEDICATED_RECOVERY))
		return 1;

	/* Check if the EC has posted the keyboard recovery/fastboot event. */
	return !!(google_chromeec_get_events_b() &
		  (EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY) |
		   EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_FASTBOOT)));
}

int get_recovery_mode_retrain_switch(void)
{
	uint32_t events;
	const uint32_t mask =
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT);

	/*
	 * Check if the EC has posted the keyboard recovery event with memory
	 * retrain.
	 */
	events = google_chromeec_get_events_b();

	if (cbmem_possibly_online()) {
		const uint32_t *events_save;

		events_save = cbmem_find(CBMEM_ID_EC_HOSTEVENT);
		if (events_save != NULL)
			events |= *events_save;
	}

	return !!(events & mask);
}

int clear_recovery_mode_switch(void)
{
	/* Clear all host event bits requesting recovery mode. */
	return google_chromeec_clear_events_b(
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_FASTBOOT));
}
