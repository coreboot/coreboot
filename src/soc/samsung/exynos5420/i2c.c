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
#include <timer.h>
#include <arch/io.h>
#include <device/i2c.h>
#include "clk.h"
#include "i2c.h"
#include "pinmux.h"

#define	I2C_WRITE	0
#define I2C_READ	1

#define I2C_OK		0
#define I2C_NOK		1
#define I2C_NACK	2
#define I2C_NOK_LA	3	/* Lost arbitration */
#define I2C_NOK_TOUT	4	/* time out */

/* HSI2C specific register description */

/* I2C_CTL Register bits */
/* FIXME(dhendrix): do we really need to cast these as unsigned? */
#define HSI2C_FUNC_MODE_I2C		(1u << 0)
#define HSI2C_MASTER			(1u << 3)
#define HSI2C_RXCHON			(1u << 6)	/* Write/Send */
#define HSI2C_TXCHON			(1u << 7)	/* Read/Receive */
#define HSI2C_SW_RST			(1u << 31)

/* I2C_FIFO_STAT Register bits */
#define HSI2C_TX_FIFO_LEVEL		(0x7f << 0)
#define HSI2C_TX_FIFO_FULL		(1u << 7)
#define HSI2C_TX_FIFO_EMPTY		(1u << 8)
#define HSI2C_RX_FIFO_LEVEL		(0x7f << 16)
#define HSI2C_RX_FIFO_FULL		(1u << 23)
#define HSI2C_RX_FIFO_EMPTY		(1u << 24)

/* I2C_FIFO_CTL Register bits */
#define HSI2C_RXFIFO_EN			(1u << 0)
#define HSI2C_TXFIFO_EN			(1u << 1)
#define HSI2C_TXFIFO_TRIGGER_LEVEL	(0x20 << 16)
#define HSI2C_RXFIFO_TRIGGER_LEVEL	(0x20 << 4)

/* I2C_TRAILING_CTL Register bits */
#define HSI2C_TRAILING_COUNT		(0xff)

/* I2C_INT_EN Register bits */
#define HSI2C_INT_TX_ALMOSTEMPTY_EN	(1u << 0)
#define HSI2C_INT_RX_ALMOSTFULL_EN	(1u << 1)
#define HSI2C_INT_TRAILING_EN		(1u << 6)
#define HSI2C_INT_I2C_EN		(1u << 9)

/* I2C_CONF Register bits */
#define HSI2C_AUTO_MODE			(1u << 31)
#define HSI2C_10BIT_ADDR_MODE		(1u << 30)
#define HSI2C_HS_MODE			(1u << 29)

/* I2C_AUTO_CONF Register bits */
#define HSI2C_READ_WRITE		(1u << 16)
#define HSI2C_STOP_AFTER_TRANS		(1u << 17)
#define HSI2C_MASTER_RUN		(1u << 31)

/* I2C_TIMEOUT Register bits */
#define HSI2C_TIMEOUT_EN		(1u << 31)

/* I2C_TRANS_STATUS register bits */
#define HSI2C_MASTER_BUSY		(1u << 17)
#define HSI2C_SLAVE_BUSY		(1u << 16)
#define HSI2C_TIMEOUT_AUTO		(1u << 4)
#define HSI2C_NO_DEV			(1u << 3)
#define HSI2C_NO_DEV_ACK		(1u << 2)
#define HSI2C_TRANS_ABORT		(1u << 1)
#define HSI2C_TRANS_DONE		(1u << 0)

#define HSI2C_SLV_ADDR_MAS(x)		((x & 0x3ff) << 10)

/* S3C I2C Controller bits */
#define I2CSTAT_BSY	0x20	/* Busy bit */
#define I2CSTAT_NACK	0x01	/* Nack bit */
#define I2CCON_ACKGEN	0x80	/* Acknowledge generation */
#define I2CCON_IRPND	0x10	/* Interrupt pending bit */
#define I2C_MODE_MT	0xC0	/* Master Transmit Mode */
#define I2C_MODE_MR	0x80	/* Master Receive Mode */
#define I2C_START_STOP	0x20	/* START / STOP */
#define I2C_TXRX_ENA	0x10	/* I2C Tx/Rx enable */

#define I2C_TIMEOUT_MS 1000		/* 1 second */

