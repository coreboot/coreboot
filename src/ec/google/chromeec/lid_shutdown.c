/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootstate.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <framebuffer_info.h>
#include <option.h>

static void chromeec_lid_shutdown(void *unused)
{
	const struct lb_framebuffer *fb;

	if (!get_uint_option("lid_shutdown", CONFIG(EC_GOOGLE_CHROMEEC_LID_SHUTDOWN)))
		return;

	/* Lid open: continue boot. */
	if (google_chromeec_get_switches() & EC_SWITCH_LID_OPEN)
		return;

	fb = get_lb_framebuffer();

	/*
	 * Shut down if there is no framebuffer yet, or the active display is
	 * not an external one (clamshell panel only). Keep running when an
	 * external display is attached so lid-closed + HDMI/DP still works.
	 */
	if (fb && fb->flags.has_external_display)
		return;

	printk(BIOS_INFO, "Lid closed, no external display: powering off.\n");
	google_chromeec_ap_poweroff();
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, chromeec_lid_shutdown, NULL);
