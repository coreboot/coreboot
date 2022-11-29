/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <device/device.h>
#include <ec/lenovo/pmh7/pmh7.h>
#include <ec/lenovo/h8/h8.h>
#include <ec/acpi/ec.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/chip.h>
#include <drivers/intel/gma/int15.h>
#include <acpi/acpigen.h>

#define PANEL INT15_5F35_CL_DISPLAY_DEFAULT

static void mainboard_init(struct device *dev)
{
	struct southbridge_intel_i82801gx_config *config;
	struct device *idedev;

	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					PANEL, 3);

	/* If we're resuming from suspend, blink suspend LED */
	if (acpi_is_wakeup_s3())
		ec_write(0x0c, 0xc7);

	idedev = pcidev_on_root(0x1f, 1);

	if (!(inb(DEFAULT_GPIOBASE + 0x0c) & 0x40)) {
		/* legacy I/O connected */
		pmh7_ultrabay_power_enable(1);
		ec_write(0x0c, 0x84);
	} else if (idedev && idedev->chip_info &&
		   h8_ultrabay_device_present()) {
		config = idedev->chip_info;
		config->ide_enable_primary = true;
		pmh7_ultrabay_power_enable(1);
		ec_write(0x0c, 0x84);
	} else {
		pmh7_ultrabay_power_enable(0);
		ec_write(0x0c, 0x04);
	}

	/* set dock status led */
	ec_write(0x0c, 0x08);
	ec_write(0x0c, inb(0x164c) & 8 ? 0x89 : 0x09);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
