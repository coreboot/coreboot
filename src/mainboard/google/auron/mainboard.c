/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <option.h>
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

static void mainboard_fill_ssdt(const struct device *dev)
{
	if (CONFIG(BOARD_GOOGLE_LULU)) {
		/* Get touchscreen enable option from CFR */
		unsigned int touchscreen_enabled = get_uint_option("touchscreen", 1);

		acpigen_write_scope("\\_SB.PCI0.I2C1");
		acpigen_write_store_int_to_namestr(touchscreen_enabled ? 1 : 0, "S2EN");
		acpigen_pop_len(); /* Scope */
	}
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	return variant_smbios_data(dev, handle, current);
}

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = mainboard_smbios_data;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_CENTERING, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
