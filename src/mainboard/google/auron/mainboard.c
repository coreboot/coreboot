/* SPDX-License-Identifier: GPL-2.0-only */

#include <drivers/intel/gma/int15.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"
#include "variant.h"

__weak void lan_init(void)
{
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
	lan_init();
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	return variant_smbios_data(dev, handle, current);
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
