/*
 * This file is part of the coreboot project.
 *
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
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
#include <delay.h>
#include <arch/io.h>
#include <device/i2c.h>
#include "clk.h"
#include "i2c.h"

#define I2C_WRITE	0
#define I2C_READ	1

#define I2C_OK		0
#define I2C_NOK		1
#define I2C_NACK	2
#define I2C_NOK_LA	3	/* Lost arbitration */
#define I2C_NOK_TOUT	4	/* time out */

#define I2CSTAT_BSY	0x20	/* Busy bit */
#define I2CSTAT_NACK	0x01	/* Nack bit */
#define I2CCON_ACKGEN	0x80	/* Acknowledge generation */
#define I2CCON_IRPND	0x10	/* Interrupt pending bit */
#define I2C_MODE_MT	0xC0	/* Master Transmit Mode */
#define I2C_MODE_MR	0x80	/* Master Receive Mode */
#define I2C_START_STOP	0x20	/* START / STOP */
#define I2C_TXRX_ENA	0x10	/* I2C Tx/Rx enable */

/* The timeouts we live by */
enum {
	I2C_XFER_TIMEOUT_MS	= 35,	/* xfer to complete */
	I2C_INIT_TIMEOUT_MS	= 1000,	/* bus free on init */
	I2C_IDLE_TIMEOUT_MS	= 100,	/* waiting for bus idle */
	I2C_STOP_TIMEOUT_US	= 200,	/* waiting for stop events */
};

static struct s3c24x0_i2c_bus i2c_buses[] = {
	{
		.bus_num = 0,
		.regs = (struct s3c24x0_i2c *)0x12c60000,
		.periph_id = PERIPH_ID_I2C0,
	},
	{
		.bus_num = 1,
		.regs = (struct s3c24x0_i2c *)0x12c70000,
		.periph_id = PERIPH_ID_I2C1,
	},
	{
		.bus_num = 2,
		.regs = (struct s3c24x0_i2c *)0x12c80000,
		.periph_id = PERIPH_ID_I2C2,
	},
	{
		.bus_num = 3,
		.regs = (struct s3c24x0_i2c *)0x12c90000,
		.periph_id = PERIPH_ID_I2C3,
	},
	{
		.bus_num = 4,
		.regs = (struct s3c24x0_i2c *)0x12ca0000,
		.periph_id = PERIPH_ID_I2C4,
	},
	{
		.bus_num = 5,
		.regs = (struct s3c24x0_i2c *)0x12cb0000,
		.periph_id = PERIPH_ID_I2C5,
	},
	{
		.bus_num = 6,
		.regs = (struct s3c24x0_i2c *)0x12cc0000,
		.periph_id = PERIPH_ID_I2C6,
	},
	{
		.bus_num = 7,
		.regs = (struct s3c24x0_i2c *)0x12cd0000,
		.periph_id = PERIPH_ID_I2C7,
	},
};

static int WaitForXfer(struct s3c24x0_i2c *i2c)
{
	int i;

	i = I2C_XFER_TIMEOUT_MS * 20;
	while (!(readl(&i2c->iiccon) & I2CCON_IRPND)) {
		if (i == 0) {
			printk(BIOS_ERR, "%s: i2c xfer timeout\n", __func__);
			return I2C_NOK_TOUT;
		}
		udelay(50);
		i--;
	}

	return I2C_OK;
}

static int IsACK(struct s3c24x0_i2c *i2c)
{
	return !(readl(&i2c->iicstat) & I2CSTAT_NACK);
}

static void ReadWriteByte(struct s3c24x0_i2c *i2c)
{
	uint32_t x;

	x = readl(&i2c->iiccon);
	writel(x & ~I2CCON_IRPND, &i2c->iiccon);
}

