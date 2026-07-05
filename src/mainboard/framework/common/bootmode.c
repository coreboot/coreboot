/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <bootmode.h>
#include <ec/google/chromeec/ec.h>

/*
 * The Framework EC enters manual recovery mode when ESC is held while
 * powering on ("BIOS crisis recovery"). Unlike the ChromeOS ECs it does not
 * post EC_HOST_EVENT_KEYBOARD_RECOVERY, so poll the sysinfo flag instead of
 * using EC_GOOGLE_CHROMEEC_SWITCHES. The EC clears the flag on shutdown, so
 * recovery mode persists across warm resets within one power-on session.
 */
int get_recovery_mode_switch(void)
{
	return google_chromeec_in_manual_recovery();
}
