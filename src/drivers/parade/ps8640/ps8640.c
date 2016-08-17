/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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

#include <delay.h>
#include <device/i2c.h>
#include <edid.h>
#include <console/console.h>
#include <timer.h>

#include "ps8640.h"

int ps8640_get_edid(uint8_t bus, uint8_t chip, struct edid *out)
{
	int ret;
	u8 edid[EDID_LENGTH * 2];
	int edid_size = EDID_LENGTH;

	i2c_writeb(bus, chip + 2, PAGE2_I2C_BYPASS,
		   EDID_I2C_ADDR | I2C_BYPASS_EN);
	ret = i2c_read_bytes(bus, EDID_I2C_ADDR, 0, edid, EDID_LENGTH);

	if (ret != 0) {
		printk(BIOS_INFO, "Failed to read EDID.\n");
		return -1;
	}

	/* check if edid have extension flag, and read additional EDID data */
	if (edid[EDID_EXTENSION_FLAG]) {
		edid_size += EDID_LENGTH;
		ret = i2c_read_bytes(bus, EDID_I2C_ADDR, EDID_LENGTH,
				     &edid[EDID_LENGTH], EDID_LENGTH);
		if (ret != 0) {
			printk(BIOS_INFO, "Failed to read EDID ext block.\n");
			return -1;
		}
	}

	if (decode_edid(edid, edid_size, out)) {
		printk(BIOS_INFO, "Failed to decode EDID.\n");
		return -1;
	}

	return 0;
}

int ps8640_init(uint8_t bus, uint8_t chip)
{
	u8 set_vdo_done;
	struct stopwatch sw;

	mdelay(200);
	stopwatch_init_msecs_expire(&sw, 200);

	while (true) {
		i2c_readb(bus, chip + 2, PAGE2_GPIO_H, &set_vdo_done);
		if ((set_vdo_done & PS_GPIO9) == PS_GPIO9)
			break;
		if (stopwatch_expired(&sw)) {
			printk(BIOS_INFO, "Failed to init ps8640.\n");
			return -1;
		}

		mdelay(20);
	}

	mdelay(50);
	i2c_writeb(bus, chip + 3, PAGE3_SET_ADD, VDO_CTL_ADD);
	i2c_writeb(bus, chip + 3, PAGE3_SET_VAL, VDO_DIS);
	i2c_writeb(bus, chip + 3, PAGE3_SET_ADD, VDO_CTL_ADD);
	i2c_writeb(bus, chip + 3, PAGE3_SET_VAL, VDO_EN);

	return 0;
}
