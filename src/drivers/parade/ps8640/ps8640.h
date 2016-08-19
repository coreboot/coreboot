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

#include <edid.h>
#include <types.h>

#ifndef _PS8640_H_
#define _PS8640_H_

enum {
	PAGE2_GPIO_L     = 0xa6,
	PAGE2_GPIO_H     = 0xa7,
	PAGE2_I2C_BYPASS = 0xea,
	PS_GPIO9         = BIT(1),
	I2C_BYPASS_EN    = BIT(7),

	PAGE2_MCS_EN     = 0xf3,
	MCS_EN_SHIFT     = 0,
	MCS_EN_MASK      = 0x1,
	PAGE3_SET_ADD    = 0xfe,
	PAGE3_SET_VAL    = 0xff,
	VDO_CTL_ADD      = 0x13,
	VDO_DIS          = 0x18,
	VDO_EN           = 0x1c,
};

enum {
	EDID_LENGTH         = 128,
	EDID_I2C_ADDR       = 0x50,
	EDID_EXTENSION_FLAG = 0x7e,
};

int ps8640_init(uint8_t bus, uint8_t chip);
int ps8640_get_edid(uint8_t bus, uint8_t chip, struct edid *out);
#endif
