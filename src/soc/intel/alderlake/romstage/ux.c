/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <intelblocks/early_graphics.h>
#include <pc80/vga.h>
#include <timestamp.h>
#include <ux_locales.h>

#include "ux.h"

static bool ux_inform_user_of_operation(const char *name, enum ux_locale_msg id)
{
	timestamp_add_now(TS_ESOL_START);

	if (!CONFIG(CHROMEOS_ENABLE_ESOL) || !early_graphics_init()) {
		timestamp_add_now(TS_ESOL_END);
		return false;
	}

	printk(BIOS_INFO, "Informing user on-display of %s.\n", name);

	const char *text = ux_locales_get_text(id);

	vga_write_text(VGA_TEXT_CENTER, VGA_TEXT_HORIZONTAL_MIDDLE,
		       (const unsigned char *)text);
	ux_locales_unmap();
	timestamp_add_now(TS_ESOL_END);
	return true;
}

bool ux_inform_user_of_update_operation(const char *name)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_MEMORY_TRAINING);
}

bool ux_inform_user_of_poweroff_operation(const char *name)
{
	return ux_inform_user_of_operation(name, UX_LOCALE_MSG_LOW_BATTERY);
}
