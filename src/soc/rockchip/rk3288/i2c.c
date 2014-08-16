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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <console/console.h>
#include <arch/io.h>
#include <stdlib.h>
#include <assert.h>

#include <delay.h>
#include <cbfs.h>
#include <string.h>
#include <device/i2c.h>

#include "addressmap.h"
#include "grf.h"
#include "soc.h"
#define RETRY_COUNT	3
/* 100000us = 100ms */
#define I2C_TIMEOUT_US	100000
#define I2C_BUS_MAX 6
#define I2C_NOACK	2
#define I2C_TIMEOUT	3

#define i2c_info(x...) do {if (0) printk(BIOS_DEBUG, x); } while (0)

struct rk3288_i2c_regs {
	u32 i2c_con;
	u32 i2c_clkdiv;
	u32 i2c_mrxaddr;
	u32 i2c_mrxraddr;
	u32 i2c_mtxcnt;
	u32 i2c_mrxcnt;
	u32 i2c_ien;
	u32 i2c_ipd;
	u32 i2c_fcnt;
	u32 reserved0[(0x100 - 0x24) / 4];
	u32 txdata[8];
	u32 reserved1[(0x200 - 0x120) / 4];
	u32 rxdata[8];
};

struct rk3288_i2c_regs *i2c_bus[] = {
	(struct rk3288_i2c_regs *)0xff650000,
	(struct rk3288_i2c_regs *)0xff140000,
	(struct rk3288_i2c_regs *)0xff660000,
	(struct rk3288_i2c_regs *)0xff150000,
	(struct rk3288_i2c_regs *)0xff160000,
	(struct rk3288_i2c_regs *)0xff170000,
};

/* Con register bits. */
#define I2C_ACT2NAK			(1<<6)
#define I2C_NAK				(1<<5)
#define I2C_STOP				(1<<4)
#define I2C_START			(1<<3)
#define I2C_MODE_TX			(0<<1)
#define I2C_MODE_TRX		(1<<1)
#define I2C_MODE_RX			(2<<1)
#define I2C_EN				(1<<0)

#define I2C_8BIT	(1<<24)
#define I2C_16BIT	(3<<24)
#define I2C_24BIT	(7<<24)

/* Mtxcnt register bits. */
#define I2C_CNT(cnt)		((cnt) & 0x3F)

#define I2C_NAKRCVI	(1<<6)
#define I2C_STOPI	(1<<5)
#define I2C_STARTI	(1<<4)
#define I2C_MBRFI	(1<<3)
#define I2C_MBTFI	(1<<2)
#define I2C_BRFI	(1<<1)
#define I2C_BTFI	(1<<0)
#define I2C_CLEANI	0x7F

static int i2c_send_start(struct rk3288_i2c_regs *reg_addr)
{
	int res = 0;
	int timeout = I2C_TIMEOUT_US;

	i2c_info("I2c Start::Send Start bit\n");
	writel(I2C_CLEANI, &reg_addr->i2c_ipd);
	writel(I2C_EN | I2C_START, &reg_addr->i2c_con);
	while (timeout--) {
		if (readl(&reg_addr->i2c_ipd) & I2C_STARTI)
			break;
		udelay(1);
	}

	if (timeout <= 0) {
		printk(BIOS_ERR, "I2C Start::Send Start Bit Timeout\n");
		res = I2C_TIMEOUT;
	}

	return res;
}

static int i2c_send_stop(struct rk3288_i2c_regs *reg_addr)
{
	int res = 0;
	int timeout = I2C_TIMEOUT_US;

	i2c_info("I2c Stop::Send Stop bit\n");
	writel(I2C_CLEANI, &reg_addr->i2c_ipd);
	writel(I2C_EN | I2C_STOP, &reg_addr->i2c_con);
	while (timeout--) {
		if (readl(&reg_addr->i2c_ipd) & I2C_STOPI)
			break;
		udelay(1);
	}
	writel(0, &reg_addr->i2c_con);
	if (timeout <= 0) {
		printk(BIOS_ERR, "I2C Stop::Send Stop Bit Timeout\n");
		res = I2C_TIMEOUT;
	}

	return res;
}

