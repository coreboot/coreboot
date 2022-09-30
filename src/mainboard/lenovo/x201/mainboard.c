/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <device/device.h>
#include <ec/acpi/ec.h>
#include <drivers/intel/gma/int15.h>
#include <drivers/lenovo/lenovo.h>

static void fill_ssdt(const struct device *device)
{
	drivers_lenovo_serial_ports_ssdt_generate("\\_SB.PCI0.LPCB", 0);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = fill_ssdt;

	/* If we're resuming from suspend, blink suspend LED */
	if (acpi_is_wakeup_s3())
		ec_write(0x0c, 0xc7);

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_LFP, 2);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
