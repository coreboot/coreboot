/*
 * This file is part of the coreboot project.
 *
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
#include <ec/google/chromeec/ec.h>
#include <elog.h>

#if CONFIG(EC_GOOGLE_CHROMEEC_LPC)
int get_lid_switch(void)
{
	if (!CONFIG(VBOOT_LID_SWITCH))
		return -1;

	return !!(google_chromeec_get_switches() & EC_SWITCH_LID_OPEN);
}
#endif

int get_recovery_mode_switch(void)
{
	/* Check for dedicated recovery switch first. */
	if (CONFIG(EC_GOOGLE_CHROMEEC_LPC) &&
	    (google_chromeec_get_switches() & EC_SWITCH_DEDICATED_RECOVERY))
		return 1;

	/* Check if the EC has posted the keyboard recovery event. */
	return !!(google_chromeec_get_events_b() &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY));
}

int get_recovery_mode_retrain_switch(void)
{
	/*
	 * Check if the EC has posted the keyboard recovery event with memory
	 * retrain.
	 */
	return !!(google_chromeec_get_events_b() &
		  EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT));
}

static void elog_add_recovery_mode_switch_event(void)
{
	uint64_t events = google_chromeec_get_events_b();
	uint8_t event_byte = EC_HOST_EVENT_KEYBOARD_RECOVERY;

	if (!(events & EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY)))
		return;

	if (events & EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT))
		event_byte = EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT;

	elog_add_event_byte(ELOG_TYPE_EC_EVENT, event_byte);
}

int clear_recovery_mode_switch(void)
{
	/* Log elog event before clearing */
	elog_add_recovery_mode_switch_event();

	/* Clear all host event bits requesting recovery mode. */
	return google_chromeec_clear_events_b(
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY) |
		EC_HOST_EVENT_MASK(EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT));
}
