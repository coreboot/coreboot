/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <soc/int15.h>

static void mainboard_enable(struct device *dev)
{
	/* Install custom int15 handler for VGA OPROM */
	if (CONFIG(VGA_ROM_RUN))
		install_baytrail_vga_int15_handler();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
