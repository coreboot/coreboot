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

struct rockchip_spi_slave {
	struct spi_slave slave;
	struct rockchip_spi *regs;
};

#define SPI_TIMEOUT_US	1000
#define SPI_SRCCLK_HZ   (99*MHz)
#define SPI_FIFO_DEPTH	32

static struct rockchip_spi_slave rockchip_spi_slaves[3] = {
	{
	 .slave = {
		   .bus = 0,
		   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG,
		   },
	 .regs = (void *)SPI0_BASE,
	},
	{
	 .slave = {.bus = 1, .rw = SPI_READ_FLAG,},
	 .regs = (void *)SPI1_BASE,
	},
	{
	 .slave = {
		   .bus = 2,
		   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG,
		   },
	 .regs = (void *)SPI2_BASE,
	},

};

static struct rockchip_spi_slave *to_rockchip_spi(struct spi_slave *slave)
{
	return container_of(slave, struct rockchip_spi_slave, slave);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	assert(bus >= 0 && bus < 3);
	return &(rockchip_spi_slaves[bus].slave);
}

static void spi_cs_activate(struct spi_slave *slave)
{
	struct rockchip_spi *regs = to_rockchip_spi(slave)->regs;
	setbits_le32(&regs->ser, 1);
}

static void spi_cs_deactivate(struct spi_slave *slave)
{
	struct rockchip_spi *regs = to_rockchip_spi(slave)->regs;
	clrbits_le32(&regs->ser, 1);
}

static void rockchip_spi_enable_chip(struct rockchip_spi *regs, int enable)
{
	if (enable == 1)
		write32(&regs->spienr, 1);
	else
		write32(&regs->spienr, 0);
}

static void rockchip_spi_set_clk(struct rockchip_spi *regs, unsigned int hz)
{
	unsigned short clk_div = SPI_SRCCLK_HZ / hz;
	assert(clk_div * hz == SPI_SRCCLK_HZ && !(clk_div & 1));
	write32(&regs->baudr, clk_div);
}

void rockchip_spi_init(unsigned int bus, unsigned int speed_hz)
{
	struct rockchip_spi *regs = rockchip_spi_slaves[bus].regs;
	unsigned int ctrlr0 = 0;

	rkclk_configure_spi(bus, SPI_SRCCLK_HZ);
	rockchip_spi_enable_chip(regs, 0);
	rockchip_spi_set_clk(regs, speed_hz);

	/* Operation Mode */
	ctrlr0 = (SPI_OMOD_MASTER << SPI_OMOD_OFFSET);

	/* Data Frame Size */
	ctrlr0 |= SPI_DFS_8BIT << SPI_DFS_OFFSET;

	/* Chip Select Mode */
	ctrlr0 |= (SPI_CSM_KEEP << SPI_CSM_OFFSET);

	/* SSN to Sclk_out delay */
	ctrlr0 |= (SPI_SSN_DELAY_ONE << SPI_SSN_DELAY_OFFSET);

	/* Serial Endian Mode */
	ctrlr0 |= (SPI_SEM_LITTLE << SPI_SEM_OFFSET);

	/* First Bit Mode */
	ctrlr0 |= (SPI_FBM_MSB << SPI_FBM_OFFSET);

	/* Byte and Halfword Transform */
	ctrlr0 |= (SPI_APB_8BIT << SPI_HALF_WORLD_TX_OFFSET);

	/* Rxd Sample Delay */
	ctrlr0 |= (0 << SPI_RXDSD_OFFSET);

	/* Frame Format */
	ctrlr0 |= (SPI_FRF_SPI << SPI_FRF_OFFSET);

	write32(&regs->ctrlr0, ctrlr0);

	/* fifo depth */
	write32(&regs->txftlr, SPI_FIFO_DEPTH / 2 - 1);
	write32(&regs->rxftlr, SPI_FIFO_DEPTH / 2 - 1);
}

int spi_claim_bus(struct spi_slave *slave)
{
	spi_cs_activate(slave);
	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	spi_cs_deactivate(slave);
}

static int rockchip_spi_wait_till_not_busy(struct rockchip_spi *regs)
{
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, SPI_TIMEOUT_US);
	do {
		if (!(read32(&regs->sr) & SR_BUSY))
			return 0;
	} while (!stopwatch_expired(&sw));
	printk(BIOS_DEBUG,
	       "RK SPI: Status keeps busy for 1000us after a read/write!\n");
	return -1;
}

