/* SPDX-License-Identifier: GPL-2.0-only */

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
	PAGE0_AUXCH_CFG3 = 0x76,
	AUXCH_CFG3_RESET = 0xff,
	PAGE0_SWAUX_ADDR_7_0 = 0x7d,
	PAGE0_SWAUX_ADDR_15_8 = 0x7e,
	PAGE0_SWAUX_ADDR_23_16 = 0x7f,
	SWAUX_ADDR_MASK = 0xfffff,
	PAGE0_SWAUX_LENGTH = 0x80,
	SWAUX_LENGTH_MASK = 0xf,
	SWAUX_NO_PAYLOAD = BIT(7),
	PAGE0_SWAUX_WDATA = 0x81,
	PAGE0_SWAUX_RDATA = 0x82,
	PAGE0_SWAUX_CTRL = 0x83,
	SWAUX_SEND = BIT(0),
	PAGE0_SWAUX_STATUS = 0x84,
	SWAUX_M_MASK = 0x1f,
	SWAUX_STATUS_MASK = (0x7 << 5),
	SWAUX_STATUS_NACK = (0x1 << 5),
	SWAUX_STATUS_DEFER = (0x2 << 5),
	SWAUX_STATUS_ACKM = (0x3 << 5),
	SWAUX_STATUS_INVALID = (0x4 << 5),
	SWAUX_STATUS_I2C_NACK = (0x5 << 5),
	SWAUX_STATUS_I2C_DEFER = (0x6 << 5),
	SWAUX_STATUS_TIMEOUT = (0x7 << 5),
};

int ps8640_init(uint8_t bus, uint8_t chip);
int ps8640_get_edid(uint8_t bus, uint8_t chip, struct edid *out);
void ps8640_backlight_enable(uint8_t bus, uint8_t chip);

#endif
