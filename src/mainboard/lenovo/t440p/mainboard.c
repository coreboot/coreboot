/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <ec/lenovo/h8/h8.h>

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
