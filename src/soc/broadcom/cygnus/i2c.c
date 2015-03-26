/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
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
 * Foundation, Inc.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/i2c.h>
#include <soc/i2c.h>

struct cygnus_i2c_regs {
	u32 i2c_con;
	u32 i2c_timing_con;
	u32 i2c_addr;
	u32 i2c_fifo_master;
	u32 i2c_fifo_slave;
	u32 i2c_bit_bang;
	u32 reserved0[(0x30 - 0x18) / 4];
	u32 i2c_master_comm;
	u32 i2c_slave_comm;
	u32 i2c_int_en;
	u32 i2c_int_status;
	u32 i2c_master_data_wr;
	u32 i2c_master_data_rd;
	u32 i2c_slave_data_wr;
	u32 i2c_slave_data_rd;
	u32 reserved1[(0xb0 - 0x50) / 4];
	u32 i2c_timing_con2;
};

static struct cygnus_i2c_regs *i2c_bus[] = {
	(struct cygnus_i2c_regs *)0x18008000,
	(struct cygnus_i2c_regs *)0x1800b000,
};

#define I2C_TIMEOUT_US	100000 /* 100ms */
#define I2C_FIFO_MAX_SIZE 64

#define ETIMEDOUT 1
#define EINVAL 2
#define EBUSY 3

/* Configuration (0x0) */
#define I2C_SMB_RESET (1 << 31)
#define I2C_SMB_EN (1 << 30)

/* Timing configuration (0x4) */
#define I2C_MODE_400 (1 << 31)

/* Master FIFO control (0xc) */
#define I2C_MASTER_RX_FIFO_FLUSH (1 << 31)
#define I2C_MASTER_TX_FIFO_FLUSH (1 << 30)

/* Master command (0x30) */
#define I2C_MASTER_START_BUSY (1 << 31)
#define I2C_MASTER_STATUS_SFT 25
#define I2C_MASTER_STATUS_MASK (0x7 << I2C_MASTER_STATUS_SFT)
#define I2C_MASTER_PROT_SFT   9
#define I2C_MASTER_PROT_BLK_WR (0x7 << I2C_MASTER_PROT_SFT)
#define I2C_MASTER_PROT_BLK_RD (0x8 << I2C_MASTER_PROT_SFT)

/* Master data write (0x40) */
#define I2C_MASTER_WR_STATUS (1 << 31)

/* Master data read (0x44) */
#define I2C_MASTER_RD_DATA_MASK 0xff

static unsigned int i2c_bus_busy(struct cygnus_i2c_regs *reg_addr)
{
	return read32(&reg_addr->i2c_master_comm) & I2C_MASTER_START_BUSY;
}

static int i2c_wait_bus_busy(struct cygnus_i2c_regs *reg_addr)
{
	int timeout = I2C_TIMEOUT_US;
	while (timeout--) {
		if (!i2c_bus_busy(reg_addr))
			break;
		udelay(1);
	}

	if (timeout <= 0)
		return ETIMEDOUT;

	return 0;
}

static void i2c_flush_fifo(struct cygnus_i2c_regs *reg_addr)
{
	write32(&reg_addr->i2c_fifo_master,
		I2C_MASTER_RX_FIFO_FLUSH | I2C_MASTER_TX_FIFO_FLUSH);
}

static int i2c_write(struct cygnus_i2c_regs *reg_addr, struct i2c_seg *segment)
{
	uint8_t *data = segment->buf;
	unsigned int val, status;
	int i, ret;

	write32(&reg_addr->i2c_master_data_wr, segment->chip << 1);

	for (i = 0; i < segment->len; i++) {
		val = data[i];

		/* mark the last byte */
		if (i == segment->len - 1)
			val |= I2C_MASTER_WR_STATUS;

		write32(&reg_addr->i2c_master_data_wr, val);
	}
	if (segment->len == 0)
		write32(&reg_addr->i2c_master_data_wr, I2C_MASTER_WR_STATUS);

	/*
	 * Now we can activate the transfer.
	 */
	write32(&reg_addr->i2c_master_comm,
		I2C_MASTER_START_BUSY | I2C_MASTER_PROT_BLK_WR);

	ret = i2c_wait_bus_busy(reg_addr);
	if (ret) {
		printk(BIOS_ERR, "I2C bus timeout\n");
		goto flush_fifo;
	}

	/* check transaction successful */
	status = read32(&reg_addr->i2c_master_comm);
	ret = (status & I2C_MASTER_STATUS_MASK) >> I2C_MASTER_STATUS_SFT;
	if (ret) {
		printk(BIOS_ERR, "I2C write error %u\n", status);
		goto flush_fifo;
	}

	return 0;

flush_fifo:
	i2c_flush_fifo(reg_addr);
	return ret;
}

