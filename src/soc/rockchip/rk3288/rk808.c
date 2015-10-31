/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Rockchip Inc.
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

#include <assert.h>
#include <bcd.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <rtc.h>
#include <soc/rk808.h>
#include <stdint.h>
#include <stdlib.h>

#if CONFIG_PMIC_BUS < 0
#error "PMIC_BUS must be set in mainboard's Kconfig."
#endif

#define RK808_ADDR		0x1b

#define DCDC_EN			0x23
#define LDO_EN			0x24
#define BUCK1SEL		0x2f
#define BUCK4SEL		0x38
#define LDO_ONSEL(i)		(0x39 + 2 * i)
#define LDO_SLPSEL(i)		(0x3a + 2 * i)

#define RTC_SECOND		0x00
#define RTC_MINUTE		0x01
#define RTC_HOUR		0x02
#define RTC_DAY			0x03
#define RTC_MONTH		0x04
#define RTC_YEAR		0x05
#define RTC_WEEKS		0x06
#define RTC_CTRL		0x10
#define RTC_STATUS		0x11

#define RTC_CTRL_STOP_RTC	(1 << 0)
#define RTC_CTRL_GET_TIME	(1 << 6)
#define RTC_CTRL_RTC_READSEL	(1 << 7)

#define DCDC_UV_ACT		0x28
#define DCDC_ILMAX		0x90

static int rk808_read(uint8_t reg, uint8_t *value)
{
	return i2c_readb(CONFIG_PMIC_BUS, RK808_ADDR, reg, value);
}

static int rk808_write(uint8_t reg, uint8_t value)
{
	return i2c_writeb(CONFIG_PMIC_BUS, RK808_ADDR, reg, value);
}

static void rk808_clrsetbits(uint8_t reg, uint8_t clr, uint8_t set)
{
	uint8_t value;

	if (rk808_read(reg, &value) || rk808_write(reg, (value & ~clr) | set))
		printk(BIOS_ERR, "ERROR: Cannot set Rk808[%#x]!\n", reg);
}

void rk808_configure_switch(int sw, int enabled)
{
	assert(sw == 1 || sw == 2);
	rk808_clrsetbits(DCDC_EN, 1 << (sw + 4), !!enabled << (sw + 4));
}

void rk808_configure_ldo(int ldo, int millivolts)
{
	uint8_t vsel;

	if (!millivolts) {
		rk808_clrsetbits(LDO_EN, 1 << (ldo - 1), 0);
		return;
	}

	switch (ldo) {
	case 1:
	case 2:
	case 4:
	case 5:
	case 8:
		vsel = div_round_up(millivolts, 100) - 18;
		assert(vsel <= 0x10);
		break;
	case 3:
	case 6:
	case 7:
		vsel = div_round_up(millivolts, 100) - 8;
		assert(vsel <= 0x11);
		break;
	default:
		die("Unknown LDO index!");
	}

	rk808_clrsetbits(LDO_ONSEL(ldo), 0x1f, vsel);
	rk808_clrsetbits(LDO_EN, 0, 1 << (ldo - 1));
}

void rk808_configure_buck(int buck, int millivolts)
{
	uint8_t vsel;
	uint8_t buck_reg;

	switch (buck) {
	case 1:
	case 2:
		/* 25mV steps. base = 29 * 25mV = 725 */
		vsel = (div_round_up(millivolts, 25) - 29) * 2 + 1;
		assert(vsel <= 0x3f);
		buck_reg = BUCK1SEL + 4 * (buck - 1);
		break;
	case 4:
		vsel = div_round_up(millivolts, 100) - 18;
		assert(vsel <= 0xf);
		buck_reg = BUCK4SEL;
		break;
	default:
		die("Unknown buck index!");
	}
	rk808_clrsetbits(DCDC_ILMAX, 0, 3 << ((buck - 1) * 2));

	/* undervoltage detection may be wrong, disable it */
	rk808_clrsetbits(DCDC_UV_ACT, 1 << (buck - 1), 0);

	rk808_clrsetbits(buck_reg, 0x3f, vsel);
	rk808_clrsetbits(DCDC_EN, 0, 1 << (buck - 1));
}

static void rk808rtc_stop(void)
{
	rk808_clrsetbits(RTC_CTRL, RTC_CTRL_STOP_RTC, 0);
}

static void rk808rtc_start(void)
{
	rk808_clrsetbits(RTC_CTRL, 0, RTC_CTRL_STOP_RTC);
}

int rtc_set(const struct rtc_time *time)
{
	int ret = 0;

	/* RTC time can only be set when RTC is frozen */
	rk808rtc_stop();

	ret |= rk808_write(RTC_SECOND, bin2bcd(time->sec));
	ret |= rk808_write(RTC_MINUTE, bin2bcd(time->min));
	ret |= rk808_write(RTC_HOUR, bin2bcd(time->hour));
	ret |= rk808_write(RTC_DAY, bin2bcd(time->mday));
	ret |= rk808_write(RTC_MONTH, bin2bcd(time->mon));
	ret |= rk808_write(RTC_YEAR, bin2bcd(time->year));

	rk808rtc_start();
	return ret;
}

int rtc_get(struct rtc_time *time)
{
	uint8_t value;
	int ret = 0;

	/*
	 * Set RTC_READSEL to cause reads to access shadow registers and
	 * transition GET_TIME from 0 to 1 to cause dynamic register content
	 * to be copied into shadow registers. This ensures a coherent reading
	 * of time values as we access each register using slow I2C transfers.
	 */
	rk808_clrsetbits(RTC_CTRL, RTC_CTRL_GET_TIME, 0);
	rk808_clrsetbits(RTC_CTRL, 0, RTC_CTRL_GET_TIME | RTC_CTRL_RTC_READSEL);

	ret |= rk808_read(RTC_SECOND, &value);
	time->sec = bcd2bin(value & 0x7f);

	ret |= rk808_read(RTC_MINUTE, &value);
	time->min = bcd2bin(value & 0x7f);

	ret |= rk808_read(RTC_HOUR, &value);
	time->hour = bcd2bin(value & 0x3f);

	ret |= rk808_read(RTC_DAY, &value);
	time->mday = bcd2bin(value & 0x3f);

	ret |= rk808_read(RTC_MONTH, &value);
	time->mon = bcd2bin(value & 0x1f);

	ret |= rk808_read(RTC_YEAR, &value);
	time->year = bcd2bin(value);

	return ret;
}
