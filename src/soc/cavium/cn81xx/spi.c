/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018       Facebook, Inc.
 * Copyright 2003-2017  Cavium Inc.  <support@cavium.com>
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
 * Derived from Cavium's BSD-3 Clause OCTEONTX-SDK-6.2.0.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <endian.h>
#include <soc/addressmap.h>
#include <soc/spi.h>
#include <soc/clock.h>
#include <spi-generic.h>
#include <spi_flash.h>
#include <stdlib.h>
#include <timer.h>

union cavium_spi_cfg {
	u64 u;
	struct {
		u64 enable	: 1;
		u64 idlelow	: 1;
		u64 clk_cont	: 1;
		u64 wireor	: 1;
		u64 lsbfirst	: 1;
		u64		: 2;
		u64 cshi	: 1;
		u64 idleclks	: 2;
		u64 tristate	: 1;
		u64 cslate	: 1;
		u64 csena	: 4; /* Must be one */
		u64 clkdiv	: 13;
		u64 		: 35;
	} s;
};

union cavium_spi_sts {
	u64 u;
	struct {
		u64 busy	: 1;
		u64 mpi_intr	: 1;
		u64		: 6;
		u64 rxnum	: 5;
		u64		: 51;
	} s;
};

union cavium_spi_tx {
	u64 u;
	struct {
		u64 totnum	: 5;
		u64		: 3;
		u64 txnum	: 5;
		u64		: 3;
		u64 leavecs	: 1;
		u64		: 3;
		u64 csid	: 2;
		u64 		: 42;
	} s;
};

struct cavium_spi {
	union cavium_spi_cfg cfg;
	union cavium_spi_sts sts;
	union cavium_spi_tx tx;
	u64	rsvd1;
	u64	sts_w1s;
	u64	rsvd2;
	u64	int_ena_w1c;
	u64	int_ena_w1s;
	u64	wide_dat;
	u8	rsvd4[0x38];
	u64	dat[8];
};

check_member(cavium_spi, cfg, 0);
check_member(cavium_spi, sts, 0x8);
check_member(cavium_spi, tx, 0x10);
check_member(cavium_spi, dat[7], 0xb8);

struct cavium_spi_slave {
	struct cavium_spi *regs;
	int cs;
};

#define SPI_TIMEOUT_US	5000

static struct cavium_spi_slave cavium_spi_slaves[] = {
	{
	 .regs = (struct cavium_spi *)MPI_PF_BAR0,
	 .cs = 0,
	},
};

static struct cavium_spi_slave *to_cavium_spi(const struct spi_slave *slave)
{
	assert(slave->bus < ARRAY_SIZE(cavium_spi_slaves));
	return &cavium_spi_slaves[slave->bus];
}

/**
 * Enable the SPI controller. Pins are driven.
 *
 * @param bus                 The SPI bus to operate on
 */
void spi_enable(const size_t bus)
{
	union cavium_spi_cfg cfg;

	assert(bus < ARRAY_SIZE(cavium_spi_slaves));
	if (bus >= ARRAY_SIZE(cavium_spi_slaves))
		return;

	struct cavium_spi *regs = cavium_spi_slaves[bus].regs;

	cfg.u = read64(&regs->cfg);
	cfg.s.csena = 0xf;
	cfg.s.enable = 1;
	write64(&regs->cfg, cfg.u);
}

/**
 * Disable the SPI controller. Pins are tristated.
 *
 * @param bus                 The SPI bus to operate on
 */
void spi_disable(const size_t bus)
{
	union cavium_spi_cfg cfg;

	assert(bus < ARRAY_SIZE(cavium_spi_slaves));
	if (bus >= ARRAY_SIZE(cavium_spi_slaves))
		return;

	struct cavium_spi *regs = cavium_spi_slaves[bus].regs;

	cfg.u = read64(&regs->cfg);
	cfg.s.csena = 0xf;
	cfg.s.enable = 0;
	write64(&regs->cfg, cfg.u);
}

/**
 * Set SPI Chip select line and level if asserted.
 *
 * @param bus                 The SPI bus to operate on
 * @param chip_select         The chip select pin to use (0 - 3)
 * @param assert_is_low       CS pin state is low when asserted
 */
