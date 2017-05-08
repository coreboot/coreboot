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
#include <device/device.h>
#include <version.h>
#include <console/console.h>
#include <bcd.h>
#include "chip.h"
#include "rx6110sa.h"

#define I2C_BUS_NUM	(dev->bus->secondary - 1)
#define I2C_DEV_NUM	(dev->path.i2c.device)

/* Set RTC date from coreboot build date. */
static void rx6110sa_set_build_date(struct device *dev)
{
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, YEAR_REG,
			coreboot_build_date.year);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, MONTH_REG,
			coreboot_build_date.month);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, DAY_REG,
			coreboot_build_date.day);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, WEEK_REG,
			(1 << coreboot_build_date.weekday));
}

/* Set RTC date from user defined date (available in e.g. device tree). */
static void rx6110sa_set_user_date(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;

	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, YEAR_REG,
			bin2bcd(config->user_year));
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, MONTH_REG,
			bin2bcd(config->user_month));
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, DAY_REG,
			bin2bcd(config->user_day));
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, WEEK_REG,
			(1 << config->user_weekday));
}

static void rx6110sa_final(struct device *dev)
{
	uint8_t hour, minute, second, year, month, day;

	/* Read back current RTC date and time and print it to the console. */
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, HOUR_REG, &hour);
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, MINUTE_REG, &minute);
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, SECOND_REG, &second);
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, YEAR_REG, &year);
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, MONTH_REG, &month);
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, DAY_REG, &day);

	printk(BIOS_INFO, "%s: Current date %02d.%02d.%02d %02d:%02d:%02d\n",
		dev->chip_ops->name, bcd2bin(month), bcd2bin(day),
		bcd2bin(year), bcd2bin(hour), bcd2bin(minute), bcd2bin(second));
}

static void rx6110sa_init(struct device *dev)
{
	struct drivers_i2c_rx6110sa_config *config = dev->chip_info;
	uint8_t reg;

	/* Do a dummy read first. */
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, SECOND_REG, &reg);

	/*
	 * Check VLF-bit which indicates the RTC data loss, such as due to a
	 * supply voltage drop.
	 */
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, FLAG_REGISTER, &reg);

	if (!(reg & VLF_BIT))
		/* No voltage low detected, everything is well. */
		return;

	/*
	 * Voltage low detected, initialize RX6110 SA again.
	 * Set first some registers to known state.
	 */
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, BATTERY_BACKUP_REG, 0x00);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, RESERVED_BIT_REG, RTC_INIT_VALUE);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, DIGITAL_REG, 0x00);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, IRQ_CONTROL_REG, 0x00);

	/* Clear timer enable bit and set frequency of clock output. */
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, EXTENSION_REG, &reg);
	reg &= ~(FSEL_MASK | TE_BIT);
	reg |= (config->cof_selection << 6);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, EXTENSION_REG, reg);

	/* Clear voltage low detect bit. */
	i2c_readb(I2C_BUS_NUM, I2C_DEV_NUM, FLAG_REGISTER, &reg);
	reg &= ~VLF_BIT;
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, FLAG_REGISTER, reg);

	/* Before setting the clock stop oscillator. */
	i2c_writeb(I2C_BUS_NUM,	I2C_DEV_NUM, CTRL_REG, STOP_BIT);
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
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, HOUR_REG, 1);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, MINUTE_REG, 0);
	i2c_writeb(I2C_BUS_NUM, I2C_DEV_NUM, SECOND_REG, 0);
	/* Start oscillator again as the RTC is set up now. */
	i2c_writeb(I2C_BUS_NUM,	I2C_DEV_NUM, CTRL_REG, 0x00);
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
