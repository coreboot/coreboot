/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c_simple.h>
#include <soc/addressmap.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <soc/i2c.h>
#include <soc/clock.h>
#include <stddef.h>
#include <stdint.h>

#define RETRY_COUNT	3
/* 100000us = 100ms */
#define I2C_TIMEOUT_US	100000
#define I2C_BUS_MAX	6
#define I2C_NOACK	2
#define I2C_TIMEOUT	3

#define i2c_info(x...) do {if (0) printk(BIOS_DEBUG, x); } while (0)

struct rk_i2c_regs {
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

static const uintptr_t i2c_bus[] = IC_BASES;

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

static int i2c_send_start(struct rk_i2c_regs *reg_addr)
{
	int res = 0;
	int timeout = I2C_TIMEOUT_US;

	i2c_info("I2c Start::Send Start bit\n");
	write32(&reg_addr->i2c_ipd, I2C_CLEANI);
	write32(&reg_addr->i2c_con, I2C_EN | I2C_START);
	while (timeout--) {
		if (read32(&reg_addr->i2c_ipd) & I2C_STARTI)
			break;
		udelay(1);
	}

	if (timeout <= 0) {
		printk(BIOS_ERR, "I2C Start::Send Start Bit Timeout\n");
		res = I2C_TIMEOUT;
	}

	return res;
}

static int i2c_send_stop(struct rk_i2c_regs *reg_addr)
{
	int res = 0;
	int timeout = I2C_TIMEOUT_US;

	i2c_info("I2c Stop::Send Stop bit\n");
	write32(&reg_addr->i2c_ipd, I2C_CLEANI);
	write32(&reg_addr->i2c_con, I2C_EN | I2C_STOP);
	while (timeout--) {
		if (read32(&reg_addr->i2c_ipd) & I2C_STOPI)
			break;
		udelay(1);
	}
	write32(&reg_addr->i2c_con, 0);
	if (timeout <= 0) {
		printk(BIOS_ERR, "I2C Stop::Send Stop Bit Timeout\n");
		res = I2C_TIMEOUT;
	}

	return res;
}

static int i2c_read(struct rk_i2c_regs *reg_addr, struct i2c_msg segment)
{
	int res = 0;
	uint8_t *data = segment.buf;
	unsigned int bytes_remaining = segment.len;
	unsigned int con = 0;

	write32(&reg_addr->i2c_mrxaddr, I2C_8BIT | segment.slave << 1 | 1);
	write32(&reg_addr->i2c_mrxraddr, 0);
	con = I2C_MODE_TRX | I2C_EN | I2C_ACT2NAK;
	while (bytes_remaining) {
		int timeout = CONFIG_I2C_TRANSFER_TIMEOUT_US;
		size_t size = MIN(bytes_remaining, 32);
		bytes_remaining -= size;
		if (!bytes_remaining)
			con |= I2C_EN | I2C_NAK;

		i2c_info("I2C Read::%zu bytes\n", size);
		write32(&reg_addr->i2c_ipd, I2C_CLEANI);
		write32(&reg_addr->i2c_con, con);
		write32(&reg_addr->i2c_mrxcnt, size);

		while (timeout--) {
			if (read32(&reg_addr->i2c_ipd) & I2C_NAKRCVI) {
				write32(&reg_addr->i2c_mrxcnt, 0);
				write32(&reg_addr->i2c_con, 0);
				return I2C_NOACK;
			}
			if (read32(&reg_addr->i2c_ipd) & I2C_MBRFI)
				break;
			udelay(1);
		}
		if (timeout <= 0) {
			printk(BIOS_ERR, "I2C Read::Recv Data Timeout\n");
			write32(&reg_addr->i2c_mrxcnt, 0);
			write32(&reg_addr->i2c_con, 0);
			return I2C_TIMEOUT;
		}

		buffer_from_fifo32(data, size, &reg_addr->rxdata, 4, 4);
		data += size;
		con = I2C_MODE_RX | I2C_EN | I2C_ACT2NAK;
	}
	return res;
}

static int i2c_write(struct rk_i2c_regs *reg_addr, struct i2c_msg segment)
{
	int res = 0;
	uint8_t *data = segment.buf;
	int bytes_remaining = segment.len + 1;

	/* Prepend one byte for the slave address to the transfer. */
	u32 prefix = segment.slave << 1;
	int prefsz = 1;

	while (bytes_remaining) {
		int timeout = CONFIG_I2C_TRANSFER_TIMEOUT_US;
		size_t size = MIN(bytes_remaining, 32);
		buffer_to_fifo32_prefix(data, prefix, prefsz, size,
					&reg_addr->txdata, 4, 4);
		data += size - prefsz;

		i2c_info("I2C Write::%zu bytes\n", size);
		write32(&reg_addr->i2c_ipd, I2C_CLEANI);
		write32(&reg_addr->i2c_con,
			I2C_EN | I2C_MODE_TX | I2C_ACT2NAK);
		write32(&reg_addr->i2c_mtxcnt, size);

		while (timeout--) {
			if (read32(&reg_addr->i2c_ipd) & I2C_NAKRCVI) {
				write32(&reg_addr->i2c_mtxcnt, 0);
				write32(&reg_addr->i2c_con, 0);
				return I2C_NOACK;
			}
			if (read32(&reg_addr->i2c_ipd) & I2C_MBTFI)
				break;
			udelay(1);
		}

		if (timeout <= 0) {
			printk(BIOS_ERR, "I2C Write::Send Data Timeout\n");
			write32(&reg_addr->i2c_mtxcnt, 0);
			write32(&reg_addr->i2c_con, 0);
			return I2C_TIMEOUT;
		}

		bytes_remaining -= size;
		prefsz = 0;
		prefix = 0;
	}
	return res;
}

static int i2c_do_xfer(void *reg_addr, struct i2c_msg segment)
{
	int res = 0;

	if (i2c_send_start(reg_addr))
		return I2C_TIMEOUT;
	if (segment.flags & I2C_M_RD)
		res = i2c_read(reg_addr, segment);
	else
		res = i2c_write(reg_addr, segment);
	return i2c_send_stop(reg_addr) || res;
}

int platform_i2c_transfer(unsigned int bus, struct i2c_msg *segments,
			  int seg_count)
{
	int i;
	int res = 0;
	struct rk_i2c_regs *regs = (struct rk_i2c_regs *)(i2c_bus[bus]);
	struct i2c_msg *seg = segments;

	for (i = 0; i < seg_count; i++, seg++) {
		res = i2c_do_xfer(regs, *seg);
		if (res)
			break;
	}
	return res;
}

void i2c_init(unsigned int bus, unsigned int hz)
{
	unsigned int clk_div;
	unsigned int divl;
	unsigned int divh;
	unsigned int i2c_src_clk;
	unsigned int i2c_clk;
	struct rk_i2c_regs *regs = (struct rk_i2c_regs *)(i2c_bus[bus]);

	i2c_src_clk = rkclk_i2c_clock_for_bus(bus);

	/* SCL Divisor = 8*(CLKDIVL + 1 + CLKDIVH + 1)
	   SCL = PCLK / SCLK Divisor */
	clk_div = DIV_ROUND_UP(i2c_src_clk, hz * 8);
	divh = clk_div * 3 / 7 - 1;
	divl = clk_div - divh - 2;
	i2c_clk = i2c_src_clk / (8 * (divl + 1 + divh + 1));
	printk(BIOS_DEBUG, "I2C bus %u: %uHz (divh = %u, divl = %u)\n",
	       bus, i2c_clk, divh, divl);
	assert((divh < 65536) && (divl < 65536) && hz - i2c_clk < 15*KHz);
	write32(&regs->i2c_clkdiv, (divh << 16) | (divl << 0));
}
