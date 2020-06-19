/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <pc80/i8254.h>

static void mainboard_final(void *unused)
{
	if (CONFIG(BEEP_ON_BOOT))
		beep(1500, 100);
}

struct chip_operations mainboard_ops = {
	.final = mainboard_final,
};
