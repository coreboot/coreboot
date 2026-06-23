/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <build.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <ec/lenovo/h8/h8.h>
#include <smbios.h>
#include <string.h>

const char *smbios_mainboard_bios_version(void)
{
	/* Satisfy thinkpad_acpi. */
	if (strlen(CONFIG_LOCALVERSION))
		return "CBET4000 " CONFIG_LOCALVERSION;

	return "CBET4000 " COREBOOT_VERSION;
}

static void mainboard_enable(struct device *dev)
{
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