static void i2c_ch_init(struct s3c24x0_i2c_bus *bus, int speed, int slaveadd)
{
	unsigned long freq, pres = 16, div;
	unsigned long val;

	freq = clock_get_periph_rate(bus->periph_id);
	/* calculate prescaler and divisor values */
	if ((freq / pres / (16 + 1)) > speed)
		/* set prescaler to 512 */
		pres = 512;

	div = 0;

	while ((freq / pres / (div + 1)) > speed)
		div++;

	/* set prescaler, divisor according to freq, also set ACKGEN, IRQ */
	val = (div & 0x0F) | 0xA0 | ((pres == 512) ? 0x40 : 0);
	writel(val, &bus->regs->iiccon);

	/* init to SLAVE RECEIVE mode and clear I2CADDn */
	writel(0, &bus->regs->iicstat);
	writel(slaveadd, &bus->regs->iicadd);
	/* program Master Transmit (and implicit STOP) */
	writel(I2C_MODE_MT | I2C_TXRX_ENA, &bus->regs->iicstat);
}

/*
 * MULTI BUS I2C support
 */
static void i2c_bus_init(struct s3c24x0_i2c_bus *bus, int speed, int slaveadd)
{
	i2c_ch_init(bus, speed, slaveadd);
}

/*
 * Verify the whether I2C ACK was received or not
 *
 * @param i2c	pointer to I2C register base
 * @param buf	array of data
 * @param len	length of data
 * return	I2C_OK when transmission done
 *		I2C_NACK otherwise
 */
static int i2c_send_verify(struct s3c24x0_i2c *i2c, unsigned char buf[],
			   unsigned char len)
{
	int i, result = I2C_OK;

	if (IsACK(i2c)) {
		for (i = 0; (i < len) && (result == I2C_OK); i++) {
			writel(buf[i], &i2c->iicds);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
			if (result == I2C_OK && !IsACK(i2c))
				result = I2C_NACK;
		}
	} else {
		result = I2C_NACK;
	}

	return result;
}