#define	HSI2C_TIMEOUT	100

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
	/* I2C4-I2C10 are part of the USI block */
	{
		.bus_num = 4,
		.hsregs = (struct exynos5_hsi2c *)0x12ca0000,
		.periph_id = PERIPH_ID_I2C4,
		.is_highspeed = 1,
	},
	{
		.bus_num = 5,
		.hsregs = (struct exynos5_hsi2c *)0x12cb0000,
		.periph_id = PERIPH_ID_I2C5,
		.is_highspeed = 1,
	},
	{
		.bus_num = 6,
		.hsregs = (struct exynos5_hsi2c *)0x12cc0000,
		.periph_id = PERIPH_ID_I2C6,
		.is_highspeed = 1,
	},
	{
		.bus_num = 7,
		.hsregs = (struct exynos5_hsi2c *)0x12cd0000,
		.periph_id = PERIPH_ID_I2C7,
		.is_highspeed = 1,
	},
	{
		.bus_num = 8,
		.hsregs = (struct exynos5_hsi2c *)0x12e00000,
		.periph_id = PERIPH_ID_I2C8,
		.is_highspeed = 1,
	},
	{
		.bus_num = 9,
		.hsregs = (struct exynos5_hsi2c *)0x12e10000,
		.periph_id = PERIPH_ID_I2C9,
		.is_highspeed = 1,
	},
	{
		.bus_num = 10,
		.hsregs = (struct exynos5_hsi2c *)0x12e20000,
		.periph_id = PERIPH_ID_I2C10,
		.is_highspeed = 1,
	},
};

/*
 * Wait til the byte transfer is completed.
 *
 * @param i2c- pointer to the appropriate i2c register bank.
 * @return I2C_OK, if transmission was ACKED
 *         I2C_NACK, if transmission was NACKED
 *         I2C_NOK_TIMEOUT, if transaction did not complete in I2C_TIMEOUT_MS
 */

static int WaitForXfer(struct s3c24x0_i2c *i2c)
{
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_usecs(&end, I2C_TIMEOUT_MS * 1000);
	do {
		if (read32(&i2c->iiccon) & I2CCON_IRPND)
			return (read32(&i2c->iicstat) & I2CSTAT_NACK) ?
				I2C_NACK : I2C_OK;
		timer_monotonic_get(&current);
	} while (mono_time_before(&current, &end));

	printk(BIOS_ERR, "%s timed out\n", __func__);
	return I2C_NOK_TOUT;
}

static void ReadWriteByte(struct s3c24x0_i2c *i2c)
{
	writel(read32(&i2c->iiccon) & ~I2CCON_IRPND, &i2c->iiccon);
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
	write32(val, &bus->regs->iiccon);

	/* init to SLAVE RECEIVE mode and clear I2CADDn */
	write32(0, &bus->regs->iicstat);
	write32(slaveadd, &bus->regs->iicadd);
	/* program Master Transmit (and implicit STOP) */
	write32(I2C_MODE_MT | I2C_TXRX_ENA, &bus->regs->iicstat);
}

static int hsi2c_get_clk_details(struct s3c24x0_i2c_bus *i2c_bus,
		unsigned int bus_freq_hz)
{
	struct exynos5_hsi2c *hsregs = i2c_bus->hsregs;
	unsigned long clkin = clock_get_periph_rate(i2c_bus->periph_id);
	unsigned int i = 0, utemp0 = 0, utemp1 = 0;
	unsigned int t_ftl_cycle;

	/* FPCLK / FI2C =
	 * (CLK_DIV + 1) * (TSCLK_L + TSCLK_H + 2) + 8 + 2 * FLT_CYCLE
	 * uTemp0 = (CLK_DIV + 1) * (TSCLK_L + TSCLK_H + 2)
	 * uTemp1 = (TSCLK_L + TSCLK_H + 2)
	 * uTemp2 = TSCLK_L + TSCLK_H
	 */
	t_ftl_cycle = (read32(&hsregs->usi_conf) >> 16) & 0x7;
	utemp0 = (clkin / bus_freq_hz) - 8 - 2 * t_ftl_cycle;

	/* CLK_DIV max is 256 */
	for (i = 0; i < 256; i++) {
		utemp1 = utemp0 / (i + 1);
		if ((utemp1 < 512) && (utemp1 > 4)) {
			i2c_bus->clk_cycle = utemp1 - 2;
			i2c_bus->clk_div = i;
			return 0;
		}
	}
	printk(BIOS_ERR, "%s: failed?\n", __func__);
	return -1;
}

