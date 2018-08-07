/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
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

#include <bcd.h>
#include <console/console.h>
#include <device/i2c_simple.h>
#include <rtc.h>
#include <stdint.h>

enum TPS65913_RTC_REG {
	TPS65913_SECONDS_REG		= 0x00,
	TPS65913_MINUTES_REG		= 0x01,
	TPS65913_HOURS_REG		= 0x02,
	TPS65913_DAYS_REG		= 0x03,
	TPS65913_MONTHS_REG		= 0x04,
	TPS65913_YEARS_REG		= 0x05,
	TPS65913_WEEKS_REG		= 0x06,
	TPS65913_RTC_CTRL_REG		= 0x10,
	TPS65913_RTC_STATUS_REG	= 0x11,
	TPS65913_RTC_INTERRUPTS_REG	= 0x12,
};

enum {
	TPS65913_RTC_CTRL_STOP		= (1 << 0),
	TPS65913_RTC_CTRL_GET_TIME	= (1 << 6),

	TPS65913_RTC_STATUS_RUN	= (1 << 1),
	TPS65913_RTC_RUNNING		= (1 << 1),
	TPS65913_RTC_FROZEN		= (0 << 1),
};

static inline uint8_t tps65913_read(enum TPS65913_RTC_REG reg)
{
	uint8_t val;
	i2c_readb(CONFIG_DRIVERS_TI_TPS65913_RTC_BUS,
		  CONFIG_DRIVERS_TI_TPS65913_RTC_ADDR, reg, &val);
	return val;
}

static inline void tps65913_write(enum TPS65913_RTC_REG reg, uint8_t val)
{
	i2c_writeb(CONFIG_DRIVERS_TI_TPS65913_RTC_BUS,
		   CONFIG_DRIVERS_TI_TPS65913_RTC_ADDR, reg, val);
}

static void tps65913_rtc_ctrl_clear(uint8_t bit)
{
	uint8_t control = tps65913_read(TPS65913_RTC_CTRL_REG);

	control &= ~bit;
	tps65913_write(TPS65913_RTC_CTRL_REG, control);
}

static void tps65913_rtc_ctrl_set(uint8_t bit)
{
	uint8_t control = tps65913_read(TPS65913_RTC_CTRL_REG);

	control |= TPS65913_RTC_CTRL_GET_TIME;
	tps65913_write(TPS65913_RTC_CTRL_REG, control);
}

static int tps65913_is_rtc_running(void)
{
	uint8_t status = tps65913_read(TPS65913_RTC_STATUS_REG);
	return ((status & TPS65913_RTC_STATUS_RUN) == TPS65913_RTC_RUNNING);
}

/*
 * This function ensures that current time is copied to shadow registers. Then a
 * normal read on TC registers reads from the shadow instead of current TC
 * registers. This helps prevent the accidental change in counters while
 * reading. In order to ensure that the current TC registers are copied into
 * shadow registers, GET_TIME bit needs to be set to 0 and then to 1.
 */
static void tps65913_rtc_shadow(void)
{
	tps65913_rtc_ctrl_clear(TPS65913_RTC_CTRL_GET_TIME);
	tps65913_rtc_ctrl_set(TPS65913_RTC_CTRL_GET_TIME);
}

static int tps65913_rtc_stop(void)
{
	/* Clearing stop bit freezes RTC */
	tps65913_rtc_ctrl_clear(TPS65913_RTC_CTRL_STOP);

	if (tps65913_is_rtc_running()) {
		printk(BIOS_ERR, "Could not stop RTC\n");
		return 1;
	}

	return 0;
}

static int tps65913_rtc_start(void)
{
	/* Setting stop bit starts RTC */
	tps65913_rtc_ctrl_set(TPS65913_RTC_CTRL_STOP);

	if (!tps65913_is_rtc_running()) {
		printk(BIOS_ERR, "Could not start RTC\n");
		return 1;
	}

	return 0;
}

int rtc_set(const struct rtc_time *time)
{
	/* Before setting the time, ensure that rtc is stopped */
	if (tps65913_rtc_stop())
		return 1;

	tps65913_write(TPS65913_SECONDS_REG, bin2bcd(time->sec));
	tps65913_write(TPS65913_MINUTES_REG, bin2bcd(time->min));
	tps65913_write(TPS65913_HOURS_REG, bin2bcd(time->hour));
	tps65913_write(TPS65913_DAYS_REG, bin2bcd(time->mday));
	tps65913_write(TPS65913_MONTHS_REG, bin2bcd(time->mon));
	tps65913_write(TPS65913_YEARS_REG, bin2bcd(time->year));

	/* Re-start rtc */
	if (tps65913_rtc_start())
		return 1;

	return 0;
}

int rtc_get(struct rtc_time *time)
{
	tps65913_rtc_shadow();

	time->sec = bcd2bin(tps65913_read(TPS65913_SECONDS_REG) & 0x7f);
	time->min = bcd2bin(tps65913_read(TPS65913_MINUTES_REG) & 0x7f);
	time->hour = bcd2bin(tps65913_read(TPS65913_HOURS_REG) & 0x3f);
	time->mday = bcd2bin(tps65913_read(TPS65913_DAYS_REG) & 0x3f);
	time->mon = bcd2bin(tps65913_read(TPS65913_MONTHS_REG) & 0x1f);
	time->year = bcd2bin(tps65913_read(TPS65913_YEARS_REG) & 0xff);

	return 0;
}
