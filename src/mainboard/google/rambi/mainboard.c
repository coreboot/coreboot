/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <smbios.h>
#include "ec.h"
#include <variant/onboard.h>
#include <soc/gpio.h>
#include <soc/int15.h>
#include <bootstate.h>

static void mainboard_init(struct device *dev)
{
	mainboard_ec_init();
#if CONFIG(BOARD_GOOGLE_NINJA) || CONFIG(BOARD_GOOGLE_SUMO)
	lan_init();
#endif
}

static int mainboard_smbios_data(struct device *dev, int *handle,
				 unsigned long *current)
{
	int len = 0;
#ifdef BOARD_TRACKPAD_NAME
	len += smbios_write_type41(
		current, handle,
		BOARD_TRACKPAD_NAME,            /* name */
		BOARD_TRACKPAD_IRQ,             /* instance */
		BOARD_TRACKPAD_I2C_BUS,         /* segment */
		BOARD_TRACKPAD_I2C_ADDR,        /* bus */
		0,                              /* device */
		0,                             /* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */
#endif
#ifdef BOARD_TOUCHSCREEN_NAME
	len += smbios_write_type41(
		current, handle,
		BOARD_TOUCHSCREEN_NAME,         /* name */
		BOARD_TOUCHSCREEN_IRQ,          /* instance */
		BOARD_TOUCHSCREEN_I2C_BUS,      /* segment */
		BOARD_TOUCHSCREEN_I2C_ADDR,     /* bus */
		0,                              /* device */
		0,                             /* function */
		SMBIOS_DEVICE_TYPE_OTHER);	/* device type */
#endif
	return len;
}

// mainboard_enable is executed as first thing after
// enumerate_buses().

static void mainboard_enable(struct device *dev)
{
	dev->ops->init = mainboard_init;
	dev->ops->get_smbios_data = mainboard_smbios_data;

	/* Install custom int15 handler for VGA OPROM */
	if (CONFIG(VGA_ROM_RUN))
		install_baytrail_vga_int15_handler();
}

struct chip_operations mainboard_ops = {
	.enable_dev = mainboard_enable,
};

static void edp_vdden_cb(void *unused)
{
	ncore_select_func(SOC_DDI1_VDDEN_PAD, PAD_FUNC2);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, edp_vdden_cb, NULL);
