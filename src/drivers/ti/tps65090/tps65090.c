/*
 * Copyright (c) 2012 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 * Alternatively, this software may be distributed under the terms of the
 * GNU General Public License ("GPL") version 2 as published by the Free
 * Software Foundation.
 */

#include <common.h>
#include <fdtdec.h>
#include <i2c.h>
#include <tps65090.h>

DECLARE_GLOBAL_DATA_PTR;

/* TPS65090 register addresses */
enum {
	REG_CG_CTRL0 = 4,
	REG_CG_STATUS1 = 0xa,
	REG_FET1_CTRL = 0x0f,
	REG_FET2_CTRL,
	REG_FET3_CTRL,
	REG_FET4_CTRL,
	REG_FET5_CTRL,
	REG_FET6_CTRL,
	REG_FET7_CTRL,
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

static struct tps65090_config {
	int bus;
	int addr;
	int old_bus;
} config;

/**
 * Write a value to a register
 *
 * @param	chip_addr	i2c slave addr for max77686
 * @param	reg_addr	register address to write
 * @param	value		value to be written
 * @return	0 on success, non-0 on failure
 */
static int tps65090_i2c_write(unsigned int reg_addr, unsigned char value)
{
	int ret;

	ret = i2c_write(config.addr, reg_addr, 1, &value, 1);
	debug("%s: reg=%#x, value=%#x, ret=%d\n", __func__, reg_addr, value,
	      ret);
	return ret;
}

/**
 * Read a value from a register
 *
 * @param	chip_addr	i2c addr for max77686
 * @param	reg_addr	register address to read
 * @param	value		address to store the value to be read
 * @return	0 on success, non-0 on failure
 */
static int tps65090_i2c_read(unsigned int reg_addr, unsigned char *value)
{
	int ret;

	debug("%s: reg=%#x, ", __func__, reg_addr);
	ret = i2c_read(config.addr, reg_addr, 1, value, 1);
	if (ret)
		debug("fail, ret=%d\n", ret);
	else
		debug("value=%#x, ret=%d\n", *value, ret);
	return ret;
}

static int tps65090_select(void)
{
	int ret;

	config.old_bus = i2c_get_bus_num();
	if (config.old_bus != config.bus) {
		debug("%s: Select bus %d\n", __func__, config.bus);
		ret = i2c_set_bus_num(config.bus);
		if (ret) {
			debug("%s: Cannot select TPS65090, err %d\n",
			      __func__, ret);
			return -1;
		}
	}

	return 0;
}

static int tps65090_deselect(void)
{
	int ret;

	if (config.old_bus != i2c_get_bus_num()) {
		ret = i2c_set_bus_num(config.old_bus);
		debug("%s: Select bus %d\n", __func__, config.old_bus);
		if (ret) {
			debug("%s: Cannot restore i2c bus, err %d\n",
			      __func__, ret);
			return -1;
		}
	}
	config.old_bus = -1;
	return 0;
}

/**
 * Checks for a valid FET number
 *
 * @param fet_id	FET number to check
 * @return 0 if ok, -1 if FET value is out of range
 */
static int tps65090_check_fet(unsigned int fet_id)
{
	if (fet_id == 0 || fet_id > MAX_FET_NUM) {
		debug("parameter fet_id is out of range, %u not in 1 ~ %u\n",
				fet_id, MAX_FET_NUM);
		return -1;
	}

	return 0;
}

/**
 * Set the power state for a FET
 *
 * @param fet_id	Fet number to set (1..MAX_FET_NUM)
 * @param set		1 to power on FET, 0 to power off
 * @return FET_ERR_COMMS if we got a comms error, FET_ERR_NOT_READY if the
 * FET failed to change state. If all is ok, returns 0.
 */
static int tps65090_fet_set(int fet_id, int set)
{
	int retry, value;
	uchar reg;

	value = FET_CTRL_ADENFET | FET_CTRL_WAIT;
	if (set)
		value |= FET_CTRL_ENFET;

	if (tps65090_i2c_write(REG_FET1_CTRL + fet_id - 1, value))
		return FET_ERR_COMMS;
	/* Try reading until we get a result */
	for (retry = 0; retry < MAX_CTRL_READ_TRIES; retry++) {
		if (tps65090_i2c_read(REG_FET1_CTRL + fet_id - 1, &reg))
			return FET_ERR_COMMS;

		/* Check that the fet went into the expected state */
		if (!!(reg & FET_CTRL_PGFET) == set)
			return 0;

		/* If we got a timeout, there is no point in waiting longer */
		if (reg & FET_CTRL_TOFET)
			break;

		mdelay(1);
	}

	debug("FET %d: Power good should have set to %d but reg=%#02x\n",
	      fet_id, set, reg);
	return FET_ERR_NOT_READY;
}

int tps65090_fet_enable(unsigned int fet_id)
{
	int loops;
	ulong start;
	int ret = 0;

	if (tps65090_check_fet(fet_id))
		return -1;
	if (tps65090_select())
		return -1;
	start = get_timer(0);
	for (loops = 0; ; loops++) {
		ret = tps65090_fet_set(fet_id, 1);
		if (!ret)
			break;

		if (get_timer(start) > 100)
			break;

		/* Turn it off and try again until we time out */
		tps65090_fet_set(fet_id, 0);
	}
	tps65090_deselect();

	if (ret) {
		debug("%s: FET%d failed to power on: time=%lums, loops=%d\n",
	      __func__, fet_id, get_timer(start), loops);
	} else if (loops) {
		debug("%s: FET%d powered on after %lums, loops=%d\n",
		      __func__, fet_id, get_timer(start), loops);
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

int tps65090_fet_disable(unsigned int fet_id)
{
	int ret;

	if (tps65090_check_fet(fet_id))
		return -1;
	if (tps65090_select())
		return -1;
	ret = tps65090_fet_set(fet_id, 0);
	tps65090_deselect();

	return ret;
}

int tps65090_fet_is_enabled(unsigned int fet_id)
{
	unsigned char reg;
	int ret;

	if (tps65090_check_fet(fet_id))
		return -1;
	if (tps65090_select())
		return -1;
	ret = tps65090_i2c_read(REG_FET1_CTRL + fet_id - 1, &reg);
	tps65090_deselect();
	if (ret) {
		debug("fail to read FET%u_CTRL register over I2C", fet_id);
		return -2;
	}

	return reg & FET_CTRL_ENFET;
}

int tps65090_get_charging(void)
{
	unsigned char val;
	int ret;

	if (tps65090_select())
		return -1;
	ret = tps65090_i2c_read(REG_CG_CTRL0, &val);
	tps65090_deselect();
	if (ret)
		return ret;
	return val & CG_CTRL0_ENC_MASK ? 1 : 0;
}

int tps65090_set_charge_enable(int enable)
{
	unsigned char val;
	int ret;

	if (tps65090_select())
		return -1;
	ret = tps65090_i2c_read(REG_CG_CTRL0, &val);
	if (!ret) {
		if (enable)
			val |= CG_CTRL0_ENC_MASK;
		else
			val &= ~CG_CTRL0_ENC_MASK;
		ret = tps65090_i2c_write(REG_CG_CTRL0, val);
	}
	tps65090_deselect();
	if (ret) {
		debug("%s: Failed to read/write register\n", __func__);
		return ret;
	}
	return 0;
}

int tps65090_get_status(void)
{
	unsigned char val;
	int ret;

	if (tps65090_select())
		return -1;
	ret = tps65090_i2c_read(REG_CG_STATUS1, &val);
	tps65090_deselect();
	if (ret)
		return ret;
	return val;
}

static int tps65090_decode_config(struct tps65090_config *config)
{
#ifdef CONFIG_OF_CONTROL
	const void *blob = gd->fdt_blob;
	int node, parent;
	int i2c_bus;

	node = fdtdec_next_compatible(blob, 0, COMPAT_TI_TPS65090);
	if (node < 0) {
		debug("%s: Node not found\n", __func__);
		return -1;
	}
	parent = fdt_parent_offset(blob, node);
	if (parent < 0) {
		debug("%s: Cannot find node parent\n", __func__);
		return -1;
	}
	i2c_bus = i2c_get_bus_num_fdt(blob, parent);
	if (i2c_bus < 0)
		return -1;
	config->bus = i2c_bus;
	config->addr = fdtdec_get_addr(blob, node, "reg");
#else
	config->bus = CONFIG_TPS65090_I2C_BUS;
	config->addr = TPS65090_I2C_ADDR;
#endif
	return 0;
}

int tps65090_init(void)
{
	int ret;

	if (tps65090_decode_config(&config))
		return -1;

	config.old_bus = -1;

	if (tps65090_select())
		return -1;

	/* Probe the chip */
	ret = i2c_probe(config.addr);
	if (ret)
		debug("%s: failed to probe TPS65090 over I2C, returned %d\n",
		      __func__, ret);

	return ret;
}
