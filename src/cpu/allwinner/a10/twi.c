/*
 * Setup helpers for Two Wire Interface (TWI) (I²C) Allwinner CPUs
 *
 * Only functionality for I²C master is provided.
 * Largely based on the uboot-sunxi code.
 *
 * Copyright (C) 2012 Henrik Nordstrom <henrik@henriknordstrom.net>
 * Copyright (C) 2013 Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#include "memmap.h"
#include "twi.h"

#include <arch/io.h>
#include <delay.h>
#include <device/i2c.h>

#define TWI_BASE(n)			(A1X_TWI0_BASE + 0x400 * (n))

#define TWI_TIMEOUT			(50 * 1000)

static u8 is_busy(struct a1x_twi *twi)
{
	return (read32(&twi->stat) != TWI_STAT_IDLE);
}

static enum cb_err wait_until_idle(struct a1x_twi *twi)
{
	u32 i = TWI_TIMEOUT;
	while (i-- && is_busy((twi)))
		udelay(1);
	return i ? CB_SUCCESS : CB_ERR;
}

/* FIXME: This function is basic, and unintelligent */
static void configure_clock(struct a1x_twi *twi, u32 speed_hz)
{
	/* FIXME: We assume clock is 24MHz, which may not be the case */
	u32 apb_clk = 24000000, m, n;

	/* Pre-divide the clock by 8 */
	n = 3;
	m = (apb_clk >> n) / speed_hz;
	write32(&twi->clk, TWI_CLK_M(m) | TWI_CLK_N(n));
}

void a1x_twi_init(u8 bus, u32 speed_hz)
{
	u32 i = TWI_TIMEOUT;
	struct a1x_twi *twi = (void *)TWI_BASE(bus);

	configure_clock(twi, speed_hz);

	/* Enable the I²C bus */
	write32(&twi->ctl, TWI_CTL_BUS_EN);
	/* Issue soft reset */
	write32(&twi->reset, 1);

	while (i-- && read32(&twi->reset))
		udelay(1);
}

static void clear_interrupt_flag(struct a1x_twi *twi)
{
	write32(&twi->ctl, read32(&twi->ctl) & ~TWI_CTL_INT_FLAG);
}

static void i2c_send_data(struct a1x_twi *twi, u8 data)
{
	write32(&twi->data, data);
	clear_interrupt_flag(twi);
}

static enum twi_status wait_for_status(struct a1x_twi *twi)
{
	u32 i = TWI_TIMEOUT;
	/* Wait until interrupt is asserted again */
	while (i-- && !(read32(&twi->ctl) & TWI_CTL_INT_FLAG))
		udelay(1);
	/* A timeout here most likely indicates a bus error */
	return i ? read32(&twi->stat) : TWI_STAT_BUS_ERROR;
}

static void i2c_send_start(struct a1x_twi *twi)
{
	u32 reg32, i;

	/* Send START condition */
	reg32 = read32(&twi->ctl);
	reg32 &= ~TWI_CTL_INT_FLAG;
	reg32 |= TWI_CTL_M_START;
	write32(&twi->ctl, reg32);

	/* M_START is automatically cleared after condition is transmitted */
	i = TWI_TIMEOUT;
	while (i-- && (read32(&twi->ctl) & TWI_CTL_M_START))
		udelay(1);
}

static void i2c_send_stop(struct a1x_twi *twi)
{
	u32 reg32;

	/* Send STOP condition */
	reg32 = read32(&twi->ctl);
	reg32 &= ~TWI_CTL_INT_FLAG;
	reg32 |= TWI_CTL_M_STOP;
	write32(&twi->ctl, reg32);
}

static int i2c_read(struct a1x_twi *twi, uint8_t chip,
			uint8_t *buf, size_t len)
{
	unsigned count = len;
	enum twi_status expected_status;

	/* Send restart for read */
	i2c_send_start(twi);
	if (wait_for_status(twi) != TWI_STAT_TX_RSTART)
		return CB_ERR;

	/* Send chip address */
	i2c_send_data(twi, chip << 1 | 1);
	if (wait_for_status(twi) != TWI_STAT_TX_AR_ACK)
		return CB_ERR;

	/* Start ACK-ing received data */
	setbits_le32(&twi->ctl, TWI_CTL_A_ACK);
	expected_status = TWI_STAT_RXD_ACK;

	/* Read data */
	while (count > 0) {
		if (count == 1) {
			/* Do not ACK the last byte */
			clrbits_le32(&twi->ctl, TWI_CTL_A_ACK);
			expected_status = TWI_STAT_RXD_NAK;
		}

		clear_interrupt_flag(twi);

		if (wait_for_status(twi) != expected_status)
			return CB_ERR;

		*buf++ = read32(&twi->data);
		count--;
	}

	return len;
}

static int i2c_write(struct a1x_twi *twi, uint8_t chip,
		     const uint8_t *buf, size_t len)
{
	size_t count = len;

	i2c_send_start(twi);
	if (wait_for_status(twi) != TWI_STAT_TX_START)
		return CB_ERR;

	/* Send chip address */
	i2c_send_data(twi, chip << 1);
	if (wait_for_status(twi) != TWI_STAT_TX_AW_ACK)
		return CB_ERR;

	/* Send data */
	while (count > 0) {
		i2c_send_data(twi, *buf++);
		if (wait_for_status(twi) != TWI_STAT_TXD_ACK)
			return CB_ERR;
		count--;
	}

	return len;
}

int platform_i2c_transfer(unsigned bus, struct i2c_seg *segments, int count)
{
	int i, ret = CB_SUCCESS;
	struct i2c_seg *seg = segments;
	struct a1x_twi *twi = (void *)TWI_BASE(bus);


	if (wait_until_idle(twi) != CB_SUCCESS)
		return CB_ERR;

	for (i = 0; i < count; i++) {
		seg = segments + i;

		if (seg->read) {
			ret = i2c_read(twi, seg->chip, seg->buf, seg->len);
			if (ret < 0)
				break;
		} else {
			ret = i2c_write(twi, seg->chip, seg->buf, seg->len);
			if (ret < 0)
				break;
		}
	}

	/* Don't worry about the status. STOP is on a best-effort basis */
	i2c_send_stop(twi);

	return ret;
}
