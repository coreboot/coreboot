/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot/coreboot_tables.h>
#include <bootmode.h>
#include <bootstate.h>
#include <console/console.h>
#include <ec/google/chromeec/ec.h>
#include <framebuffer_info.h>

static void handle_board_poweroff(void *unused)
{
	if (!CONFIG(EC_GOOGLE_CHROMEEC))
		return;

	/* If lid is open, we do nothing */
	if (CONFIG(VBOOT_LID_SWITCH) && get_lid_switch())
		return;

	const struct lb_framebuffer *fb = get_lb_framebuffer();

	/*
	 * Shut down if:
	 * 1. Framebuffer is NULL (no display initialized)
	 * OR
	 * 2. Framebuffer exists but 'has_external_display' is false
	 */
	if (!fb || !fb->flags.has_external_display) {
		printk(BIOS_INFO, "Lid closed, no external display: Initiating shutdown.\n");
		google_chromeec_ap_poweroff();
	}
}

BOOT_STATE_INIT_ENTRY(BS_PAYLOAD_LOAD, BS_ON_ENTRY, handle_board_poweroff, NULL);
