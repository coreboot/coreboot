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
 * Foundation, Inc.
 */

#include <stdlib.h>
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

/* Panel Power Sequence Timing Registers. */
#define RTD2132_COMMAND_PWR_SEQ_T1	0x32 /* 1ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T2	0x33 /* 4ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T3	0x34 /* 1ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T4	0x35 /* 1ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T5	0x36 /* 4ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T6	0x37 /* 1ms units. */
#define RTD2132_COMMAND_PWR_SEQ_T7	0x38 /* 4ms units. */

/* Spread spectrum configuration */
#define RTD2132_COMMAND_SSCG_CONFIG_0	0x39
#define  RTD2132_SSCG_ENABLE		0xa0
#define  RTD2132_SSCG_DISABLE		0x20
#define RTD2132_COMMAND_SSCG_CONFIG_1	0x3a
#define  RTD2132_SSCG_CONFIG_DISABLED	0x01	/* DISABLED */
#define  RTD2132_SSCG_CONFIG_0_5	0x07	/* 0.5% */
#define  RTD2132_SSCG_CONFIG_1_0	0x0f	/* 1.0% */
#define  RTD2132_SSCG_CONFIG_1_5	0x16	/* 1.5% */

/* LVDS Swap */
#define RTD2132_COMMAND_LVDS_SWAP	0x3b
#define  RTD2132_LVDS_SWAP_DUAL		0x80
#define  RTD2132_LVDS_SWAP_NORMAL	0x04
#define  RTD2132_LVDS_SWAP_MIRROR	0x14
#define  RTD2132_LVDS_SWAP_P_N		0x24
#define  RTD2132_LVDS_SWAP_MIRROR_P_N	0x34
#define  RTD2132_LVDS_SWAP_R_L		0x0c

/* Configuration values from devicetree */
#define RTD2132_SSCG_PERCENT_0_0	0x00	/* DISABLED */
#define RTD2132_SSCG_PERCENT_0_5	0x05	/* 0.5% */
#define RTD2132_SSCG_PERCENT_1_0	0x10	/* 1.0% */
#define RTD2132_SSCG_PERCENT_1_5	0x15	/* 1.5% */

#define  RTD2132_LVDS_SWAP_CFG_DUAL		0x80
#define  RTD2132_LVDS_SWAP_CFG_NORMAL		0x00
#define  RTD2132_LVDS_SWAP_CFG_MIRROR		0x01
#define  RTD2132_LVDS_SWAP_CFG_P_N		0x02
#define  RTD2132_LVDS_SWAP_CFG_MIRROR_P_N	0x03
#define  RTD2132_LVDS_SWAP_CFG_R_L		0x04

#define RTD2132_DEBUG_REG 0

static void rtd2132_write_reg(struct device *dev, u8 reg, u8 value)
{
	if (RTD2132_DEBUG_REG)
		printk(BIOS_DEBUG, "RTD2132 0x%02x <- 0x%02x\n", reg, value);
	smbus_write_byte(dev, RTD2132_COMMAND, reg);
	smbus_write_byte(dev, RTD2132_DATA, value);
}

static void rtd2132_firmware_stop(struct device *dev)
{
	smbus_write_byte(dev, RTD2132_FIRMWARE, RTD2132_FIRMWARE_STOP);
	mdelay(60);
}

static void rtd2132_firmware_start(struct device *dev)
{
	smbus_write_byte(dev, RTD2132_FIRMWARE, RTD2132_FIRMWARE_START);
}

static void rtd2132_pps(struct device *dev, struct drivers_i2c_rtd2132_config *cfg)
{
	/* T2, T5, and T7 register values are in units of 4ms. */
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T1, cfg->t1);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T2, cfg->t2 / 4);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T3, cfg->t3);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T4, cfg->t4);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T5, cfg->t5 / 4);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T6, cfg->t6);
	rtd2132_write_reg(dev, RTD2132_COMMAND_PWR_SEQ_T7, cfg->t7 / 4);
}