void spi_set_cs(const size_t bus,
		const size_t chip_select,
		const size_t assert_is_low)
{
	union cavium_spi_cfg cfg;

	assert(bus < ARRAY_SIZE(cavium_spi_slaves));
	if (bus >= ARRAY_SIZE(cavium_spi_slaves))
		return;

	cavium_spi_slaves[bus].cs = chip_select & 0x3;
	struct cavium_spi *regs = cavium_spi_slaves[bus].regs;

	cfg.u = read64(&regs->cfg);
	cfg.s.csena = 0xf;
	cfg.s.cshi = !assert_is_low;
	write64(&regs->cfg, cfg.u);

	//FIXME: CS2/3: Change pin mux here
}

/**
 * Set SPI clock frequency.
 *
 * @param bus                 The SPI bus to operate on
 * @param speed_hz            The SPI frequency in Hz
 * @param idle_low            The SPI clock idles low
 * @param idle_cycles         Number of CLK cycles between two commands (0 - 3)

 */
void spi_set_clock(const size_t bus,
		   const size_t speed_hz,
		   const size_t idle_low,
		   const size_t idle_cycles)
{
	union cavium_spi_cfg cfg;

	assert(bus < ARRAY_SIZE(cavium_spi_slaves));
	if (bus >= ARRAY_SIZE(cavium_spi_slaves))
		return;

	struct cavium_spi *regs = cavium_spi_slaves[bus].regs;
	const uint64_t sclk = thunderx_get_io_clock();

	cfg.u = read64(&regs->cfg);
	cfg.s.csena = 0xf;
	cfg.s.clk_cont = 0;
	cfg.s.idlelow = !!idle_low;
	cfg.s.idleclks = idle_cycles & 0x3;
	cfg.s.clkdiv = MIN(sclk / (2ULL * speed_hz), 0x1fff);
	write64(&regs->cfg, cfg.u);

	printk(BIOS_DEBUG, "SPI: set clock to %lld kHz\n",
	       (sclk / (2ULL * cfg.s.clkdiv)) >> 10);
}

/**
 * Set SPI LSB/MSB first.
 *
 * @param bus                 The SPI bus to operate on
 * @param lsb_first           The SPI operates LSB first
 *
 */
void spi_set_lsbmsb(const size_t bus, const size_t lsb_first)
{
	union cavium_spi_cfg cfg;

	assert(bus < ARRAY_SIZE(cavium_spi_slaves));
	if (bus >= ARRAY_SIZE(cavium_spi_slaves))
		return;

	struct cavium_spi *regs = cavium_spi_slaves[bus].regs;

	cfg.u = read64(&regs->cfg);
	cfg.s.csena = 0xf;
	cfg.s.lsbfirst = !!lsb_first;
	write64(&regs->cfg, cfg.u);
}

/**
 * Init SPI with custom parameters and enable SPI controller.
 *
 * @param bus                 The SPI bus to operate on
 * @param speed_hz            The SPI frequency in Hz
 * @param idle_low            The SPI clock idles low
 * @param idle_cycles         Number of CLK cycles between two commands (0 - 3)
 * @param lsb_first           The SPI operates LSB first
 * @param chip_select         The chip select pin to use (0 - 3)
 * @param assert_is_low       CS pin state is low when asserted
 */
void spi_init_custom(const size_t bus,
		     const size_t speed_hz,
		     const size_t idle_low,
		     const size_t idle_cycles,
		     const size_t lsb_first,
		     const size_t chip_select,
		     const size_t assert_is_low)
{
	spi_disable(bus);
	spi_set_clock(bus, speed_hz, idle_low, idle_cycles);
	spi_set_lsbmsb(bus, lsb_first);
	spi_set_cs(bus, chip_select, assert_is_low);
	spi_enable(bus);
}

/**
 * Init all SPI controllers with default values and enable all SPI controller.
 *
 */
void spi_init(void)
{
	for (size_t i = 0; i < ARRAY_SIZE(cavium_spi_slaves); i++) {
		spi_disable(i);
		spi_set_clock(i, 12500000, 0, 0);
		spi_set_lsbmsb(i, 0);
		spi_set_cs(i, 0, 1);
		spi_enable(i);
	}
}

