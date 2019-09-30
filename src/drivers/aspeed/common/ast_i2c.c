/*
 * Copied from Linux drivers/gpu/drm/ast/ast_mode.c
 *
 * Copyright 2012 Red Hat Inc.
 * Parts based on xf86-video-ast
 * Copyright (c) 2005 ASPEED Technology Inc.
 * Copyright Dave Airlie <airlied@redhat.com>
 * Copyright 2019 9Elements Agency GmbH <patrick.rudolph@9elements.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
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