static void rtd2132_sscg_enable(struct device *dev, u8 sscg_percent)
{
	/* SSCG_Config_0 */
	rtd2132_write_reg(dev, RTD2132_COMMAND_SSCG_CONFIG_0,
	                  RTD2132_SSCG_ENABLE);

	/* SSCG_Config_1 */
	rtd2132_write_reg(dev, RTD2132_COMMAND_SSCG_CONFIG_1, sscg_percent);
}

static void rtd2132_sscg_disable(struct device *dev)
{
	/* SSCG_Config_0 */
	rtd2132_write_reg(dev, RTD2132_COMMAND_SSCG_CONFIG_0,
	                  RTD2132_SSCG_DISABLE);

	/* SSCG_Config_1 */
	rtd2132_write_reg(dev, RTD2132_COMMAND_SSCG_CONFIG_1,
	                  RTD2132_SSCG_CONFIG_DISABLED);
}

static void rtd2132_sscg(struct device *dev, struct drivers_i2c_rtd2132_config *cfg)
{
	switch (cfg->sscg_percent) {
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
		       cfg->sscg_percent);
	}
}

static void rtd2132_lvds_swap(struct device *dev,
                              struct drivers_i2c_rtd2132_config *cfg)
{
	u8 swap_value = RTD2132_LVDS_SWAP_NORMAL;

	switch (cfg->lvds_swap & ~RTD2132_LVDS_SWAP_CFG_DUAL) {
	case RTD2132_LVDS_SWAP_CFG_NORMAL:
		swap_value = RTD2132_LVDS_SWAP_NORMAL;
		break;
	case RTD2132_LVDS_SWAP_CFG_MIRROR:
		swap_value = RTD2132_LVDS_SWAP_MIRROR;
		break;
	case RTD2132_LVDS_SWAP_CFG_P_N:
		swap_value = RTD2132_LVDS_SWAP_P_N;
		break;
	case RTD2132_LVDS_SWAP_CFG_MIRROR_P_N:
		swap_value = RTD2132_LVDS_SWAP_MIRROR_P_N;
		break;
	case RTD2132_LVDS_SWAP_CFG_R_L:
		swap_value = RTD2132_LVDS_SWAP_R_L;
		break;
	default:
		printk(BIOS_ERR, "RTD2132: Invalid LVDS swap value 0x%02x\n",
		       cfg->lvds_swap);
	}

	if (cfg->lvds_swap & RTD2132_LVDS_SWAP_CFG_DUAL)
		swap_value |= RTD2132_LVDS_SWAP_DUAL;

	printk(BIOS_INFO, "RTD2132: LVDS Swap 0x%02x\n", swap_value);

	rtd2132_write_reg(dev, RTD2132_COMMAND_LVDS_SWAP, swap_value);
}

static void rtd2132_defaults(struct device *dev)
{
	static const struct def_setting {
		u8 reg;
		u8 value;
	} def_settings[] = {
		{ 0x3c, 0x06 },
		{ 0x3d, 0x38 },
		{ 0x3e, 0x73 },
		{ 0x3f, 0x33 },
		{ 0x06, 0x90 },
		{ 0x06, 0xb0 },
		{ 0x06, 0x80 },
	};
	int i;

	for (i = 0; i < ARRAY_SIZE(def_settings); i++)
		rtd2132_write_reg(dev, def_settings[i].reg,
		                  def_settings[i].value);
}

static void rtd2132_setup(struct device *dev)
{
	struct drivers_i2c_rtd2132_config *config = dev->chip_info;

	if (!config)
		return;

	/* Stop running firmware */
	rtd2132_firmware_stop(dev);

	/* Panel Power Sequencing Settings. */
	rtd2132_pps(dev, config);

	/* Spread spectrum configuration */
	rtd2132_sscg(dev, config);

	/* LVDS Swap Setting. */
	rtd2132_lvds_swap(dev, config);

	/* Default settings. */
	rtd2132_defaults(dev);

	/* Start firmware */
	rtd2132_firmware_start(dev);
}

static void rtd2132_init(struct device *dev)
{
	if (dev->enabled && dev->path.type == DEVICE_PATH_I2C &&
	    ops_smbus_bus(get_pbus_smbus(dev))) {
		rtd2132_setup(dev);
	}
}

static struct device_operations rtd2132_operations = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
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
