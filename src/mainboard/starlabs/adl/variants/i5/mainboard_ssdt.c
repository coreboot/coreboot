/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen_ps2_keybd.h>
#include <commonlib/helpers.h>
#include <device/device.h>
#include <variants.h>

void starlabs_adl_mainboard_fill_ssdt(const struct device *dev)
{
	enum ps2_action_key ps2_action_keys[2] = { PS2_KEY_VOL_DOWN, PS2_KEY_VOL_UP };

	(void)dev;

	acpigen_ps2_keyboard_dsd("_SB.PCI0.PS2K", ARRAY_SIZE(ps2_action_keys), ps2_action_keys,
				 false, false, false, false, false);
}
