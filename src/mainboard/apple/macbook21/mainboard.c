/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <northbridge/intel/i945/i945.h>
#include <drivers/intel/gma/int15.h>
#include <ec/acpi/ec.h>

#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

static void mainboard_init(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS, GMA_INT15_PANEL_FIT_DEFAULT, PANEL, 3);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

static void mainboard_final(void *chip_info)
{
	ec_set_bit(0x10, 2); /* switch off led */
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};
