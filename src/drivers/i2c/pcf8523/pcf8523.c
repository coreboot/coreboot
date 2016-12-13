/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Siemens AG.
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

#include <device/smbus.h>
#include <version.h>
#include <console/console.h>
#include <bcd.h>
#include "chip.h"

/* Set RTC date from coreboot build date. */
static void pcf8523_set_build_date(struct device *dev)
{
	smbus_write_byte(dev, YEAR_REG, coreboot_build_date.year);
	smbus_write_byte(dev, MONTH_REG, coreboot_build_date.month);
	smbus_write_byte(dev, WEEKDAY_REG, coreboot_build_date.weekday);
	smbus_write_byte(dev, DAY_REG, coreboot_build_date.day);
}

/* Set RTC date from user defined date (available in e.g. device tree). */
static void pcf8523_set_user_date(struct device *dev)
{
	struct drivers_i2c_pcf8523_config *config = dev->chip_info;
	smbus_write_byte(dev, YEAR_REG, bin2bcd(config->user_year));
	smbus_write_byte(dev, MONTH_REG, bin2bcd(config->user_month));
	smbus_write_byte(dev, DAY_REG, bin2bcd(config->user_day));
	smbus_write_byte(dev, WEEKDAY_REG, bin2bcd(config->user_weekday));
}

static void pcf8523_final(struct device *dev)
{
	/* Read back current RTC date and time and print it to the console. */
	printk(BIOS_INFO, "%s: Current date %02d.%02d.%02d %02d:%02d:%02d\n",
		dev->chip_ops->name,
		bcd2bin(smbus_read_byte(dev, MONTH_REG)),
		bcd2bin(smbus_read_byte(dev, DAY_REG)),
		bcd2bin(smbus_read_byte(dev, YEAR_REG)),
		bcd2bin(smbus_read_byte(dev, HOUR_REG)),
		bcd2bin(smbus_read_byte(dev, MINUTE_REG)),
		bcd2bin(smbus_read_byte(dev, SECOND_REG)) & ~OS_BIT);
}

static void pcf8523_init(struct device *dev)
{
	struct drivers_i2c_pcf8523_config *config = dev->chip_info;
	uint8_t reg = 0;

	if (!(smbus_read_byte(dev, SECOND_REG) & OS_BIT)) {
		/* Set control registers to a known good value even if no
		 * power loss event was recognized. There were issues with
		 * this RTC in the past where control registers were
		 * corrupted and OS bit was not set. */
		reg = smbus_read_byte(dev, CTRL_REG_1);
		reg &= ~(STOP_BIT | CAP_SEL);
		reg |= ((!!config->cap_sel) << 7);
		smbus_write_byte(dev, CTRL_REG_1, reg);
		reg = smbus_read_byte(dev, CTRL_REG_3);
		reg &= ~PM_MASK;
		reg |= ((config->power_mode & 0x07) << 5);
		smbus_write_byte(dev, CTRL_REG_3, reg);
		reg = smbus_read_byte(dev, TMR_CLKOUT_REG);
		reg &= ~COF_MASK;
		reg |= ((config->cof_selection & 0x07) << 3);
		smbus_write_byte(dev, TMR_CLKOUT_REG, reg);
		return;
	}

	/* Initialize the RTC fully only if a power-loss event was recognized.
	 * In this case RTC will be set up with default date and time. */
	smbus_write_byte(dev, CTRL_REG_1, ((!!config->cap_sel) << 7) |
					  ((!!config->second_int_en) << 2) |
					  ((!!config->alarm_int_en) << 1) |
					  (!!config->correction_int_en));

	smbus_write_byte(dev, CTRL_REG_2, ((!!config->wdt_int_en) << 2) |
					  ((!!config->tmrA_int_en) << 1) |
					  (!!config->tmrB_int_en));

	smbus_write_byte(dev, CTRL_REG_3, ((config->power_mode & 0x07) << 5) |
					  ((!!config->bat_switch_int_en) << 1) |
					  (!!config->bat_low_int_en));

	smbus_write_byte(dev, OFFSET_REG, ((!!config->offset_mode) << 7) |
					  (config->offset_val & 0x7f));

	smbus_write_byte(dev, TMR_CLKOUT_REG, ((!!config->tmrA_int_mode) << 7) |
					((!!config->tmrB_int_mode) << 6) |
					((config->cof_selection & 0x07) << 3) |
					((config->tmrA_mode & 0x03) << 1) |
					(!!config->tmrB_mode));

	smbus_write_byte(dev, TMR_A_FREQ_REG, (config->tmrA_prescaler & 0x7));

	smbus_write_byte(dev, TMR_B_FREQ_REG,  (config->tmrB_prescaler & 0x7) |
					((config->tmrB_pulse_cfg & 0x7) << 4));

	/* Before setting the clock stop oscillator. */
	reg = smbus_read_byte(dev, CTRL_REG_1);
	reg |= STOP_BIT;
	smbus_write_byte(dev, CTRL_REG_1, reg);
	if (config->set_user_date) {
		/* Set user date defined in device tree. */
		printk(BIOS_DEBUG, "%s: Set to user date\n",
			dev->chip_ops->name);
		pcf8523_set_user_date(dev);
	} else {
		/* Set date from coreboot build. */
		printk(BIOS_DEBUG, "%s: Set to coreboot build date\n",
			dev->chip_ops->name);
		pcf8523_set_build_date(dev);
	}
	/* Set time to 01:00:00 */
	smbus_write_byte(dev, HOUR_REG, 1);
	smbus_write_byte(dev, MINUTE_REG, 0);
	smbus_write_byte(dev, SECOND_REG, 0);
	/* Start oscillator again as the clock is set up now */
	reg &= ~STOP_BIT;
	smbus_write_byte(dev, CTRL_REG_1, reg);
}

static struct device_operations pcf8523c_ops = {
	.read_resources		= DEVICE_NOOP,
	.set_resources		= DEVICE_NOOP,
	.enable_resources	= DEVICE_NOOP,
	.init			= pcf8523_init,
	.final			= pcf8523_final
};

static void pcf8523_enable(struct device *dev)
{
	dev->ops = &pcf8523c_ops;
}

struct chip_operations drivers_i2c_pcf8523_ops = {
	CHIP_NAME("PCF8523")
	.enable_dev = pcf8523_enable
};
