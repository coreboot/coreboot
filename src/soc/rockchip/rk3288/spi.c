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
#include <spi-generic.h>
#include <spi_flash.h>
#include <delay.h>
#include <timer.h>
#include <endian.h>
#include "addressmap.h"
#include "spi.h"
#include "clock.h"

struct rockchip_spi_slave {
	struct spi_slave slave;
	struct rockchip_spi *regs;
	unsigned int fifo_size;
};

#define SPI_TIMEOUT_US	1000
#define SPI_SRCCLK_HZ   99000000

static struct rockchip_spi_slave rockchip_spi_slaves[3] = {
	{
	 .slave = {
		   .bus = 0,
		   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG,
		   },
	 .regs = (void *)SPI0_BASE,
	 .fifo_size = 32,
	},
	{
	 .slave = {.bus = 1, .rw = SPI_READ_FLAG,},
	 .regs = (void *)SPI1_BASE,
	 .fifo_size = 32,
	},
	{
	 .slave = {
		   .bus = 2,
		   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG,
		   },
	 .regs = (void *)SPI2_BASE,
	 .fifo_size = 32,
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
		writel(1, &regs->spienr);
	else
		writel(0, &regs->spienr);
}

static void rockchip_spi_set_clk(struct rockchip_spi *regs, unsigned int hz)
{
	unsigned short clk_div = 0;

	/* Calculate clock divisor.  */
	clk_div = SPI_SRCCLK_HZ / hz;
	clk_div = (clk_div + 1) & 0xfffe;
	assert((clk_div - 1) * hz == SPI_SRCCLK_HZ);
	writel(clk_div, &regs->baudr);
}

void rockchip_spi_init(unsigned int bus, unsigned int speed_hz)
{
	struct rockchip_spi_slave *espi = &rockchip_spi_slaves[bus];
	struct rockchip_spi *regs = espi->regs;
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

	writel(ctrlr0, &regs->ctrlr0);

	/*fifo depth */
	writel(espi->fifo_size / 2 - 1, &regs->txftlr);
	writel(espi->fifo_size / 2 - 1, &regs->rxftlr);
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
		if (!(readl(&regs->sr) & SR_BUSY))
			return 0;
	} while (!stopwatch_expired(&sw));
	printk(BIOS_DEBUG,
	       "RK SPI: Status keeps busy for 1000us after a read/write!\n");
	return -1;
}

int spi_xfer(struct spi_slave *slave, const void *dout, unsigned int sout,
	     void *din, unsigned int sin)
{
	unsigned int len;
	unsigned int bytes_remaining;
	uint8_t *p;
	struct rockchip_spi *regs = to_rockchip_spi(slave)->regs;
	struct rockchip_spi_slave *espi = to_rockchip_spi(slave);

	if (dout) {
		len = sout;
		p = (uint8_t *) dout;
		bytes_remaining = len;
		writel(0, &regs->spienr);	/*disable spi */
		writel(len - 1, &regs->ctrlr1);	/*wrtie len */

		/*tx only */
		clrsetbits_le32(&regs->ctrlr0, SPI_TMOD_MASK << SPI_TMOD_OFFSET,
					      SPI_TMOD_TO << SPI_TMOD_OFFSET);
		writel(1, &regs->spienr);/*enable spi */
		while (bytes_remaining) {
			if ((readl(&regs->txflr) & 0x3f)
			    < espi->fifo_size) {
				writel(*p++, &regs->txdr);
				bytes_remaining--;
			}
		}
		if (rockchip_spi_wait_till_not_busy(regs))
			return -1;
	}
	if (din) {
		len = sin;
		p = (uint8_t *) din;
		writel(0, &regs->spienr);	/*disable spi */
		writel(len - 1, &regs->ctrlr1);	/*write len */

		/*rx only */
		clrsetbits_le32(&regs->ctrlr0, SPI_TMOD_MASK << SPI_TMOD_OFFSET,
					      SPI_TMOD_RO << SPI_TMOD_OFFSET);
		while (len) {
			writel(0, &regs->spienr);/*disable spi */
			bytes_remaining = MIN(len, 0xffff);
			writel(bytes_remaining - 1, &regs->ctrlr1);
			len -= bytes_remaining;
			writel(1, &regs->spienr);/*enable spi */
			while (bytes_remaining) {
				if (readl(&regs->rxflr) & 0x3f) {
					*p = readl(&regs->rxdr) & 0xff;
					p += 1;
					bytes_remaining--;
				}
			}
		}
		if (rockchip_spi_wait_till_not_busy(regs))
			return -1;
	}
	return 0;
}

static int rockchip_spi_read(struct spi_slave *slave, void *dest, uint32_t len,
			     uint32_t off)
{
	unsigned int cmd;

	spi_claim_bus(slave);
	cmd = swab32(off) | SF_READ_DATA_CMD;
	if (spi_xfer(slave, &cmd, sizeof(cmd), dest, len)) {
		printk(BIOS_DEBUG, "rockchip_spi_read err\n");
		spi_release_bus(slave);
		return -1;
	}
	spi_release_bus(slave);
	return len;
}

struct rockchip_spi_media {
	struct spi_slave *slave;
	struct cbfs_simple_buffer buffer;
};

static int rockchip_spi_cbfs_open(struct cbfs_media *media)
{
	return 0;
}

static int rockchip_spi_cbfs_close(struct cbfs_media *media)
{
	return 0;
}

static size_t rockchip_spi_cbfs_read(struct cbfs_media *media, void *dest,
				     size_t offset, size_t count)
{
	struct rockchip_spi_media *spi =
	    (struct rockchip_spi_media *)media->context;

	return rockchip_spi_read(spi->slave, dest, count, offset);
}

static void *rockchip_spi_cbfs_map(struct cbfs_media *media, size_t offset,
				   size_t count)
{
	struct rockchip_spi_media *spi =
	    (struct rockchip_spi_media *)media->context;

	count = ALIGN_UP(count, 4);

	return cbfs_simple_buffer_map(&spi->buffer, media, offset, count);
}

static void *rockchip_spi_cbfs_unmap(struct cbfs_media *media,
				     const void *address)
{
	struct rockchip_spi_media *spi =
	    (struct rockchip_spi_media *)media->context;
	return cbfs_simple_buffer_unmap(&spi->buffer, address);
}

int initialize_rockchip_spi_cbfs_media(struct cbfs_media *media,
				       void *buffer_address, size_t buffer_size)
{
	static struct rockchip_spi_media context;
	static struct rockchip_spi_slave *eslave = &rockchip_spi_slaves[2];
	context.slave = &eslave->slave;
	context.buffer.allocated = context.buffer.last_allocate = 0;
	context.buffer.buffer = buffer_address;
	context.buffer.size = buffer_size;
	media->context = (void *)&context;
	media->open = rockchip_spi_cbfs_open;
	media->close = rockchip_spi_cbfs_close;
	media->read = rockchip_spi_cbfs_read;
	media->map = rockchip_spi_cbfs_map;
	media->unmap = rockchip_spi_cbfs_unmap;

	return 0;
}
