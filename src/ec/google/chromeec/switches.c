/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <ec/google/chromeec/ec.h>
#include <elog.h>
#include <security/vboot/misc.h>

#if CONFIG(EC_GOOGLE_CHROMEEC_LPC)
/*
 * Retrieves the state of the lid switch.
 *
 * The get_lid_switch() function checks if lid switch functionality is enabled
 * in the coreboot configuration. If the VBOOT_LID_SWITCH Kconfig option is
 * enabled, it retrieves the lid switch state from the Embedded Controller (EC).
 *
 * If the GBB_FLAG_DISABLE_LID_SHUTDOWN Kconfig option is enabled, this function
 * will always return 1 (lid open), effectively faking the lid status. This is
 * intended for stages post-romstage, particularly during display-related
 * initialization, where lid-based shutdown should be suppressed.
 *
 * @return 1 if the lid is open, 0 if the lid is closed, or -1 if the lid switch
 * functionality is not configured (VBOOT_LID_SWITCH is disabled).
 */
int get_lid_switch(void)
{
	if (!CONFIG(VBOOT_LID_SWITCH))
		return -1;

	/* If GBB_FLAG_DISABLE_LID_SHUTDOWN is enabled, fake the lid status as always open. */
	if (!ENV_ROMSTAGE_OR_BEFORE && vboot_is_gbb_flag_set(VB2_GBB_FLAG_DISABLE_LID_SHUTDOWN))
		return 1; /* Always return 1 (lid open) */

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