static int i2c_read(struct rk3288_i2c_regs *reg_addr, struct i2c_seg segment)
{
	int res = 0;
	uint8_t *data = segment.buf;
	int timeout = I2C_TIMEOUT_US;
	unsigned int bytes_remaining = segment.len;
	unsigned int bytes_transfered = 0;
	unsigned int words_transfered = 0;
	unsigned int rxdata = 0;
	unsigned int con = 0;
	unsigned int i, j;

	writel(I2C_8BIT | segment.chip << 1 | 1, &reg_addr->i2c_mrxaddr);
	writel(0, &reg_addr->i2c_mrxraddr);
	con = I2C_MODE_TRX | I2C_EN;
	while (bytes_remaining) {
		bytes_transfered = MIN(bytes_remaining, 32);
		bytes_remaining -= bytes_transfered;
		if (!bytes_remaining)
			con |= I2C_EN | I2C_NAK;
		words_transfered = ALIGN_UP(bytes_transfered, 4) / 4;

		writel(I2C_CLEANI, &reg_addr->i2c_ipd);
		writel(con, &reg_addr->i2c_con);
		writel(bytes_transfered, &reg_addr->i2c_mrxcnt);

		timeout = I2C_TIMEOUT_US;
		while (timeout--) {
			if (readl(&reg_addr->i2c_ipd) & I2C_NAKRCVI) {
				writel(0, &reg_addr->i2c_mrxcnt);
				writel(0, &reg_addr->i2c_con);
				return I2C_NOACK;
			}
			if (readl(&reg_addr->i2c_ipd) & I2C_MBRFI)
				break;
			udelay(1);
		}
		if (timeout <= 0) {
			printk(BIOS_ERR, "I2C Read::Recv Data Timeout\n");
			writel(0, &reg_addr->i2c_mrxcnt);
			writel(0, &reg_addr->i2c_con);
			return	I2C_TIMEOUT;
		}

		for (i = 0; i < words_transfered; i++) {
			rxdata = readl(&reg_addr->rxdata[i]);
			i2c_info("I2c Read::RXDATA[%d] = 0x%x\n", i, rxdata);
			for (j = 0; j < 4; j++) {
				if ((i * 4 + j) == bytes_transfered)
					break;
				*data++ = (rxdata >> (j * 8)) & 0xff;
			}
		}
		con = I2C_MODE_RX | I2C_EN;
	}
	return res;
}

static int i2c_write(struct rk3288_i2c_regs *reg_addr, struct i2c_seg segment)
{
	int res = 0;
	uint8_t *data = segment.buf;
	int timeout = I2C_TIMEOUT_US;
	int bytes_remaining = segment.len + 1;
	int bytes_transfered = 0;
	int words_transfered = 0;
	unsigned int i;
	unsigned int j = 1;
	u32 txdata = 0;

	txdata |= (segment.chip << 1);
	while (bytes_remaining) {
		bytes_transfered = MIN(bytes_remaining, 32);
		words_transfered = ALIGN_UP(bytes_transfered, 4) / 4;
		for (i = 0; i < words_transfered; i++) {
			do {
				if ((i * 4 + j) == bytes_transfered)
					break;
				txdata |= (*data++) << (j * 8);
			} while (++j < 4);
			writel(txdata, &reg_addr->txdata[i]);
			j = 0;
			i2c_info("I2c Write::TXDATA[%d] = 0x%x\n", i, txdata);
			txdata = 0;
		}

		writel(I2C_CLEANI, &reg_addr->i2c_ipd);
		writel(I2C_EN | I2C_MODE_TX, &reg_addr->i2c_con);
		writel(bytes_transfered, &reg_addr->i2c_mtxcnt);

		timeout = I2C_TIMEOUT_US;
		while (timeout--) {
			if (readl(&reg_addr->i2c_ipd) & I2C_NAKRCVI) {
				writel(0, &reg_addr->i2c_mtxcnt);
				writel(0, &reg_addr->i2c_con);
				return I2C_NOACK;
			}
			if (readl(&reg_addr->i2c_ipd) & I2C_MBTFI)
				break;
			udelay(1);
		}

		if (timeout <= 0) {
			printk(BIOS_ERR, "I2C Write::Send Data Timeout\n");
			writel(0, &reg_addr->i2c_mtxcnt);
			writel(0, &reg_addr->i2c_con);
			return	I2C_TIMEOUT;
		}

		bytes_remaining -= bytes_transfered;
	}
	return res;
}

static int i2c_do_xfer(void *reg_addr, struct i2c_seg segment)
{
	int res = 0;

	if (i2c_send_start(reg_addr))
		return I2C_TIMEOUT;
	if (segment.read)
		res = i2c_read(reg_addr, segment);
	else
		res = i2c_write(reg_addr, segment);
	return i2c_send_stop(reg_addr) || res;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int seg_count)
{
	int i;
	int res = 0;
	struct rk3288_i2c_regs *regs = i2c_bus[bus];
	struct i2c_seg  *seg = segments;

	for (i = 0; i < seg_count; i++, seg++) {
		res = i2c_do_xfer(regs, *seg);
		if (res)
			break;
	}
	return res;
}
