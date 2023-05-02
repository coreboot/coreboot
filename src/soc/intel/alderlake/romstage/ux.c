/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/early_graphics.h>
#include <pc80/vga.h>
#include <ux_locales.h>

#include "ux.h"

#define UX_MEMORY_TRAINING_DESC "memory_training_desc"

bool ux_inform_user_of_update_operation(const char *name)
{
	if (!CONFIG(MAINBOARD_HAS_EARLY_LIBGFXINIT) ||
	    !early_graphics_init())
		return false;

	printk(BIOS_INFO, "Informing user on-display of %s.\n", name);

	const char *text = ux_locales_get_text(UX_MEMORY_TRAINING_DESC);
	/* No localized text found; fallback to built-in English. */
	if (!text)
		text = "Your device is finishing an update. "
		       "This may take 1-2 minutes.\n"
		       "Please do not turn off your device.";
	vga_write_text(VGA_TEXT_CENTER, VGA_TEXT_HORIZONTAL_MIDDLE,
		       (const unsigned char *)text);
	ux_locales_unmap();
	return true;
}