static void hsi2c_ch_init(struct s3c24x0_i2c_bus *i2c_bus,
				unsigned int bus_freq_hz)
{
	struct exynos5_hsi2c *hsregs = i2c_bus->hsregs;
	unsigned int t_sr_release;
	unsigned int n_clkdiv;
	unsigned int t_start_su, t_start_hd;
	unsigned int t_stop_su;
	unsigned int t_data_su, t_data_hd;
	unsigned int t_scl_l, t_scl_h;
	u32 i2c_timing_s1;
	u32 i2c_timing_s2;
	u32 i2c_timing_s3;
	u32 i2c_timing_sla;

	hsi2c_get_clk_details(i2c_bus, bus_freq_hz);

	n_clkdiv = i2c_bus->clk_div;
	t_scl_l = i2c_bus->clk_cycle / 2;
	t_scl_h = i2c_bus->clk_cycle / 2;
	t_start_su = t_scl_l;
	t_start_hd = t_scl_l;
	t_stop_su = t_scl_l;
	t_data_su = t_scl_l / 2;
	t_data_hd = t_scl_l / 2;
	t_sr_release = i2c_bus->clk_cycle;

	i2c_timing_s1 = t_start_su << 24 | t_start_hd << 16 | t_stop_su << 8;
	i2c_timing_s2 = t_data_su << 24 | t_scl_l << 8 | t_scl_h << 0;
	i2c_timing_s3 = n_clkdiv << 16 | t_sr_release << 0;
	i2c_timing_sla = t_data_hd << 0;

	write32(HSI2C_TRAILING_COUNT, &hsregs->usi_trailing_ctl);

	/* Clear to enable Timeout */
	clrsetbits_le32(&hsregs->usi_timeout, HSI2C_TIMEOUT_EN, 0);

	write32(read32(&hsregs->usi_conf) | HSI2C_AUTO_MODE, &hsregs->usi_conf);

	/* Currently operating in Fast speed mode. */
	write32(i2c_timing_s1, &hsregs->usi_timing_fs1);
	write32(i2c_timing_s2, &hsregs->usi_timing_fs2);
	write32(i2c_timing_s3, &hsregs->usi_timing_fs3);
	write32(i2c_timing_sla, &hsregs->usi_timing_sla);

	/* Enable TXFIFO and RXFIFO */
	write32(HSI2C_RXFIFO_EN | HSI2C_TXFIFO_EN, &hsregs->usi_fifo_ctl);

	/* i2c_conf configure */
	write32(readl(&hsregs->usi_conf) | HSI2C_AUTO_MODE, &hsregs->usi_conf);
}

/* SW reset for the high speed bus */
static void i2c_reset(struct s3c24x0_i2c_bus *i2c_bus)
{
	struct exynos5_hsi2c *i2c = i2c_bus->hsregs;
	u32 i2c_ctl;

	/* Set and clear the bit for reset */
	i2c_ctl = read32(&i2c->usi_ctl);
	i2c_ctl |= HSI2C_SW_RST;
	write32(i2c_ctl, &i2c->usi_ctl);

	i2c_ctl = read32(&i2c->usi_ctl);
	i2c_ctl &= ~HSI2C_SW_RST;
	write32(i2c_ctl, &i2c->usi_ctl);

	/* Initialize the configure registers */
	/* FIXME: This just assumes 100KHz as a default bus freq */
	hsi2c_ch_init(i2c_bus, 100000);
}

void i2c_init(unsigned bus_num, int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c;

	i2c = &i2c_buses[bus_num];

	i2c_reset(i2c);

	if (i2c->is_highspeed)
		hsi2c_ch_init(i2c, speed);
	else
		i2c_ch_init(i2c, speed, slaveadd);
}

/*
 * Check whether the transfer is complete.
 * Return values:
 * 0  - transfer not done
 * 1  - transfer finished successfully
 * -1 - transfer failed
 */
static int hsi2c_check_transfer(struct exynos5_hsi2c *i2c)
{
	uint32_t status = read32(&i2c->usi_trans_status);
	if (status & (HSI2C_TRANS_ABORT | HSI2C_NO_DEV_ACK |
		      HSI2C_NO_DEV | HSI2C_TIMEOUT_AUTO)) {
		if (status & HSI2C_TRANS_ABORT)
			printk(BIOS_ERR,
			       "%s: Transaction aborted.\n", __func__);
		if (status & HSI2C_NO_DEV_ACK)
			printk(BIOS_ERR,
			       "%s: No ack from device.\n", __func__);
		if (status & HSI2C_NO_DEV)
			printk(BIOS_ERR,
			       "%s: No response from device.\n", __func__);
		if (status & HSI2C_TIMEOUT_AUTO)
			printk(BIOS_ERR,
			       "%s: Transaction time out.\n", __func__);
		return -1;
	}
	return !(status & HSI2C_MASTER_BUSY);
}

