/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/azalia_device.h>
#include <types.h>
#include <console/console.h>

#include "variants/baseboard/include/eeprom.h"
#include "variants/baseboard/include/variant/variants.h"

void mainboard_azalia_program_runtime_verbs(u8 *base, u32 viddid)
{
	if (viddid == 0x10ec0888) {
		u8 hsi = get_bmc_hsi();

		if (hsi >= 4)
			mainboard_r0x_configure_alc888(base, viddid);
	}
}
