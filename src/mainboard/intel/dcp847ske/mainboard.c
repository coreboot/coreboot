/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <southbridge/intel/common/gpio.h>

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_DEBUG, "Memory voltage: %s\n",
		get_gpio(8) ? "1.35V" : "1.5V");
	printk(BIOS_DEBUG, "BIOS_CFG jumper: %s\n",
		get_gpio(22) ? "normal (1-2)" : "setup (2-3)");
	printk(BIOS_DEBUG, "mSATA: %s\n",
		get_gpio(35) ? "present" : "absent");

	install_intel_vga_int15_handler(
		GMA_INT15_ACTIVE_LFP_NONE, GMA_INT15_PANEL_FIT_DEFAULT,
		GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