void i2c_init(unsigned bus_num, int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c;
	int i;

	i2c = &i2c_buses[bus_num];
	i2c_bus_init(i2c, speed, slaveadd);

	/* wait for some time to give previous transfer a chance to finish */
	i = I2C_INIT_TIMEOUT_MS * 20;
	while ((readl(&i2c->regs->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		udelay(50);
		i--;
	}

	i2c_ch_init(i2c, speed, slaveadd);
}

/*
 * Send a STOP event and wait for it to have completed
 *
 * @param mode	If it is a master transmitter or receiver
 * @return I2C_OK if the line became idle before timeout I2C_NOK_TOUT otherwise
 */
static int i2c_send_stop(struct s3c24x0_i2c *i2c, int mode)
{
	int timeout;

	/* Setting the STOP event to fire */
	writel(mode | I2C_TXRX_ENA, &i2c->iicstat);
	ReadWriteByte(i2c);

	/* Wait for the STOP to send and the bus to go idle */
	for (timeout = I2C_STOP_TIMEOUT_US; timeout > 0; timeout -= 5) {
		if (!(readl(&i2c->iicstat) & I2CSTAT_BSY))
			return I2C_OK;
		udelay(5);
	}

	return I2C_NOK_TOUT;
}

/*
 * cmd_type is 0 for write, 1 for read.
 *
 * addr_len can take any value from 0-255, it is only limited
 * by the char, we could make it larger if needed. If it is
 * 0 we skip the address write cycle.
 */
static int i2c_transfer(struct s3c24x0_i2c *i2c,
			unsigned char cmd_type,
			unsigned char chip,
			unsigned char addr[],
			unsigned char addr_len,
			unsigned char data[],
			unsigned short data_len)
{
	int i, result, stop_bit_result;
	uint32_t x;

	if (data == 0 || data_len == 0) {
		/* Don't support data transfer of no length or to address 0 */
		printk(BIOS_ERR, "i2c_transfer: bad call\n");
		return I2C_NOK;
	}

	/* Check I2C bus idle */
	i = I2C_IDLE_TIMEOUT_MS * 20;
	while ((readl(&i2c->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		udelay(50);
		i--;
	}

	if (readl(&i2c->iicstat) & I2CSTAT_BSY) {
		printk(BIOS_ERR, "%s: bus busy\n", __func__);
		return I2C_NOK_TOUT;
	}

	x = readl(&i2c->iiccon);
	writel(x | I2CCON_ACKGEN, &i2c->iiccon);

	if (addr && addr_len) {
		writel(chip, &i2c->iicds);
		/* send START */
		writel(I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP,
			&i2c->iicstat);
		if (WaitForXfer(i2c) == I2C_OK)
			result = i2c_send_verify(i2c, addr, addr_len);
		else
			result = I2C_NACK;
	} else
		result = I2C_NACK;

	switch (cmd_type) {
	case I2C_WRITE:
		if (result == I2C_OK)
			result = i2c_send_verify(i2c, data, data_len);
		else {
			writel(chip, &i2c->iicds);
			/* send START */
			writel(I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP,
				&i2c->iicstat);
			if (WaitForXfer(i2c) == I2C_OK)
				result = i2c_send_verify(i2c, data, data_len);
		}

		if (result == I2C_OK)
			result = WaitForXfer(i2c);

		stop_bit_result = i2c_send_stop(i2c, I2C_MODE_MT);
		break;

	case I2C_READ:
	{
		int was_ok = (result == I2C_OK);

		writel(chip, &i2c->iicds);
		/* resend START */
		writel(I2C_MODE_MR | I2C_TXRX_ENA |
					I2C_START_STOP, &i2c->iicstat);
		ReadWriteByte(i2c);
		result = WaitForXfer(i2c);

		if (was_ok || IsACK(i2c)) {
			i = 0;
			while ((i < data_len) && (result == I2C_OK)) {
				/* disable ACK for final READ */
				if (i == data_len - 1) {
					x = readl(&i2c->iiccon) & ~I2CCON_ACKGEN;
					writel(x, &i2c->iiccon);
				}
				ReadWriteByte(i2c);
				result = WaitForXfer(i2c);
				data[i] = readl(&i2c->iicds);
				i++;
			}
		} else {
			result = I2C_NACK;
		}

		stop_bit_result = i2c_send_stop(i2c, I2C_MODE_MR);
		break;
	}

	default:
		printk(BIOS_ERR, "i2c_transfer: bad call\n");
		result = stop_bit_result = I2C_NOK;
		break;
	}

	/*
	 * If the transmission went fine, then only the stop bit was left to
	 * fail.  Otherwise, the real failure we're interested in came before
	 * that, during the actual transmission.
	 */
	return (result == I2C_OK) ? stop_bit_result : result;
}

int i2c_read(unsigned bus, unsigned chip, unsigned addr,
		unsigned alen, uint8_t *buf, unsigned len)
{
	struct s3c24x0_i2c_bus *i2c;
	unsigned char xaddr[4];
	int ret;

	if (alen > 4) {
		printk(BIOS_ERR, "I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}

	i2c = &i2c_buses[bus];
	ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1, &xaddr[4 - alen],
			   alen, buf, len);
	if (ret) {
		printk(BIOS_ERR, "I2c read: failed %d\n", ret);
		return 1;
	}
	return 0;
}

int i2c_write(unsigned bus, unsigned chip, unsigned addr,
		unsigned alen, const uint8_t *buf, unsigned len)
{
	struct s3c24x0_i2c_bus *i2c;
	unsigned char xaddr[4];
	int ret;

	if (alen > 4) {
		printk(BIOS_ERR, "I2C write: addr len %d not supported\n",
				alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}

	i2c = &i2c_buses[bus];
	ret = i2c_transfer(i2c->regs, I2C_WRITE, chip << 1, &xaddr[4 - alen],
			   alen, (void *)buf, len);

	return ret != 0;
}
