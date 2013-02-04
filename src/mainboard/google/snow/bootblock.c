/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The ChromiumOS Authors.  All rights reserved.
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

#include <stdlib.h>
#include <types.h>
#include <assert.h>
#include <arch/armv7/include/common.h>
#include <arch/io.h>
#include "cpu/samsung/s5p-common/clk.h"
#include "cpu/samsung/s5p-common/s3c24x0_i2c.h"
#include "cpu/samsung/exynos5-common/spi.h"
#include "cpu/samsung/exynos5-common/uart.h"
#include "cpu/samsung/exynos5250/clk.h"
#include "cpu/samsung/exynos5250/cpu.h"
#include "cpu/samsung/exynos5250/dmc.h"
#include "cpu/samsung/exynos5250/gpio.h"
#include "cpu/samsung/exynos5250/periph.h"
#include "cpu/samsung/exynos5250/setup.h"
#include "cpu/samsung/exynos5250/clock_init.h"

#include <device/i2c.h>
#include <drivers/maxim/max77686/max77686.h>

#include <console/console.h>
#include <cbfs.h>

static uint32_t uart3_base = CONFIG_CONSOLE_SERIAL_UART_ADDRESS;

#define CONFIG_SYS_CLK_FREQ            24000000

static void serial_setbrg_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
	u32 uclk;
	u32 baudrate = CONFIG_TTYS0_BAUD;
	u32 val;
//	enum periph_id periph;

//	periph = exynos5_get_periph_id(base_port);
	uclk = clock_get_periph_rate(PERIPH_ID_UART3);
	val = uclk / baudrate;

	writel(val / 16 - 1, &uart->ubrdiv);

	/*
	 * FIXME(dhendrix): the original uart.h had a "br_rest" value which
	 * does not seem relevant to the exynos5250... not entirely sure
	 * where/if we need to worry about it here
	 */
#if 0
	if (s5p_uart_divslot())
		writew(udivslot[val % 16], &uart->rest.slot);
	else
		writeb(val % 16, &uart->rest.value);
#endif
}

/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 */
static void exynos5_init_dev(void)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	/* enable FIFOs */
	writel(0x1, &uart->ufcon);
	writel(0, &uart->umcon);
	/* 8N1 */
	writel(0x3, &uart->ulcon);
	/* No interrupts, no DMA, pure polling */
	writel(0x245, &uart->ucon);

	serial_setbrg_dev();
}

static int exynos5_uart_err_check(int op)
{
	//struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;
	unsigned int mask;

	/*
	 * UERSTAT
	 * Break Detect	[3]
	 * Frame Err	[2] : receive operation
	 * Parity Err	[1] : receive operation
	 * Overrun Err	[0] : receive operation
	 */
	if (op)
		mask = 0x8;
	else
		mask = 0xf;

	return readl(&uart->uerstat) & mask;
}

#define RX_FIFO_COUNT_MASK	0xff
#define RX_FIFO_FULL_MASK	(1 << 8)
#define TX_FIFO_FULL_MASK	(1 << 24)

/*
 * Output a single byte to the serial port.
 */
static void exynos5_uart_tx_byte(unsigned char data)
{
//	struct s5p_uart *const uart = s5p_get_base_uart(dev_index);
	struct s5p_uart *uart = (struct s5p_uart *)uart3_base;

	if (data == '\n')
		exynos5_uart_tx_byte('\r');

	/* wait for room in the tx FIFO */
	while ((readl(uart->ufstat) & TX_FIFO_FULL_MASK)) {
		if (exynos5_uart_err_check(1))
			return;
	}

	writeb(data, &uart->utxh);
}

void puts(const char *s);
void puts(const char *s)
{
	int n = 0;

	while (*s) {
		if (*s == '\n') {
			exynos5_uart_tx_byte(0xd);	/* CR */
		}

		exynos5_uart_tx_byte(*s++);
		n++;
	}
}

static void do_serial(void)
{
	//exynos_pinmux_config(PERIPH_ID_UART3, PINMUX_FLAG_NONE);
	gpio_set_pull(GPIO_A14, EXYNOS_GPIO_PULL_NONE);
	gpio_cfg_pin(GPIO_A15, EXYNOS_GPIO_FUNC(0x2));

	exynos5_init_dev();
}

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

#define I2C0_BASE	0x12c60000
struct s3c24x0_i2c_bus i2c0 = {
	.node = 0,
	.bus_num = 0,
	.regs = (struct s3c24x0_i2c *)I2C0_BASE,
	.id = PERIPH_ID_I2C0,
};

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