static int cavium_spi_wait(struct cavium_spi *regs)
{
	struct stopwatch sw;
	union cavium_spi_sts sts;

	stopwatch_init_usecs_expire(&sw, SPI_TIMEOUT_US);
	do {
		sts.u = read64(&regs->sts);
		if (!sts.s.busy)
			return 0;
	} while (!stopwatch_expired(&sw));
	printk(BIOS_DEBUG, "SPI: Timed out after %uus\n", SPI_TIMEOUT_US);
	return -1;
}

static int do_xfer(const struct spi_slave *slave, struct spi_op *vector,
		int leavecs)
{
	struct cavium_spi *regs = to_cavium_spi(slave)->regs;
	uint8_t *out_buf = (uint8_t *)vector->dout;
	size_t bytesout = vector->bytesout;
	uint8_t *in_buf = (uint8_t *)vector->din;
	size_t bytesin = vector->bytesin;
	union cavium_spi_sts sts;
	union cavium_spi_tx tx;

	/**
	 * The CN81xx SPI controller is half-duplex and has 8 data registers.
	 * If >8 bytes remain in the transfer then we must set LEAVECS = 1 so
	 * that the /CS remains asserted. Once <=8 bytes remain we must set
	 * LEAVECS = 0 so that /CS is de-asserted, thus completing the transfer.
	 */
	while (bytesout) {
		size_t out_now = MIN(bytesout, 8);
		unsigned int i;

		for (i = 0; i < out_now; i++)
			write64(&regs->dat[i], out_buf[i] & 0xff);

		tx.u = 0;
		tx.s.csid = to_cavium_spi(slave)->cs;
		if (leavecs || ((bytesout > 8) || bytesin))
			tx.s.leavecs = 1;
		/* number of bytes to transmit goes in both TXNUM and TOTNUM */
		tx.s.totnum = out_now;
		tx.s.txnum = out_now;
		write64(&regs->tx, tx.u);

		/* check status */
		if (cavium_spi_wait(regs) < 0)
			return -1;

		bytesout -= out_now;
		out_buf += out_now;
	}

	while (bytesin) {
		size_t in_now = MIN(bytesin, 8);
		unsigned int i;

		tx.u = 0;
		tx.s.csid = to_cavium_spi(slave)->cs;
		if (leavecs || (bytesin > 8))
			tx.s.leavecs = 1;
		tx.s.totnum = in_now;
		write64(&regs->tx, tx.u);

		/* check status */
		if (cavium_spi_wait(regs) < 0)
			return -1;

		sts.u = read64(&regs->sts);
		if (sts.s.rxnum != in_now) {
			printk(BIOS_ERR,
			       "SPI: Incorrect number of bytes received: %u.\n",
			       sts.s.rxnum);
			return -1;
		}

		for (i = 0; i < in_now; i++) {
			*in_buf = (uint8_t)((read64(&regs->dat[i]) & 0xff));
			in_buf++;
		}
		bytesin -= in_now;
	}

	return 0;
}

static int spi_ctrlr_xfer_vector(const struct spi_slave *slave,
		struct spi_op vectors[], size_t count)
{
	int i;

	for (i = 0; i < count; i++) {
		if (do_xfer(slave, &vectors[i], count - 1 == i ? 0 : 1)) {
			printk(BIOS_ERR,
			       "SPI: Failed to transfer %zu vectors.\n", count);
			return -1;
		}
	}

	return 0;
}
static const struct spi_ctrlr spi_ctrlr = {

	.xfer_vector = spi_ctrlr_xfer_vector,
	.max_xfer_size = SPI_CTRLR_DEFAULT_MAX_XFER_SIZE,
};

const struct spi_ctrlr_buses spi_ctrlr_bus_map[] = {
	{
		.ctrlr = &spi_ctrlr,
		.bus_start = 0,
		.bus_end = ARRAY_SIZE(cavium_spi_slaves) - 1,
	},
};
const size_t spi_ctrlr_bus_map_count = ARRAY_SIZE(spi_ctrlr_bus_map);
