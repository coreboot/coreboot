/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/early_graphics.h>
#include <pc80/vga.h>

#include "ux.h"

bool ux_inform_user_of_update_operation(const char *name)
{
	if (!CONFIG(MAINBOARD_HAS_EARLY_LIBGFXINIT) ||
	    !early_graphics_init())
		return false;

	printk(BIOS_INFO, "Informing user on-display of %s.\n", name);
	vga_write_text(VGA_TEXT_CENTER, VGA_TEXT_HORIZONTAL_MIDDLE,
		       "Your device is finishing an update. This may take 1-2 minutes.\nPlease do not turn off your device.");
	return true;
}
