/*
 * (C) Copyright 2002
 * David Mueller, ELSOFT AG, d.mueller@elsoft.ch
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/* This code should work for both the S3C2400 and the S3C2410
 * as they seem to have the same I2C controller inside.
 * The different address mapping is handled by the s3c24xx.h files below.
 */

#include <common.h>
#include <arch/io.h>
#include "clk.h"
#include "cpu/samsung/exynos5-common/clk.h"
#include "cpu/samsung/exynos5250/cpu.h"
#include "gpio.h"
#include "cpu/samsung/exynos5250/gpio.h"
#include "cpu/samsung/exynos5250/pinmux.h"

//#include <fdtdec.h>
#include "device/i2c-old.h"
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

/* We should not rely on any particular ordering of these IDs */
#if 0
#ifndef CONFIG_OF_CONTROL
static enum periph_id periph_for_dev[EXYNOS_I2C_MAX_CONTROLLERS] = {
	PERIPH_ID_I2C0,
	PERIPH_ID_I2C1,
	PERIPH_ID_I2C2,
	PERIPH_ID_I2C3,
	PERIPH_ID_I2C4,
	PERIPH_ID_I2C5,
	PERIPH_ID_I2C6,
	PERIPH_ID_I2C7,
};
#endif
#endif

static unsigned int g_current_bus __attribute__((section(".data")));
static struct s3c24x0_i2c *g_early_i2c_config __attribute__((section(".data")));

static struct s3c24x0_i2c_bus i2c_bus[EXYNOS_I2C_MAX_CONTROLLERS]
		__attribute__((section(".data")));
static int i2c_busses __attribute__((section(".data")));

void i2c_set_early_reg(unsigned int base)
{
	g_early_i2c_config = (struct s3c24x0_i2c *)base;
}

static struct s3c24x0_i2c_bus *get_bus(int bus_idx)
{
	/* If an early i2c config exists we just use that */
	if (g_early_i2c_config) {
		/* FIXME: value not retained from i2c_set_early_reg()? (but then, how
		 * did if (!i2c) check pass earlier on? Corrupt value? */
		i2c_bus[0].regs = g_early_i2c_config;
		return &i2c_bus[0];
	}

	if (bus_idx < i2c_busses)
		return &i2c_bus[bus_idx];
	debug("Undefined bus: %d\n", bus_idx);
	return NULL;
}

static inline struct exynos5_gpio_part1 *exynos_get_base_gpio1(void)
{
	return (struct exynos5_gpio_part1 *)(EXYNOS5_GPIO_PART1_BASE);
}

