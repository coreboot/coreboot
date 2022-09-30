/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <drivers/lenovo/lenovo.h>

static void fill_ssdt(const struct device *device)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 0);
}

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_CENTERING,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 2);

	dev->ops->acpi_fill_ssdt = fill_ssdt;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
