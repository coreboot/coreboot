/* SPDX-License-Identifier: GPL-2.0-only */

#include <smbios.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <drivers/intel/gma/int15.h>
#include <acpi/acpi.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include "ec.h"
#include "onboard.h"

void mainboard_suspend_resume(void)
{
	/* Call SMM finalize() handlers before resume */
	apm_control(APM_CNT_FINALIZE);
}

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;

	len += smbios_write_type41(
		current, handle,
		BOARD_LIGHTSENSOR_NAME,		/* name */
		BOARD_LIGHTSENSOR_IRQ,		/* instance */
		BOARD_LIGHTSENSOR_I2C_BUS,	/* segment */
		BOARD_LIGHTSENSOR_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,		/* name */
		BOARD_TRACKPAD_IRQ,		/* instance */
		BOARD_TRACKPAD_I2C_BUS,		/* segment */
		BOARD_TRACKPAD_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	len += smbios_write_type41(
		current, handle,
		BOARD_TOUCHSCREEN_NAME,		/* name */
		BOARD_TOUCHSCREEN_IRQ,		/* instance */
		BOARD_TOUCHSCREEN_I2C_BUS,	/* segment */
		BOARD_TOUCHSCREEN_I2C_ADDR,	/* bus */
		0,				/* device */
		0,				/* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */

	return len;
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