static void set_tmod(struct rockchip_spi *regs, unsigned int tmod)
{
	clrsetbits_le32(&regs->ctrlr0, SPI_TMOD_MASK << SPI_TMOD_OFFSET,
				      tmod << SPI_TMOD_OFFSET);
}

static void set_transfer_mode(struct rockchip_spi *regs,
		unsigned int sout, unsigned int sin)
{
	if (!sin && !sout)
		return;
	else if (sin && sout)
		set_tmod(regs, SPI_TMOD_TR);	/* tx and rx */
	else if (!sin)
		set_tmod(regs, SPI_TMOD_TO);	/* tx only */
	else if (!sout)
		set_tmod(regs, SPI_TMOD_RO);	/* rx only */
}

/* returns 0 to indicate success, <0 otherwise */
static int do_xfer(struct spi_slave *slave, const void *dout,
	unsigned int *bytes_out, void *din, unsigned int *bytes_in)
{
	struct rockchip_spi *regs = to_rockchip_spi(slave)->regs;
	uint8_t *in_buf = din;
	uint8_t *out_buf = (uint8_t *)dout;
	unsigned int min_xfer;

	if (*bytes_out == 0)
		min_xfer = *bytes_in;
	else if (*bytes_in == 0)
		min_xfer = *bytes_out;
	else
		min_xfer = MIN(*bytes_in, *bytes_out);

	while (min_xfer) {
		uint32_t sr = read32(&regs->sr);
		int xferred = 0;	/* in either (or both) directions */

		if (*bytes_out && !(sr & SR_TF_FULL)) {
			write32(&regs->txdr, *out_buf);
			out_buf++;
			*bytes_out -= 1;
			xferred = 1;
		}

		if (*bytes_in && !(sr & SR_RF_EMPT)) {
			*in_buf = read32(&regs->rxdr) & 0xff;
			in_buf++;
			*bytes_in -= 1;
			xferred = 1;
		}

		min_xfer -= xferred;
	}

	if (rockchip_spi_wait_till_not_busy(regs)) {
		printk(BIOS_ERR, "Timed out waiting on SPI transfer\n");
		return -1;
	}

	return 0;
}

unsigned int spi_crop_chunk(unsigned int cmd_len, unsigned int buf_len)
{
	return min(65535, buf_len);
}

int spi_xfer(struct spi_slave *slave, const void *dout,
		unsigned int bytes_out, void *din, unsigned int bytes_in)
{
	struct rockchip_spi *regs = to_rockchip_spi(slave)->regs;
	int ret = 0;

	/*
	 * RK3288 SPI controller can transfer up to 65536 data frames (bytes
	 * in our case) continuously. Break apart large requests as necessary.
	 *
	 * FIXME: And by 65536, we really mean 65535. If 0xffff is written to
	 * ctrlr1, all bytes that we see in rxdr end up being 0x00. 0xffff - 1
	 * seems to work fine.
	 */
	while (bytes_out || bytes_in) {
		unsigned int in_now = MIN(bytes_in, 0xffff);
		unsigned int out_now = MIN(bytes_out, 0xffff);
		unsigned int in_rem, out_rem;

		rockchip_spi_enable_chip(regs, 0);

		/* Enable/disable transmitter and receiver as needed to
		 * avoid sending or reading spurious bits. */
		set_transfer_mode(regs, bytes_out, bytes_in);

		/* MAX() in case either counter is 0 */
		write32(&regs->ctrlr1, MAX(in_now, out_now) - 1);

		rockchip_spi_enable_chip(regs, 1);

		in_rem = in_now;
		out_rem = out_now;
		ret = do_xfer(slave, dout, &out_rem, din, &in_rem);
		if (ret < 0)
			break;

		if (bytes_out) {
			unsigned int sent = out_now - out_rem;
			bytes_out -= sent;
			dout += sent;
		}

		if (bytes_in) {
			unsigned int received = in_now - in_rem;
			bytes_in -= received;
			din += received;
		}
	}

	rockchip_spi_enable_chip(regs, 0);
	return ret < 0 ? ret : 0;
}
