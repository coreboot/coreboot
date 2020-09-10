/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <acpi/acpi.h>
#include "onboard.h"
#include "ec.h"
#include <southbridge/intel/bd82x6x/pch.h>
#include <smbios.h>
#include <ec/compal/ene932/ec.h>
#include <vendorcode/google/chromeos/chromeos.h>

void mainboard_suspend_resume(void)
{
	/* Enable ACPI mode before OS resume */
	apm_control(APM_CNT_ACPI_ENABLE);
}

static void mainboard_init(struct device *dev)
{
	/* Initialize the Embedded Controller */
	parrot_ec_init();
}

static int parrot_onboard_smbios_data(struct device *dev, int *handle,
				     unsigned long *current)
{
	int len = 0;
	u8 hardware_version = parrot_rev();
	if (hardware_version < 0x2) {		/* DVT vs PVT */
		len += smbios_write_type41(
			current, handle,
			BOARD_TRACKPAD_NAME,		/* name */
			BOARD_TRACKPAD_IRQ_DVT,		/* instance */
			0,				/* segment */
			BOARD_TRACKPAD_I2C_ADDR,	/* bus */
			0,				/* device */
			0,				/* function */
			SMBIOS_DEVICE_TYPE_OTHER);	/* device type */
	} else {
		len += smbios_write_type41(
			current, handle,
			BOARD_TRACKPAD_NAME,		/* name */
			BOARD_TRACKPAD_IRQ_PVT,		/* instance */
			0,				/* segment */
			BOARD_TRACKPAD_I2C_ADDR,	/* bus */
			0,				/* device */
			0,				/* function */
			SMBIOS_DEVICE_TYPE_OTHER);	/* device type */
	}

	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = parrot_onboard_smbios_data;
	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_EDP, GMA_INT15_PANEL_FIT_DEFAULT, GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};
