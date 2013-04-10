/*
 * Copyright 2012, Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above
 *      copyright notice, this list of conditions and the following
 *      disclaimer in the documentation and/or other materials provided
 *      with the distribution.
 *    * Neither the name of Google Inc. nor the names of its
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <delay.h>
#include <types.h>
#include <console/console.h>
#include <device/i2c.h>
#include "tps65090.h"

/* TPS65090 register addresses */
enum {
	REG_CG_CTRL0 = 4,
	REG_CG_STATUS1 = 0xa,
};

enum {
	CG_CTRL0_ENC_MASK	= 0x01,

	MAX_FET_NUM	= 7,
	MAX_CTRL_READ_TRIES = 5,

	/* TPS65090 FET_CTRL register values */
	FET_CTRL_TOFET		= 1 << 7,  /* Timeout, startup, overload */
	FET_CTRL_PGFET		= 1 << 4,  /* Power good for FET status */
	FET_CTRL_WAIT		= 3 << 2,  /* Overcurrent timeout max */
	FET_CTRL_ADENFET	= 1 << 1,  /* Enable output auto discharge */
	FET_CTRL_ENFET		= 1 << 0,  /* Enable FET */
};

static int tps65090_i2c_write(unsigned int bus,
		unsigned int reg_addr, unsigned char value)
{
	int ret;

	ret = i2c_write(bus, TPS65090_I2C_ADDR, reg_addr, 1, &value, 1);
	printk(BIOS_DEBUG, "%s: reg=%#x, value=%#x, ret=%d\n",
			__func__, reg_addr, value, ret);
	return ret;
}

static int tps65090_i2c_read(unsigned int bus,
		unsigned int reg_addr, unsigned char *value)
{
	int ret;

	printk(BIOS_DEBUG, "%s: reg=%#x, ", __func__, reg_addr);
	ret = i2c_read(bus, TPS65090_I2C_ADDR, reg_addr, 1, value, 1);
	if (ret)
		printk(BIOS_DEBUG, "fail, ret=%d\n", ret);
	else
		printk(BIOS_DEBUG, "value=%#x, ret=%d\n", *value, ret);
	return ret;
}

/**
 * Set the power state for a FET
 *
 * @fet_id		Fet number to set (1..MAX_FET_NUM)
 * @set			1 to power on FET, 0 to power off
 * @return FET_ERR_COMMS if we got a comms error, FET_ERR_NOT_READY if the
 * FET failed to change state. If all is ok, returns 0.
 */
static int tps65090_fet_set(unsigned int bus, enum fet_id fet_id, int set)
{
	int retry, value;
	uint8_t reg;

	value = FET_CTRL_ADENFET | FET_CTRL_WAIT;
	if (set)
		value |= FET_CTRL_ENFET;

	if (tps65090_i2c_write(bus, fet_id, value))
		return FET_ERR_COMMS;
	/* Try reading until we get a result */
	for (retry = 0; retry < MAX_CTRL_READ_TRIES; retry++) {
		if (tps65090_i2c_read(bus, fet_id, &reg))
			return FET_ERR_COMMS;

		/* Check that the fet went into the expected state */
		if (!!(reg & FET_CTRL_PGFET) == set)
			return 0;

		/* If we got a timeout, there is no point in waiting longer */
		if (reg & FET_CTRL_TOFET)
			break;

		udelay(1000);
	}

	printk(BIOS_DEBUG, "FET %d: Power good should have set to %d but "
			"reg=%#02x\n", fet_id, set, reg);
	return FET_ERR_NOT_READY;
}

/* FIXME(dhendrix): add timer API */
#if 0
int tps65090_fet_enable(unsigned int bus, enum fet_id fet_id)
{
	int loops;
	unsigned long start;
	int ret = 0;

	start = get_timer(0);
	for (loops = 0; ; loops++) {
		ret = tps65090_fet_set(bus, fet_id, 1);
		if (!ret)
			break;

		if (get_timer(start) > 100)
			break;

		/* Turn it off and try again until we time out */
		tps65090_fet_set(bus, fet_id, 0);
	}

	if (ret) {
		printk(BIOS_DEBUG, "%s: FET%d failed to power on: time=%lums, "
				"loops=%d\n", __func__, fet_id,
				get_timer(start), loops);
	} else if (loops) {
		printk(BIOS_DEBUG, "%s: FET%d powered on after %lums, "
				"loops=%d\n", __func__, fet_id,
				get_timer(start), loops);
	}
	/*
	 * Unfortunately, there are some conditions where the power
	 * good bit will be 0, but the fet still comes up. One such
	 * case occurs with the lcd backlight. We'll just return 0 here
	 * and assume that the fet will eventually come up.
	 */
	if (ret == FET_ERR_NOT_READY)
		ret = 0;

	return ret;
}
#endif
int tps65090_fet_enable(unsigned int bus, enum fet_id fet_id)
{
	int loops;
	int ret = 0;

	for (loops = 0; loops < 100; loops++) {
		ret = tps65090_fet_set(bus, fet_id, 1);
		if (!ret)
			break;

		/* Turn it off and try again until we time out */
		tps65090_fet_set(bus, fet_id, 0);
		udelay(1000);
	}

	if (ret) {
		printk(BIOS_DEBUG, "%s: FET%d failed to power on\n",
				__func__, fet_id);
	} else if (loops) {
		printk(BIOS_DEBUG, "%s: FET%d powered on\n",
				__func__, fet_id);
	}
	/*
	 * Unfortunately, there are some conditions where the power
	 * good bit will be 0, but the fet still comes up. One such
	 * case occurs with the lcd backlight. We'll just return 0 here
	 * and assume that the fet will eventually come up.
	 */
	if (ret == FET_ERR_NOT_READY)
		ret = 0;

	return ret;
}

int tps65090_fet_disable(unsigned int bus, enum fet_id fet_id)
{
	return tps65090_fet_set(bus, fet_id, 0);
}

int tps65090_fet_is_enabled(unsigned int bus, enum fet_id fet_id)
{
	unsigned char reg;
	int ret;

	ret = tps65090_i2c_read(bus, fet_id, &reg);
	if (ret) {
		printk(BIOS_DEBUG, "fail to read FET%u_CTRL", fet_id);
		return -2;
	}

	return reg & FET_CTRL_ENFET;
}

int tps65090_is_charging(unsigned int bus)
{
	unsigned char val;
	int ret;

	ret = tps65090_i2c_read(bus, REG_CG_CTRL0, &val);
	if (ret)
		return ret;
	return val & CG_CTRL0_ENC_MASK ? 1 : 0;
}

int tps65090_set_charge_enable(unsigned int bus, int enable)
{
	unsigned char val;
	int ret;

	ret = tps65090_i2c_read(bus, REG_CG_CTRL0, &val);
	if (!ret) {
		if (enable)
			val |= CG_CTRL0_ENC_MASK;
		else
			val &= ~CG_CTRL0_ENC_MASK;
		ret = tps65090_i2c_write(bus, REG_CG_CTRL0, val);
	}
	if (ret) {
		printk(BIOS_DEBUG, "%s: Failed to enable\n", __func__);
		return ret;
	}
	return 0;
}

int tps65090_get_status(unsigned int bus)
{
	unsigned char val;
	int ret;

	ret = tps65090_i2c_read(bus, REG_CG_STATUS1, &val);
	if (ret)
		return ret;
	return val;
}
