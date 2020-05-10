/* SPDX-License-Identifier: MIT */
/*
 * Copied from Linux drivers/gpu/drm/ast/ast_mode.c
 */
#include <delay.h>
#include <device/i2c_simple.h>

#include "ast_drv.h"

static struct ast_private *ast;

#define _GET_INDEX_REG(x) ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb7, (x))
#define ASPEED_BUS 0

static int get_clock(unsigned int bus)
{
	uint32_t val, val2, count, pass;

	count = 0;
	pass = 0;
	val = (_GET_INDEX_REG(0x10) >> 4) & 0x01;
	do {
		val2 = (_GET_INDEX_REG(0x10) >> 4) & 0x01;
		if (val == val2) {
			pass++;
		} else {
			pass = 0;
			val = (_GET_INDEX_REG(0x10) >> 4) & 0x01;
		}
	} while ((pass < 5) && (count++ < 0x10000));

	return val & 1 ? 1 : 0;
}

static int get_data(unsigned int bus)
{
	uint32_t val, val2, count, pass;

	count = 0;
	pass = 0;
	val = (_GET_INDEX_REG(0x20) >> 5) & 0x01;
	do {
		val2 = (_GET_INDEX_REG(0x20) >> 5) & 0x01;
		if (val == val2) {
			pass++;
		} else {
			pass = 0;
			val = (_GET_INDEX_REG(0x20) >> 5) & 0x01;
		}
	} while ((pass < 5) && (count++ < 0x10000));

	return val & 1 ? 1 : 0;
}

static void set_clock(unsigned int bus, int clock)
{
	int i;
	u8 ujcrb7, jtemp;

	for (i = 0; i < 0x10000; i++) {
		ujcrb7 = ((clock & 0x01) ? 0 : 1);
		ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb7, 0xf4, ujcrb7);
		jtemp = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb7, 0x01);
		if (ujcrb7 == jtemp)
			break;
	}
}

static void set_data(unsigned int bus, int data)
{
	int i;
	u8 ujcrb7, jtemp;

	for (i = 0; i < 0x10000; i++) {
		ujcrb7 = ((data & 0x01) ? 0 : 1) << 2;
		ast_set_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb7, 0xf1, ujcrb7);
		jtemp = ast_get_index_reg_mask(ast, AST_IO_CRTC_PORT, 0xb7, 0x04);
		if (ujcrb7 == jtemp)
			break;
	}
}

static struct software_i2c_ops ast_ops = {
	.set_sda = set_data,
	.set_scl = set_clock,
	.get_sda = get_data,
	.get_scl = get_clock,
};

int ast_software_i2c_read(struct ast_private *ast_priv, uint8_t edid[128])
{
	struct software_i2c_ops *backup;
	int ret;

	backup = software_i2c[ASPEED_BUS];

	software_i2c[ASPEED_BUS] = &ast_ops;

	ast = ast_priv;

	/* Ast POST pulled SDA and SCL low, recover the bus to a known state */
	set_clock(ASPEED_BUS, 1);
	set_data(ASPEED_BUS, 1);

	udelay(100);

	/* Need to reset internal EEPROM counter to 0 */
	ret = i2c_read_bytes(ASPEED_BUS, 0x50, 0, edid, 128);

	software_i2c[ASPEED_BUS] = backup;

	return ret;
}