/*
 * Wait for the transfer to finish.
 * Return values:
 * 0  - transfer not done
 * 1  - transfer finished successfully
 * -1 - transfer failed
 */
static int hsi2c_wait_for_transfer(struct exynos5_hsi2c *i2c)
{
	struct mono_time current, end;

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_usecs(&end, HSI2C_TIMEOUT * 1000);
	while (mono_time_before(&current, &end)) {
		int ret = hsi2c_check_transfer(i2c);
		if (ret)
			return ret;
		udelay(5);
		timer_monotonic_get(&current);
	}
	return 0;
}

static int hsi2c_senddata(struct exynos5_hsi2c *i2c, const uint8_t *data,
			  int len)
{
	while (!hsi2c_check_transfer(i2c) && len) {
		if (!(read32(&i2c->usi_fifo_stat) & HSI2C_TX_FIFO_FULL)) {
			write32(*data++, &i2c->usi_txdata);
			len--;
		}
	}
	return len ? -1 : 0;
}

static int hsi2c_recvdata(struct exynos5_hsi2c *i2c, uint8_t *data, int len)
{
	while (!hsi2c_check_transfer(i2c) && len) {
		if (!(read32(&i2c->usi_fifo_stat) & HSI2C_RX_FIFO_EMPTY)) {
			*data++ = read32(&i2c->usi_rxdata);
			len--;
		}
	}
	return len ? -1 : 0;
}

static int hsi2c_write(struct exynos5_hsi2c *i2c,
			unsigned char chip,
			unsigned char addr[],
			unsigned char alen,
			const uint8_t data[],
			unsigned short len)
{
	uint32_t i2c_auto_conf;

	if (hsi2c_wait_for_transfer(i2c) != 1)
		return -1;

	/* chip address */
	write32(HSI2C_SLV_ADDR_MAS(chip), &i2c->i2c_addr);

	/* usi_ctl enable i2c func, master write configure */
	write32((HSI2C_TXCHON | HSI2C_FUNC_MODE_I2C | HSI2C_MASTER),
							&i2c->usi_ctl);

	/* auto_conf for write length and stop configure */
	i2c_auto_conf = ((len + alen) | HSI2C_STOP_AFTER_TRANS);
	i2c_auto_conf &= ~HSI2C_READ_WRITE;
	/* Master run, start xfer */
	i2c_auto_conf |= HSI2C_MASTER_RUN;
	write32(i2c_auto_conf, &i2c->usi_auto_conf);

	if (hsi2c_senddata(i2c, addr, alen) ||
	    hsi2c_senddata(i2c, data, len) ||
	    hsi2c_wait_for_transfer(i2c) != 1) {
		return -1;
	}

	write32(HSI2C_FUNC_MODE_I2C, &i2c->usi_ctl);
	return 0;
}

