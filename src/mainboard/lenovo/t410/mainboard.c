/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/device.h>
#include <arch/acpi.h>
#include <drivers/intel/gma/int15.h>
#include "dock.h"

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_LFP, 2);
	init_dock();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
