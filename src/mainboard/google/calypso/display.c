/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootmode.h>
#include <soc/clock.h>
#include <soc/rpmh_config.h>

#include "board.h"
#include "display.h"

void display_startup(void)
{
	if (!display_init_required() || (CONFIG(VBOOT_LID_SWITCH) && !get_lid_switch())) {
		printk(BIOS_INFO, "Skipping display init.\n");
		return;
	}

	/* Initialize RPMh subsystem and display power rails */
	if (display_rpmh_init() != CB_SUCCESS)
		return;

	enable_mdss_clk();
}