static int WaitForXfer(struct s3c24x0_i2c *i2c)
{
	int i;

	i = I2C_XFER_TIMEOUT_MS * 20;
	while (!(readl(&i2c->iiccon) & I2CCON_IRPND)) {
		if (i == 0) {
			debug("%s: i2c xfer timeout\n", __func__);
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
	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
//	writel(readl(&i2c->iiccon) & ~I2CCON_IRPND, &i2c->iiccon);
}

static void i2c_ch_init(struct s3c24x0_i2c *i2c, int speed, int slaveadd)
{
	unsigned long freq, pres = 16, div;

	freq = clock_get_periph_rate(PERIPH_ID_I2C0);
	/* calculate prescaler and divisor values */
	if ((freq / pres / (16 + 1)) > speed)
		/* set prescaler to 512 */
		pres = 512;

	div = 0;

	while ((freq / pres / (div + 1)) > speed)
		div++;

	/* set prescaler, divisor according to freq, also set ACKGEN, IRQ */
	writel((div & 0x0F) | 0xA0 | ((pres == 512) ? 0x40 : 0), &i2c->iiccon);

	/* init to SLAVE REVEIVE and set slaveaddr */
	writel(0, &i2c->iicstat);
	writel(slaveadd, &i2c->iicadd);
	/* program Master Transmit (and implicit STOP) */
	writel(I2C_MODE_MT | I2C_TXRX_ENA, &i2c->iicstat);
}

/* TODO: determine if this is necessary to init board using FDT-provided info */
#if 0
void board_i2c_init(const void *blob)
{
	/*
	 * Turn off the early i2c configuration and init the i2c properly,
	 * this is done here to enable the use of i2c configs from FDT.
	 */
	i2c_set_early_reg(0);

#ifdef CONFIG_OF_CONTROL
	int node_list[EXYNOS_I2C_MAX_CONTROLLERS];
	int i, count;

	count = fdtdec_find_aliases_for_id(blob, "i2c",
		COMPAT_SAMSUNG_S3C2440_I2C, node_list,
		EXYNOS_I2C_MAX_CONTROLLERS);

	for (i = 0; i < count; i++) {
		struct s3c24x0_i2c_bus *bus;
		int node = node_list[i];

		if (node < 0)
			continue;
		bus = &i2c_bus[i2c_busses];
		bus->regs = (struct s3c24x0_i2c *)
			fdtdec_get_addr(blob, node, "reg");
		bus->id = (enum periph_id)
			fdtdec_get_int(blob, node, "samsung,periph-id", -1);
		bus->node = node;
		bus->bus_num = i2c_busses++;
	}
#else
	int i;

	for (i = 0; i < EXYNOS_I2C_MAX_CONTROLLERS; i++) {
		unsigned intptr_t reg_addr = samsung_get_base_i2c() +
			EXYNOS_I2C_SPACING * i;

		i2c_bus[i].regs = (struct s3c24x0_i2c_bus *)reg_addr;
		i2c_bus[i].id = periph_for_dev[i];
	}
	i2c_busses = EXYNOS_I2C_MAX_CONTROLLERS;
#endif
}
#endif

/*
 * MULTI BUS I2C support
 */
static void i2c_bus_init(struct s3c24x0_i2c_bus *i2c, unsigned int bus)
{
	exynos_pinmux_config(i2c->id, 0);
	i2c_ch_init(i2c->regs, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
}

#ifdef CONFIG_I2C_MULTI_BUS
int i2c_set_bus_num(unsigned int bus)
{
	struct s3c24x0_i2c_bus *i2c;

	i2c = get_bus(bus);
	if (!i2c)
		return -1;
	g_current_bus = bus;
	i2c_bus_init(i2c, g_current_bus);

	return 0;
}

unsigned int i2c_get_bus_num(void)
{
	return g_current_bus;
}
#endif

#ifdef CONFIG_OF_CONTROL
int i2c_get_bus_num_fdt(const void *blob, int node)
{
	enum fdt_compat_id compat;
	fdt_addr_t reg;
	int i;

	compat = fdtdec_lookup(blob, node);
	if (compat != COMPAT_SAMSUNG_S3C2440_I2C) {
		debug("%s: Not a supported I2C node\n", __func__);
		return -1;
	}

	reg = fdtdec_get_addr(blob, node, "reg");
	for (i = 0; i < i2c_busses; i++)
		if (reg == (fdt_addr_t)(unsigned intptr_t)i2c_bus[i].regs)
			return i;

	debug("%s: Can't find any matched I2C bus\n", __func__);
	return -1;
}

int i2c_reset_port_fdt(const void *blob, int node)
{
	struct s3c24x0_i2c_bus *i2c;

	int bus;

	bus = i2c_get_bus_num_fdt(blob, node);
	if (bus < 0) {
		printf("could not get bus for node %d\n", node);
		return -1;
	}
	i2c = get_bus(bus);
	if (!i2c) {
		printf("get_bus() failed for node node %d\n", node);
		return -1;
	}

	i2c_ch_init(i2c->regs, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);

	return 0;
}
#endif

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

void i2c_init(int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c;
	struct exynos5_gpio_part1 *gpio;
	int i;
	uint32_t x;

	/* By default i2c channel 0 is the current bus */
	g_current_bus = 0;

	i2c = get_bus(g_current_bus);
	if (!i2c)
		return;

	i2c_bus_init(i2c, g_current_bus);

	/* wait for some time to give previous transfer a chance to finish */
	i = I2C_INIT_TIMEOUT_MS * 20;
	while ((readl(&i2c->regs->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		udelay(50);
		i--;
	}

	gpio = exynos_get_base_gpio1();
	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
//	writel((readl(&gpio->b3.con) & ~0x00FF) | 0x0022, &gpio->b3.con);
	x = readl(&gpio->b3.con);
	writel((x & ~0x00FF) | 0x0022, &gpio->b3.con);

	i2c_ch_init(i2c->regs, speed, slaveadd);
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
		debug("i2c_transfer: bad call\n");
		return I2C_NOK;
	}

	/* Check I2C bus idle */
	i = I2C_IDLE_TIMEOUT_MS * 20;
	while ((readl(&i2c->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		udelay(50);
		i--;
	}

	if (readl(&i2c->iicstat) & I2CSTAT_BSY) {
		debug("%s: bus busy\n", __func__);
		return I2C_NOK_TOUT;
	}

	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
	//writel(readl(&i2c->iiccon) | I2CCON_ACKGEN, &i2c->iiccon);
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
					/* FIXME(dhendrix): nested macro */
#if 0
					writel(readl(&i2c->iiccon) &
					       ~I2CCON_ACKGEN,
					       &i2c->iiccon);
#endif
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
		debug("i2c_transfer: bad call\n");
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

int i2c_probe(unsigned char chip)
{
	struct s3c24x0_i2c_bus *i2c;
	unsigned char buf[1];
	int ret;

	i2c = get_bus(g_current_bus);
	if (!i2c)
		return -1;
	buf[0] = 0;

	/*
	 * What is needed is to send the chip address and verify that the
	 * address was <ACK>ed (i.e. there was a chip at that address which
	 * drove the data line low).
	 */
	ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1, 0, 0, buf, 1);

	return ret != I2C_OK;
}

int i2c_read(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c;
	unsigned char xaddr[4];
	int ret;

	if (alen > 4) {
		debug("I2C read: addr len %d not supported\n", alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}

#ifdef CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW
	/*
	 * EEPROM chips that implement "address overflow" are ones
	 * like Catalyst 24WC04/08/16 which has 9/10/11 bits of
	 * address and the extra bits end up in the "chip address"
	 * bit slots. This makes a 24WC08 (1Kbyte) chip look like
	 * four 256 byte chips.
	 *
	 * Note that we consider the length of the address field to
	 * still be one byte because the extra address bits are
	 * hidden in the chip address.
	 */
	if (alen > 0)
		chip |= ((addr >> (alen * 8)) &
			 CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW);
#endif
	i2c = get_bus(g_current_bus);
	if (!i2c)
		return -1;
	ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);
	if (ret) {
		debug("I2c read: failed %d\n", ret);
		return 1;
	}
	return 0;
}

int i2c_write(unsigned char chip, unsigned int addr, int alen, unsigned char *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c;
	unsigned char xaddr[4];
	int ret;

	if (alen > 4) {
		debug("I2C write: addr len %d not supported\n", alen);
		return 1;
	}

	if (alen > 0) {
		xaddr[0] = (addr >> 24) & 0xFF;
		xaddr[1] = (addr >> 16) & 0xFF;
		xaddr[2] = (addr >> 8) & 0xFF;
		xaddr[3] = addr & 0xFF;
	}
#ifdef CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW
	/*
	 * EEPROM chips that implement "address overflow" are ones
	 * like Catalyst 24WC04/08/16 which has 9/10/11 bits of
	 * address and the extra bits end up in the "chip address"
	 * bit slots. This makes a 24WC08 (1Kbyte) chip look like
	 * four 256 byte chips.
	 *
	 * Note that we consider the length of the address field to
	 * still be one byte because the extra address bits are
	 * hidden in the chip address.
	 */
	if (alen > 0)
		chip |= ((addr >> (alen * 8)) &
			 CONFIG_SYS_I2C_EEPROM_ADDR_OVERFLOW);
#endif
	i2c = get_bus(g_current_bus);
	if (!i2c)
		return -1;

	ret = i2c_transfer(i2c->regs, I2C_WRITE, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);

	return ret != 0;
}