static void i2c_bus_init(struct s3c24x0_i2c_bus *i2c, unsigned int bus)
{
//	exynos_pinmux_config(i2c->id, 0);
	gpio_cfg_pin(GPIO_B30, EXYNOS_GPIO_FUNC(0x2));
	gpio_cfg_pin(GPIO_B31, EXYNOS_GPIO_FUNC(0x2));
	gpio_set_pull(GPIO_B30, EXYNOS_GPIO_PULL_NONE);
	gpio_set_pull(GPIO_B31, EXYNOS_GPIO_PULL_NONE);

	i2c_ch_init(i2c->regs, CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
}

void do_barriers(void);
void do_barriers(void)
{
	/*
	 * The reason we don't write out the instructions dsb/isb/sev:
	 * While ARM Cortex-A8 supports ARM v7 instruction set (-march=armv7a),
	 * we compile with -march=armv5 to allow more compilers to work.
	 * For U-Boot code this has no performance impact.
	 */
	__asm__ __volatile__(
#if defined(__thumb__)
	".hword 0xF3BF, 0x8F4F\n"  /* dsb; darn -march=armv5 */
	".hword 0xF3BF, 0x8F6F\n"  /* isb; darn -march=armv5 */
	".hword 0xBF40\n"          /* sev; darn -march=armv5 */
#else
	".word  0xF57FF04F\n"      /* dsb; darn -march=armv5 */
	".word  0xF57FF06F\n"      /* isb; darn -march=armv5 */
	".word  0xE320F004\n"      /* sev; darn -march=armv5 */
#endif
	);
}

/* is this right? meh, it seems to work well enough... */
void my_udelay(unsigned int n);
void my_udelay(unsigned int n)
{
	n *= 1000;
	__asm__ volatile ("1:\n" "subs %0, %1, #1\n"
			  "bne 1b":"=r" (n):"0"(n));
}

void i2c_init(int speed, int slaveadd)
{
	struct s3c24x0_i2c_bus *i2c = &i2c0;
	struct exynos5_gpio_part1 *gpio;
	int i;
	uint32_t x;

#if 0
	/* By default i2c channel 0 is the current bus */
	g_current_bus = 0;

	i2c = get_bus(g_current_bus);
	if (!i2c)
		return;
#endif

	i2c_bus_init(i2c, 0);

	/* wait for some time to give previous transfer a chance to finish */
	i = I2C_INIT_TIMEOUT_MS * 20;
	while ((readl(&i2c->regs->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		my_udelay(50);
		i--;
	}

	gpio = (struct exynos5_gpio_part1 *)(EXYNOS5_GPIO_PART1_BASE);
	/* FIXME(dhendrix): cannot use nested macro (compilation failure) */
//	writel((readl(&gpio->b3.con) & ~0x00FF) | 0x0022, &gpio->b3.con);
	x = readl(&gpio->b3.con);
	writel((x & ~0x00FF) | 0x0022, &gpio->b3.con);

	i2c_ch_init(i2c->regs, speed, slaveadd);
}

static int WaitForXfer(struct s3c24x0_i2c *i2c)
{
	int i;

	i = I2C_XFER_TIMEOUT_MS * 20;
	while (!(readl(&i2c->iiccon) & I2CCON_IRPND)) {
		if (i == 0) {
			//debug("%s: i2c xfer timeout\n", __func__);
			return I2C_NOK_TOUT;
		}
		my_udelay(50);
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
		my_udelay(5);
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
		//debug("i2c_transfer: bad call\n");
		return I2C_NOK;
	}

	/* Check I2C bus idle */
	i = I2C_IDLE_TIMEOUT_MS * 20;
	while ((readl(&i2c->iicstat) & I2CSTAT_BSY) && (i > 0)) {
		my_udelay(50);
		i--;
	}

	if (readl(&i2c->iicstat) & I2CSTAT_BSY) {
		//debug("%s: bus busy\n", __func__);
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
		//debug("i2c_transfer: bad call\n");
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


int i2c_read(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c = &i2c0;
	uchar xaddr[4];
	int ret;

	if (alen > 4) {
		//debug("I2C read: addr len %d not supported\n", alen);
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
	if (!i2c)
		return -1;
	ret = i2c_transfer(i2c->regs, I2C_READ, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);
	if (ret) {
		//debug("I2c read: failed %d\n", ret);
		return 1;
	}
	return 0;
}

int i2c_write(uchar chip, uint addr, int alen, uchar *buffer, int len)
{
	struct s3c24x0_i2c_bus *i2c;
	uchar xaddr[4];
	int ret;

	if (alen > 4) {
		//debug("I2C write: addr len %d not supported\n", alen);
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
	//i2c = get_bus(g_current_bus);
	i2c = &i2c0;
	if (!i2c)
		return -1;
	ret = i2c_transfer(i2c->regs, I2C_WRITE, chip << 1, &xaddr[4 - alen],
			   alen, buffer, len);

	return ret != 0;
}

/*
 * Max77686 parameters values
 * see max77686.h for parameters details
 */
struct max77686_para max77686_param[] = {/*{vol_addr, vol_bitpos,
	vol_bitmask, reg_enaddr, reg_enbitpos, reg_enbitmask, reg_enbiton,
	reg_enbitoff, vol_min, vol_div}*/
	{/* PMIC_BUCK1 */ 0x11, 0x0, 0x3F, 0x10, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK2 */ 0x14, 0x0, 0xFF, 0x12, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK3 */ 0x1E, 0x0, 0xFF, 0x1C, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK4 */ 0x28, 0x0, 0xFF, 0x26, 0x4, 0x3, 0x1, 0x0, 600, 12500},
	{/* PMIC_BUCK5 */ 0x31, 0x0, 0x3F, 0x30, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK6 */ 0x33, 0x0, 0x3F, 0x32, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK7 */ 0x35, 0x0, 0x3F, 0x34, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK8 */ 0x37, 0x0, 0x3F, 0x36, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_BUCK9 */ 0x39, 0x0, 0x3F, 0x38, 0x0, 0x3, 0x3, 0x0, 750, 50000},
	{/* PMIC_LDO1 */  0x40, 0x0, 0x3F, 0x40, 0x6, 0x3, 0x3, 0x0, 800, 25000},
	{/* PMIC_LDO2 */  0x41, 0x0, 0x3F, 0x41, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO3 */  0x42, 0x0, 0x3F, 0x42, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO4 */  0x43, 0x0, 0x3F, 0x43, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO5 */  0x44, 0x0, 0x3F, 0x44, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO6 */  0x45, 0x0, 0x3F, 0x45, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO7 */  0x46, 0x0, 0x3F, 0x46, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO8 */  0x47, 0x0, 0x3F, 0x47, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO9 */  0x48, 0x0, 0x3F, 0x48, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO10 */ 0x49, 0x0, 0x3F, 0x49, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO11 */ 0x4A, 0x0, 0x3F, 0x4A, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO12 */ 0x4B, 0x0, 0x3F, 0x4B, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO13 */ 0x4C, 0x0, 0x3F, 0x4C, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO14 */ 0x4D, 0x0, 0x3F, 0x4D, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO15 */ 0x4E, 0x0, 0x3F, 0x4E, 0x6, 0x3, 0x1, 0x0, 800, 25000},
	{/* PMIC_LDO16 */ 0x4F, 0x0, 0x3F, 0x4F, 0x6, 0x3, 0x1, 0x0, 800, 50000},
	{/* PMIC_LDO17 */ 0x50, 0x0, 0x3F, 0x50, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO18 */ 0x51, 0x0, 0x3F, 0x51, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO19 */ 0x52, 0x0, 0x3F, 0x52, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO20 */ 0x53, 0x0, 0x3F, 0x53, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO21 */ 0x54, 0x0, 0x3F, 0x54, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO22 */ 0x55, 0x0, 0x3F, 0x55, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO23 */ 0x56, 0x0, 0x3F, 0x56, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO24 */ 0x57, 0x0, 0x3F, 0x57, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO25 */ 0x58, 0x0, 0x3F, 0x58, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_LDO26 */ 0x59, 0x0, 0x3F, 0x59, 0x6, 0x3, 0x3, 0x0, 800, 50000},
	{/* PMIC_EN32KHZ_CP */ 0x0, 0x0, 0x0, 0x7F, 0x1, 0x1, 0x1, 0x0, 0x0, 0x0},
};

/*
 * Write a value to a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_write(unsigned char chip_addr,
					unsigned int reg, unsigned char val)
{
	return i2c_write(chip_addr, reg, 1, &val, 1);
}

/*
 * Read a value from a register
 *
 * @param chip_addr	i2c addr for max77686
 * @param reg		reg number to write
 * @param val		value to be written
 *
 */
static inline int max77686_i2c_read(unsigned char chip_addr,
					unsigned int reg, unsigned char *val)
{
	return i2c_read(chip_addr, reg, 1, val, 1);
}

/* Chip register numbers (not exported from this module) */
enum {
	REG_BBAT		= 0x7e,

	/* Bits for BBAT */
	BBAT_BBCHOSTEN_MASK	= 1 << 0,
	BBAT_BBCVS_SHIFT	= 3,
	BBAT_BBCVS_MASK		= 3 << BBAT_BBCVS_SHIFT,
};

int max77686_disable_backup_batt(void)
{
	unsigned char val;
	int ret;

	//i2c_set_bus_num(0);
	ret = max77686_i2c_read(MAX77686_I2C_ADDR, REG_BBAT, &val);
	if (ret) {
//		debug("max77686 i2c read failed\n");
		return ret;
	}

	/* If we already have the correct values, exit */
	if ((val & (BBAT_BBCVS_MASK | BBAT_BBCHOSTEN_MASK)) ==
			BBAT_BBCVS_MASK)
		return 0;

	/* First disable charging */
	val &= ~BBAT_BBCHOSTEN_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	/* Finally select 3.5V to minimize power consumption */
	val |= BBAT_BBCVS_MASK;
	ret = max77686_i2c_write(MAX77686_I2C_ADDR, REG_BBAT, val);
	if (ret) {
		//debug("max77686 i2c write failed\n");
		return -1;
	}

	return 0;
}

static int max77686_enablereg(enum max77686_regnum reg, int enable)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int ret;

	pmic = &max77686_param[reg];

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->reg_enaddr,
				&read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (enable == REG_DISABLE) {
		clrbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos);
	} else {
		clrsetbits_8(&read_data,
				pmic->reg_enbitmask << pmic->reg_enbitpos,
				pmic->reg_enbiton << pmic->reg_enbitpos);
	}

	ret = max77686_i2c_write(MAX77686_I2C_ADDR,
				 pmic->reg_enaddr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	return 0;
}

static int max77686_do_volsetting(enum max77686_regnum reg, unsigned int volt,
				  int enable, int volt_units)
{
	struct max77686_para *pmic;
	unsigned char read_data;
	int vol_level = 0;
	int ret;

	pmic = &max77686_param[reg];

	if (pmic->vol_addr == 0) {
		//debug("not a voltage register.\n");
		return -1;
	}

	ret = max77686_i2c_read(MAX77686_I2C_ADDR, pmic->vol_addr, &read_data);
	if (ret != 0) {
		//debug("max77686 i2c read failed.\n");
		return -1;
	}

	if (volt_units == MAX77686_UV)
		vol_level = volt - (u32)pmic->vol_min * 1000;
	else
		vol_level = (volt - (u32)pmic->vol_min) * 1000;

	if (vol_level < 0) {
		//debug("Not a valid voltage level to set\n");
		return -1;
	}
	vol_level /= (u32)pmic->vol_div;

	clrsetbits_8(&read_data, pmic->vol_bitmask << pmic->vol_bitpos,
			vol_level << pmic->vol_bitpos);

	ret = max77686_i2c_write(MAX77686_I2C_ADDR, pmic->vol_addr, read_data);
	if (ret != 0) {
		//debug("max77686 i2c write failed.\n");
		return -1;
	}

	ret = max77686_enablereg(reg, enable);
	if (ret != 0) {
		//debug("Failed to enable buck/ldo.\n");
		return -1;
	}
	return 0;
}

int max77686_volsetting(enum max77686_regnum reg, unsigned int volt,
			int enable, int volt_units)
{
//	int old_bus = i2c_get_bus_num();
	int ret;

//	i2c_set_bus_num(0);
	ret = max77686_do_volsetting(reg, volt, enable, volt_units);
//	i2c_set_bus_num(old_bus);
	return ret;
}

/* power_init() stuff */
static void power_init(void)
{
	max77686_disable_backup_batt();

	max77686_volsetting(PMIC_BUCK2, CONFIG_VDD_ARM_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK3, CONFIG_VDD_INT_UV,
						REG_ENABLE, MAX77686_UV);
	max77686_volsetting(PMIC_BUCK1, CONFIG_VDD_MIF_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_BUCK4, CONFIG_VDD_G3D_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO2, CONFIG_VDD_LDO2_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO3, CONFIG_VDD_LDO3_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO5, CONFIG_VDD_LDO5_MV,
						REG_ENABLE, MAX77686_MV);
	max77686_volsetting(PMIC_LDO10, CONFIG_VDD_LDO10_MV,
						REG_ENABLE, MAX77686_MV);
}

void __div0 (void);
void __div0 (void)
{
	die("divide by zero detected");
	while(1) ;
}

void bootblock_mainboard_init(void);
void bootblock_mainboard_init(void)
{
#if CONFIG_EARLY_SERIAL_CONSOLE
	/* FIXME: we should not need UART in bootblock, this is only
	   done for testing purposes */
	i2c_init(CONFIG_SYS_I2C_SPEED, CONFIG_SYS_I2C_SLAVE);
	power_init();
	system_clock_init();
	do_serial();
	printk(BIOS_INFO, "\n\n\n%s: UART initialized\n", __func__);
#endif
}
