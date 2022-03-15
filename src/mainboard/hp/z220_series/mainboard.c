/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>

#if CONFIG(GENERATE_SMBIOS_TABLES)
static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;

	// add IPMI Device Information
	len += smbios_write_type38(
		current, handle,
		SMBIOS_BMC_INTERFACE_KCS,
		0x20, // IPMI Version
		0x20, // I2C address
		0xff, // no NV storage
		0, // IO port interface address
		0,
		0); // no IRQ

	return len;
}
#endif

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_NONE,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
#if CONFIG(GENERATE_SMBIOS_TABLES)
	dev->ops->get_smbios_data = mainboard_smbios_data;
#endif
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