static int hsi2c_read(struct exynos5_hsi2c *i2c,
			unsigned char chip,
			unsigned char addr[],
			unsigned char alen,
			uint8_t data[],
			unsigned short len,
			int check)
{
	uint32_t i2c_auto_conf;

	/* start read */
	if (hsi2c_wait_for_transfer(i2c) != 1)
		return -1;

	/* chip address */
	write32(HSI2C_SLV_ADDR_MAS(chip), &i2c->i2c_addr);

	/* usi_ctl enable i2c func, master write configure */
	write32((HSI2C_TXCHON | HSI2C_FUNC_MODE_I2C | HSI2C_MASTER),
							&i2c->usi_ctl);

	/* auto_conf */
	write32(alen | HSI2C_MASTER_RUN | HSI2C_STOP_AFTER_TRANS,
		&i2c->usi_auto_conf);

	if (hsi2c_senddata(i2c, addr, alen) ||
	    hsi2c_wait_for_transfer(i2c) != 1) {
		return -1;
	}

	/* usi_ctl enable i2c func, master WRITE configure */
	write32((HSI2C_RXCHON | HSI2C_FUNC_MODE_I2C | HSI2C_MASTER),
							&i2c->usi_ctl);

	/* auto_conf, length and stop configure */
	i2c_auto_conf = (len | HSI2C_STOP_AFTER_TRANS | HSI2C_READ_WRITE);
	i2c_auto_conf |= HSI2C_MASTER_RUN;
	/* Master run, start xfer */
	write32(i2c_auto_conf, &i2c->usi_auto_conf);

	if (hsi2c_recvdata(i2c, data, len) ||
	    hsi2c_wait_for_transfer(i2c) != 1) {
		return -1;
	}

	write32(HSI2C_FUNC_MODE_I2C, &i2c->usi_ctl);
	return 0;
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
	int i = 0, result;
	struct mono_time current, end;

	if (data == 0 || data_len == 0) {
		printk(BIOS_ERR, "i2c_transfer: bad call\n");
		return I2C_NOK;
	}

	timer_monotonic_get(&current);
	end = current;
	mono_time_add_usecs(&end, I2C_TIMEOUT_MS * 1000);
	while (readl(&i2c->iicstat) & I2CSTAT_BSY) {
		if (!mono_time_before(&current, &end)){
			printk(BIOS_ERR, "%s timed out\n", __func__);
			return I2C_NOK_TOUT;
		}
		timer_monotonic_get(&current);
	}

	write32(read32(&i2c->iiccon) | I2CCON_ACKGEN, &i2c->iiccon);

	/* Get the slave chip address going */
	write32(chip, &i2c->iicds);
	if ((cmd_type == I2C_WRITE) || (addr && addr_len))
		write32(I2C_MODE_MT | I2C_TXRX_ENA | I2C_START_STOP,
		       &i2c->iicstat);
	else
		write32(I2C_MODE_MR | I2C_TXRX_ENA | I2C_START_STOP,
		       &i2c->iicstat);

	/* Wait for chip address to transmit. */
	result = WaitForXfer(i2c);
	if (result != I2C_OK)
		goto bailout;

	/* If register address needs to be transmitted - do it now. */
	if (addr && addr_len) {
		while ((i < addr_len) && (result == I2C_OK)) {
			write32(addr[i++], &i2c->iicds);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
		}
		i = 0;
		if (result != I2C_OK)
			goto bailout;
	}

	switch (cmd_type) {
	case I2C_WRITE:
		while ((i < data_len) && (result == I2C_OK)) {
			write32(data[i++], &i2c->iicds);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
		}
		break;

	case I2C_READ:
		if (addr && addr_len) {
			/*
			 * Register address has been sent, now send slave chip
			 * address again to start the actual read transaction.
			 */
			write32(chip, &i2c->iicds);

			/* Generate a re-START. */
			write32(I2C_MODE_MR | I2C_TXRX_ENA | I2C_START_STOP,
			       &i2c->iicstat);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
			if (result != I2C_OK)
				goto bailout;
		}

		while ((i < data_len) && (result == I2C_OK)) {
			/* disable ACK for final READ */
			if (i == data_len - 1)
				write32(readl(&i2c->iiccon)
				       & ~I2CCON_ACKGEN,
				       &i2c->iiccon);
			ReadWriteByte(i2c);
			result = WaitForXfer(i2c);
			data[i++] = read32(&i2c->iicds);
		}
		if (result == I2C_NACK)
			result = I2C_OK; /* Normal terminated read. */
		break;

	default:
		printk(BIOS_ERR, "i2c_transfer: bad call\n");
		result = I2C_NOK;
		break;
	}

bailout:
	/* Send STOP. */
	write32(I2C_MODE_MR | I2C_TXRX_ENA, &i2c->iicstat);
	ReadWriteByte(i2c);

	return result;
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
	if (i2c->is_highspeed)
		ret = hsi2c_read(i2c->hsregs, chip, &xaddr[4 - alen],
						alen, buf, len, 0);
	else
		ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1,
				&xaddr[4 - alen], alen, buf, len);
	if (ret) {
		i2c_reset(i2c);
		printk(BIOS_ERR, "I2C read (bus %02x, chip addr %02x) failed: "
				"%d\n", bus, chip, ret);
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
	if (i2c->is_highspeed)
		ret = hsi2c_write(i2c->hsregs, chip, &xaddr[4 - alen],
					alen, buf, len);
	else
		ret = i2c_transfer(i2c->regs, I2C_WRITE, chip << 1,
				&xaddr[4 - alen], alen, (void *)buf, len);


	if (ret != 0) {
		i2c_reset(i2c);
		printk(BIOS_ERR, "I2C write (bus %02x, chip addr %02x) failed: "
				"%d\n", bus, chip, ret);
		return 1;
	}
	return 0;
}
