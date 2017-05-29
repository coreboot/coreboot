/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Siemens AG.
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

#include <device/i2c.h>
#include <device/smbus.h>
#include <device/device.h>
#include <version.h>
#include <console/console.h>
#include <bcd.h>
#include "chip.h"
#include "rx6110sa.h"

#define I2C_BUS_NUM	(dev->bus->secondary - 1)
#define I2C_DEV_NUM	(dev->path.i2c.device)

/* Function to write a register in the RTC with the given value. */
static void rx6110sa_write(struct device *dev, uint8_t reg, uint8_t val)
{
	if (IS_ENABLED(CONFIG_RX6110SA_USE_SMBUS))
		smbus_write_byte(dev, reg, val);
	else
		i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, reg, val);
}

/* Function to read a register in the RTC. */
static uint8_t rx6110sa_read(struct device *dev, uint8_t reg)
{
	uint8_t val = 0;

	if (IS_ENABLED(CONFIG_RX6110SA_USE_SMBUS))
		val = smbus_read_byte(dev, reg);
	else
		i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, reg, &val);
	return val;
}

/* Set RTC date from coreboot build date. */
static void rx6110sa_set_build_date(struct device *dev)
{
	rx6110sa_write(dev, YEAR_REG, coreboot_build_date.year);
	rx6110sa_write(dev, MONTH_REG, coreboot_build_date.month);
	rx6110sa_write(dev, DAY_REG, coreboot_build_date.day);
	rx6110sa_write(dev, WEEK_REG, (1 << coreboot_build_date.weekday));
}

/* Set RTC date from user defined date (available in e.g. device tree). */
static void rx6110sa_set_user_date(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;

	rx6110sa_write(dev, YEAR_REG, bin2bcd(config->user_year));
	rx6110sa_write(dev, MONTH_REG, bin2bcd(config->user_month));
	rx6110sa_write(dev, DAY_REG, bin2bcd(config->user_day));
	rx6110sa_write(dev, WEEK_REG, (1 << config->user_weekday));
}

static void rx6110sa_final(struct device *dev)
{
	uint8_t hour, minute, second, year, month, day;

	/* Read back current RTC date and time and print it to the console. */
	hour = rx6110sa_read(dev, HOUR_REG);
	minute = rx6110sa_read(dev, MINUTE_REG);
	second = rx6110sa_read(dev, SECOND_REG);
	year = rx6110sa_read(dev, YEAR_REG);
	month = rx6110sa_read(dev, MONTH_REG);
	day = rx6110sa_read(dev, DAY_REG);

	printk(BIOS_INFO, "%s: Current date %02d.%02d.%02d %02d:%02d:%02d\n",
		dev->chip_ops->name, bcd2bin(month), bcd2bin(day),
		bcd2bin(year), bcd2bin(hour), bcd2bin(minute), bcd2bin(second));
}

static void rx6110sa_init(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;
	uint8_t reg;

	/* Do a dummy read first. */
	reg = rx6110sa_read(dev, SECOND_REG);

	/*
	 * Check VLF-bit which indicates the RTC data loss, such as due to a
	 * supply voltage drop.
	 */
	reg = rx6110sa_read(dev, FLAG_REGISTER);

	if (!(reg & VLF_BIT))
		/* No voltage low detected, everything is well. */
		return;

	/*
	 * Voltage low detected, initialize RX6110 SA again.
	 * Set first some registers to known state.
	 */
	rx6110sa_write(dev, BATTERY_BACKUP_REG, 0x00);
	rx6110sa_write(dev, RESERVED_BIT_REG, RTC_INIT_VALUE);
	rx6110sa_write(dev, DIGITAL_REG, 0x00);
	rx6110sa_write(dev, IRQ_CONTROL_REG, 0x00);

	/* Clear timer enable bit and set frequency of clock output. */

	reg = rx6110sa_read(dev, EXTENSION_REG);
	reg &= ~(FSEL_MASK | TE_BIT);
	reg |= (config->cof_selection << 6);
	rx6110sa_write(dev, EXTENSION_REG, reg);

	/* Clear voltage low detect bit. */
	reg = rx6110sa_read(dev, FLAG_REGISTER);
	reg &= ~VLF_BIT;
	rx6110sa_write(dev, FLAG_REGISTER, reg);

	/* Before setting the clock stop oscillator. */
	rx6110sa_write(dev, CTRL_REG, STOP_BIT);

	if (config->set_user_date) {
		/* Set user date defined in device tree. */
		printk(BIOS_DEBUG, "%s: Set to user date\n",
				dev->chip_ops->name);
		rx6110sa_set_user_date(dev);
	} else {
		/* Set date from coreboot build. */
		printk(BIOS_DEBUG, "%s: Set to coreboot build date\n",
				dev->chip_ops->name);
		rx6110sa_set_build_date(dev);
	}
	rx6110sa_write(dev, HOUR_REG, 1);
	rx6110sa_write(dev, MINUTE_REG, 0);
	rx6110sa_write(dev, SECOND_REG, 0);
	/* Start oscillator again as the RTC is set up now. */
	rx6110sa_write(dev, CTRL_REG, 0x00);
}

static struct device_operations rx6110sa_ops = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= rx6110sa_init,
	.final			= rx6110sa_final
};

static void rx6110sa_enable(struct device *dev)
{
	dev->ops = &rx6110sa_ops;
}

struct chip_operations drivers_i2c_rx6110sa_ops = {
	CHIP_NAME("RX6110 SA")
	.enable_dev = rx6110sa_enable
};
