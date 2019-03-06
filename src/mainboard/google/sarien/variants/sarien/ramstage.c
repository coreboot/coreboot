/*
 * This file is part of the coreboot project.
 *
 * Copyright 2019 Google LLC
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootstate.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/hid/chip.h>
#include <drivers/vpd/vpd.h>
#include <soc/pci_devs.h>
#include <string.h>

/*
 * This board may have different touchscreen devices that are at the
 * same I2C slave address but need different drivers or ACPI configuration.
 *
 * The default touchscreen to be enabled is specified in Kconfig by the
 * ACPI HID of the device.  If a board is connected to a different
 * touchscreen device it can be enabled in Kconfig or by setting the
 * VPD key 'touchscreen_hid'.
 */

#define TOUCHSCREEN_I2C_ADDR	0x10
#define TOUCHSCREEN_VPD_KEY	"touchscreen_hid"

static void disable_unused_touchscreen(void *unused)
{
	struct device *i2c0 = PCH_DEV_I2C0;
	struct bus *i2c_slaves = i2c0->link_list;
	struct device *slave = i2c_slaves->children;
	char touchscreen_hid[9] = CONFIG_TOUCHSCREEN_HID;
	struct drivers_i2c_hid_config *info;

	/* Look for VPD key that indicates which touchscreen is present */
	if (CONFIG(VPD) &&
	    !vpd_gets(TOUCHSCREEN_VPD_KEY, touchscreen_hid,
		      ARRAY_SIZE(touchscreen_hid), VPD_ANY))
		printk(BIOS_INFO, "%s: VPD key '%s' not found, default to %s\n",
		       __func__, TOUCHSCREEN_VPD_KEY, touchscreen_hid);

	/* Go through all I2C slave devices on this bus */
	while (slave) {
		/* Find all the I2C slaves with the matching address */
		if (slave->path.type == DEVICE_PATH_I2C &&
		    slave->path.i2c.device == TOUCHSCREEN_I2C_ADDR) {
			info = slave->chip_info;
			/* Disable all devices except the matching HID */
			if (strncmp(info->generic.hid, touchscreen_hid,
				    ARRAY_SIZE(touchscreen_hid))) {
				printk(BIOS_INFO, "%s: Disable %s\n", __func__,
				       info->generic.hid);
				slave->enabled = 0;
			} else {
				printk(BIOS_INFO, "%s: Enable %s\n", __func__,
				       info->generic.hid);
			}
		}
		slave = slave->sibling;
	}
}
BOOT_STATE_INIT_ENTRY(BS_WRITE_TABLES, BS_ON_ENTRY,
		      disable_unused_touchscreen, NULL);
