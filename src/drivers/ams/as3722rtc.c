/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <bcd.h>
#include <device/i2c_simple.h>
#include <rtc.h>
#include <stdint.h>

enum AS3722_RTC_REG
{
	AS3722_RTC_CONTROL = 0x60,
	AS3722_RTC_SECOND = 0x61,
	AS3722_RTC_MINUTE = 0x62,
	AS3722_RTC_HOUR = 0x63,
	AS3722_RTC_DAY = 0x64,
	AS3722_RTC_MONTH = 0x65,
	AS3722_RTC_YEAR = 0x66
};

enum {
	AS3722_RTC_CONTROL_ON = 0x1 << 2
};

static uint8_t as3722_read(enum AS3722_RTC_REG reg)
{
	uint8_t val;
	i2c_readb(CONFIG_DRIVERS_AS3722_RTC_BUS,
		  CONFIG_DRIVERS_AS3722_RTC_ADDR, reg, &val);
	return val;
}

static void as3722_write(enum AS3722_RTC_REG reg, uint8_t val)
{
	i2c_writeb(CONFIG_DRIVERS_AS3722_RTC_BUS,
		   CONFIG_DRIVERS_AS3722_RTC_ADDR, reg, val);
}

static void as3722rtc_init(void)
{
	static int initialized;
	if (initialized)
		return;

	uint8_t control = as3722_read(AS3722_RTC_CONTROL);
	as3722_write(AS3722_RTC_CONTROL, control | AS3722_RTC_CONTROL_ON);

	initialized = 1;
}

int rtc_set(const struct rtc_time *time)
{
	as3722rtc_init();

	as3722_write(AS3722_RTC_SECOND, bin2bcd(time->sec));
	as3722_write(AS3722_RTC_MINUTE, bin2bcd(time->min));
	as3722_write(AS3722_RTC_HOUR, bin2bcd(time->hour));
	as3722_write(AS3722_RTC_DAY, bin2bcd(time->mday));
	as3722_write(AS3722_RTC_MONTH, bin2bcd(time->mon));
	as3722_write(AS3722_RTC_YEAR, bin2bcd(time->year));
	return 0;
}

int rtc_get(struct rtc_time *time)
{
	as3722rtc_init();

	time->sec = bcd2bin(as3722_read(AS3722_RTC_SECOND) & 0x7f);
	time->min = bcd2bin(as3722_read(AS3722_RTC_MINUTE) & 0x7f);
	time->hour = bcd2bin(as3722_read(AS3722_RTC_HOUR) & 0x3f);
	time->mday = bcd2bin(as3722_read(AS3722_RTC_DAY) & 0x3f);
	time->mon = bcd2bin(as3722_read(AS3722_RTC_MONTH) & 0x1f);
	time->year = bcd2bin(as3722_read(AS3722_RTC_YEAR) & 0x7f);
	return 0;
}
