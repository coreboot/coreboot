/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA, 02110-1301 USA
 */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <device/smbus.h>
#include <device/pci.h>
#include "chip.h"

/* Chip commands */
#define RTD2132_COMMAND			0x01
#define RTD2132_DATA			0x00
#define RTD2132_FIRMWARE		0x80
#define  RTD2132_FIRMWARE_START		0x00
#define  RTD2132_FIRMWARE_STOP		0x01

/* Spread spectrum configuration */
#define RTD2132_COMMAND_SSCG_CONFIG_0	0x39
#define  RTD2132_SSCG_ENABLE		0xa0
#define  RTD2132_SSCG_DISABLE		0x20
#define RTD2132_COMMAND_SSCG_CONFIG_1	0x3a
#define  RTD2132_SSCG_CONFIG_DISABLED	0x01	/* DISABLED */
#define  RTD2132_SSCG_CONFIG_0_5	0x07	/* 0.5% */
#define  RTD2132_SSCG_CONFIG_1_0	0x0f	/* 1.0% */
#define  RTD2132_SSCG_CONFIG_1_5	0x16	/* 1.5% */

/* Configuration values from devicetree */
#define RTD2132_SSCG_PERCENT_0_0	0x00	/* DISABLED */
#define RTD2132_SSCG_PERCENT_0_5	0x05	/* 0.5% */
#define RTD2132_SSCG_PERCENT_1_0	0x10	/* 1.0% */
#define RTD2132_SSCG_PERCENT_1_5	0x15	/* 1.5% */

static void rtd2132_firmware_stop(device_t dev)
{
	smbus_write_byte(dev, RTD2132_FIRMWARE, RTD2132_FIRMWARE_STOP);
	mdelay(2);
}

static void rtd2132_firmware_start(device_t dev)
{
	smbus_write_byte(dev, RTD2132_FIRMWARE, RTD2132_FIRMWARE_START);
}

static void rtd2132_sscg_enable(device_t dev, u8 sscg_percent)
{
	/* SSCG_Config_0 */
	smbus_write_byte(dev, RTD2132_COMMAND, RTD2132_COMMAND_SSCG_CONFIG_0);
	smbus_write_byte(dev, RTD2132_DATA, RTD2132_SSCG_ENABLE);

	/* SSCG_Config_1 */
	smbus_write_byte(dev, RTD2132_COMMAND, RTD2132_COMMAND_SSCG_CONFIG_1);
	smbus_write_byte(dev, RTD2132_DATA, sscg_percent);
}

static void rtd2132_sscg_disable(device_t dev)
{
	/* SSCG_Config_0 */
	smbus_write_byte(dev, RTD2132_COMMAND, RTD2132_COMMAND_SSCG_CONFIG_0);
	smbus_write_byte(dev, RTD2132_DATA, RTD2132_SSCG_DISABLE);

	/* SSCG_Config_1 */
	smbus_write_byte(dev, RTD2132_COMMAND, RTD2132_COMMAND_SSCG_CONFIG_1);
	smbus_write_byte(dev, RTD2132_DATA, RTD2132_SSCG_CONFIG_DISABLED);
}

static void rtd2132_setup(device_t dev)
{
	struct drivers_i2c_rtd2132_config *config = dev->chip_info;

	if (!config)
		return;

	/* Stop running firmware */
	rtd2132_firmware_stop(dev);

	/* Spread spectrum configuration */
	switch (config->sscg_percent) {
	case RTD2132_SSCG_PERCENT_0_0:
		printk(BIOS_INFO, "RTD2132: Disable Spread Spectrum\n");
		rtd2132_sscg_disable(dev);
		break;
	case RTD2132_SSCG_PERCENT_0_5:
		printk(BIOS_INFO, "RTD2132: Enable 0.5%% Spread Spectrum\n");
		rtd2132_sscg_enable(dev, RTD2132_SSCG_CONFIG_0_5);
		break;
	case RTD2132_SSCG_PERCENT_1_0:
		printk(BIOS_INFO, "RTD2132: Enable 1.0%% Spread Spectrum\n");
		rtd2132_sscg_enable(dev, RTD2132_SSCG_CONFIG_1_0);
		break;
	case RTD2132_SSCG_PERCENT_1_5:
		printk(BIOS_INFO, "RTD2132: Enable 1.5%% Spread Spectrum\n");
		rtd2132_sscg_enable(dev, RTD2132_SSCG_CONFIG_1_5);
		break;
	default:
		printk(BIOS_ERR, "RTD2132: Invalid Spread Spectrum 0x%02x\n",
		       config->sscg_percent);
	}

	/* Start firmware */
	rtd2132_firmware_start(dev);
}

static void rtd2132_init(device_t dev)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C &&
	    ops_smbus_bus(get_pbus_smbus(dev))) {
		rtd2132_setup(dev);
	}
}

static void rtd2132_noop(device_t dummy)
{
}

static struct device_operations rtd2132_operations = {
	.read_resources		= rtd2132_noop,
	.set_resources		= rtd2132_noop,
	.enable_resources	= rtd2132_noop,
	.init			= rtd2132_init,
};

static void enable_dev(struct device *dev)
{
	dev->ops = &rtd2132_operations;
}

struct chip_operations drivers_i2c_rtd2132_ops = {
	CHIP_NAME("Realtek RTD2132 LVDS Bridge")
	.enable_dev = enable_dev,
};