static int i2c_read(struct cygnus_i2c_regs *reg_addr, struct i2c_seg *segment)
{
	uint8_t *data = segment->buf;
	int i, ret;
	unsigned int status;

	write32(&reg_addr->i2c_master_data_wr, segment->chip << 1 | 1);

	/*
	 * Now we can activate the transfer. Specify the number of bytes to read
	 */
	write32(&reg_addr->i2c_master_comm,
		I2C_MASTER_START_BUSY | I2C_MASTER_PROT_BLK_RD | segment->len);

	ret = i2c_wait_bus_busy(reg_addr);
	if (ret) {
		printk(BIOS_ERR, "I2C bus timeout\n");
		goto flush_fifo;
	}

	/* check transaction successful */
	status = read32(&reg_addr->i2c_master_comm);
	ret = (status & I2C_MASTER_STATUS_MASK) >> I2C_MASTER_STATUS_SFT;
	if (ret) {
		printk(BIOS_ERR, "I2C read error %u\n", status);
		goto flush_fifo;
	}

	for (i = 0; i < segment->len; i++)
		data[i] = read32(&reg_addr->i2c_master_data_rd) &
			I2C_MASTER_RD_DATA_MASK;

	return 0;

flush_fifo:
	i2c_flush_fifo(reg_addr);
	return ret;
}

static int i2c_do_xfer(struct cygnus_i2c_regs *reg_addr,
	struct i2c_seg *segment)
{
	int ret;

	if (segment->len > I2C_FIFO_MAX_SIZE - 1) {
		printk(BIOS_ERR,
			"I2C transfer error: segment size (%d) is larger than limit (%d)\n",
			segment->len, I2C_FIFO_MAX_SIZE);
		return EINVAL;
	}

	if (i2c_bus_busy(reg_addr)) {
		printk(BIOS_WARNING, "I2C transfer error: bus is busy\n");
		return EBUSY;
	}

	if (segment->read)
		ret = i2c_read(reg_addr, segment);
	else
		ret = i2c_write(reg_addr, segment);

	return ret;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int seg_count)
{
	int i;
	int res = 0;
	struct cygnus_i2c_regs *regs = i2c_bus[bus];
	struct i2c_seg *seg = segments;

	for (i = 0; i < seg_count; i++, seg++) {
		res = i2c_do_xfer(regs, seg);
		if (res)
			break;
	}
	return res;
}

void i2c_init(unsigned int bus, unsigned int hz)
{
	struct cygnus_i2c_regs *regs = i2c_bus[bus];

	assert(bus >= 0 && bus <= 1);

	setbits_le32(&regs->i2c_con, I2C_SMB_RESET);
	udelay(100); /* wait 100 usec per spec */
	clrbits_le32(&regs->i2c_con, I2C_SMB_RESET);

	switch (hz) {
	case 100000:
		clrbits_le32(&regs->i2c_timing_con, I2C_MODE_400);
		break;
	case 400000:
		setbits_le32(&regs->i2c_timing_con, I2C_MODE_400);
		break;
	default:
		printk(BIOS_ERR, "I2C bus does not support frequency %d Hz\n",
			hz);
		break;
	}

	i2c_flush_fifo(regs);

	/* disable all interrupts */
	write32(&regs->i2c_int_en, 0);

	/* clear all pending interrupts */
	write32(&regs->i2c_int_status, 0xffffffff);

	write32(&regs->i2c_con, I2C_SMB_EN);
}
