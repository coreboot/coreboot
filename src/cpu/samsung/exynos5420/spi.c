/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2011 Samsung Electronics
 * Copyright 2013 Google Inc.
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
#include <spi_flash.h>

#include "cpu.h"
#include "spi.h"

#define EXYNOS_SPI_MAX_TRANSFER_BYTES (65535)

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "EXYNOS_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

struct exynos_spi_slave {
	struct spi_slave slave;
	struct exynos_spi *regs;
	unsigned int fifo_size;
	uint8_t half_duplex;
	uint8_t frame_header;  /* header byte to detect in half-duplex mode. */
};

/* TODO(hungte) Move the SPI param list to per-board configuration, probably
 * Kconfig or mainboard.c */
static struct exynos_spi_slave exynos_spi_slaves[3] = {
	// SPI 0
	{
		.slave = { .bus = 0, },
		.regs = samsung_get_base_spi0(),
	},
	// SPI 1
	{
		.slave = { .bus = 1, .rw = SPI_READ_FLAG, },
		.regs = samsung_get_base_spi1(),
		.fifo_size = 64,
		.half_duplex = 0,
	},
	// SPI 2
	{
		.slave = { .bus = 2,
			   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG, },
		.regs = samsung_get_base_spi2(),
		.fifo_size = 64,
		.half_duplex = 1,
		.frame_header = 0xec,
	},
};

static inline struct exynos_spi_slave *to_exynos_spi(struct spi_slave *slave)
{
	return container_of(slave, struct exynos_spi_slave, slave);
}

void spi_init(void)
{
	printk(BIOS_INFO, "Exynos SPI driver initiated.\n");
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs,
				  unsigned int max_hz, unsigned int mode)
{
	ASSERT(bus >= 0 && bus < 3);
	return &(exynos_spi_slaves[bus].slave);
}

int spi_cs_is_valid(unsigned int bus, unsigned int cs)
{
	return bus > 0 && bus < 3;
}

void spi_cs_activate(struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	// TODO(hungte) Add some delay if too many transactions happen at once.
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
}

void spi_cs_deactivate(struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
}

static inline void exynos_spi_soft_reset(struct exynos_spi *regs)
{
	/* The soft reset clears only FIFO and status register.
	 * All special function registers are not changed. */
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
}

static inline void exynos_spi_flush_fifo(struct exynos_spi *regs)
{
	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
	clrbits_le32(&regs->ch_cfg, SPI_CH_HS_EN);
	exynos_spi_soft_reset(regs);
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
}

static void exynos_spi_request_bytes(struct exynos_spi *regs, int count,
				     int width)
{
	uint32_t mode_word = SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD,
		 swap_word = (SPI_TX_SWAP_EN | SPI_RX_SWAP_EN |
			      SPI_TX_BYTE_SWAP | SPI_RX_BYTE_SWAP |
			      SPI_TX_HWORD_SWAP | SPI_RX_HWORD_SWAP);

	/* For word address we need to swap bytes */
	if (width == sizeof(uint32_t)) {
		setbits_le32(&regs->mode_cfg, mode_word);
		setbits_le32(&regs->swap_cfg, swap_word);
		count /= width;
	} else {
		/* Select byte access and clear the swap configuration */
		clrbits_le32(&regs->mode_cfg, mode_word);
		writel(0, &regs->swap_cfg);
	}

	exynos_spi_soft_reset(regs);

	if (count) {
		ASSERT(count < (1 << 16));
		writel(count | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
	} else {
		writel(0, &regs->pkt_cnt);
	}
}

static int spi_rx_tx(struct spi_slave *slave, uint8_t *rxp, int rx_bytes,
		     const uint8_t *txp, int tx_bytes)
{
	struct exynos_spi_slave *espi = to_exynos_spi(slave);
	struct exynos_spi *regs = espi->regs;

	int step;
	int todo = MAX(rx_bytes, tx_bytes);
	int wait_for_frame_header = espi->half_duplex;

	ASSERT(todo < EXYNOS_SPI_MAX_TRANSFER_BYTES);

	/* Select transfer mode. */
	if (espi->half_duplex) {
		step = 1;
	} else if ((rx_bytes | tx_bytes | (uintptr_t)rxp |(uintptr_t)txp) & 3) {
		printk(BIOS_CRIT, "%s: WARNING: tranfer mode decreased to 1B\n",
		       __func__);
		step = 1;
	} else {
		step = sizeof(uint32_t);
	}

	exynos_spi_request_bytes(regs, espi->half_duplex ? 0 : todo, step);

	/* Note: Some device, like ChromeOS EC, tries to work in half-duplex
	 * mode and sends a large amount of data (larger than FIFO size).
	 * Printing lots of debug messages or doing extra delay in the loop
	 * below may cause rx buffer to overflow and getting unexpected data
	 * error.
	 */
	while (rx_bytes || tx_bytes) {
		int temp;
		uint32_t spi_sts = readl(&regs->spi_sts);
		int rx_lvl = (spi_sts >> SPI_RX_LVL_OFFSET) & SPI_FIFO_LVL_MASK,
		    tx_lvl = (spi_sts >> SPI_TX_LVL_OFFSET) & SPI_FIFO_LVL_MASK;
		int min_tx = ((tx_bytes || !espi->half_duplex) ?
			      (espi->fifo_size / 2) : 1);

		// TODO(hungte) Abort if timeout happens in half-duplex mode.

		/*
		 * Don't completely fill the txfifo, since we don't want our
		 * rxfifo to overflow, and it may already contain data.
		 */
		while (tx_lvl < min_tx) {
			if (tx_bytes) {
				if (step == sizeof(uint32_t)) {
					temp = *((uint32_t *)txp);
					txp += sizeof(uint32_t);
				} else {
					temp = *txp++;
				}
				tx_bytes -= step;
			} else {
				temp = -1;
			}
			writel(temp, &regs->tx_data);
			tx_lvl += step;
		}

		while ((rx_lvl >= step) && rx_bytes) {
			temp = readl(&regs->rx_data);
			rx_lvl -= step;
			if (wait_for_frame_header) {
				if ((temp & 0xff) == espi->frame_header) {
					wait_for_frame_header = 0;
				}
				break;  /* Restart the outer loop. */
			}
			if (step == sizeof(uint32_t)) {
				*((uint32_t *)rxp) = temp;
				rxp += sizeof(uint32_t);
			} else {
				*rxp++ = temp;
			}
			rx_bytes -= step;
		}
	}
	return 0;
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct exynos_spi_slave *espi = to_exynos_spi(slave);
	struct exynos_spi *regs = espi->regs;

	exynos_spi_flush_fifo(regs);

	// Select Active High Clock, Format A (SCP 30.2.1.8).
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L | SPI_CH_CPHA_B);

	// Set FeedBack Clock Selection.
	writel(SPI_FB_DELAY_180, &regs->fb_clk);

	// HIGH speed is required for Tx/Rx to work in 50MHz (SCP 30.2.1.6).
	if (espi->half_duplex) {
		clrbits_le32(&regs->ch_cfg, SPI_CH_HS_EN);
		printk(BIOS_DEBUG, "%s: LOW speed.\n", __func__);
	} else {
		setbits_le32(&regs->ch_cfg, SPI_CH_HS_EN);
		printk(BIOS_DEBUG, "%s: HIGH speed.\n", __func__);
	}
	return 0;
}

int spi_xfer(struct spi_slave *slave, const void *dout, unsigned int bitsout,
	     void *din, unsigned int bitsin)
{
	unsigned int out_bytes = bitsout / 8, in_bytes = bitsin / 8;
	uint8_t *out_ptr = (uint8_t *)dout, *in_ptr = (uint8_t *)din;
	int offset, todo, len;
	int ret = 0;

	ASSERT(bitsout % 8 == 0 && bitsin % 8 == 0);
	len = MAX(out_bytes, in_bytes);

	/*
	 * Exynos SPI limits each transfer to (2^16-1=65535) bytes. To keep
	 * things simple (especially for word-width transfer mode), allow a
	 * maximum of (2^16-4=65532) bytes. We could allow more in word mode,
	 * but the performance difference is small.
	 */
	spi_cs_activate(slave);
	for (offset = 0; !ret && (offset < len); offset += todo) {
		todo = min(len - offset, (1 << 16) - 4);
		ret = spi_rx_tx(slave, in_ptr, MIN(in_bytes, todo), out_ptr,
				MIN(out_bytes, todo));
		// Adjust remaining bytes and pointers.
		if (in_bytes >= todo) {
			in_bytes -= todo;
			in_ptr += todo;
		} else {
			in_bytes = 0;
			in_ptr = NULL;
		}
		if (out_bytes >= todo) {
			out_bytes -= todo;
			out_ptr += todo;
		} else {
			out_bytes = 0;
			out_ptr = NULL;
		}
	}
	spi_cs_deactivate(slave);

	return ret;
}

static int exynos_spi_read(struct spi_slave *slave, void *dest, uint32_t len,
			   uint32_t off)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	int rv;

	// TODO(hungte) Merge the "read address" command into spi_xfer calls
	// (full-duplex mode).

	spi_cs_activate(slave);

	// Specify read address (in word-width mode).
	ASSERT(off < (1 << 24));
	exynos_spi_request_bytes(regs, sizeof(off), sizeof(off));
	writel(htonl((SF_READ_DATA_CMD << 24) | off), &regs->tx_data);
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE)) {
		/* Wait for TX done */
	}

	// Now, safe to transfer.
	rv = spi_xfer(slave, NULL, 0, dest, len * 8);
	spi_cs_deactivate(slave);

	return (rv == 0) ? len : -1;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	/* Reset swap mode to make sure no one relying on default values (Ex,
	 * payload or kernel) will go wrong. */
	clrbits_le32(&regs->mode_cfg, (SPI_MODE_CH_WIDTH_WORD |
				       SPI_MODE_BUS_WIDTH_WORD));
	writel(0, &regs->swap_cfg);
	exynos_spi_flush_fifo(regs);
}

// SPI as CBFS media.
struct exynos_spi_media {
	struct spi_slave *slave;
	struct cbfs_simple_buffer buffer;
};

static int exynos_spi_cbfs_open(struct cbfs_media *media) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_open\n");
	return spi_claim_bus(spi->slave);
}

static int exynos_spi_cbfs_close(struct cbfs_media *media) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_close\n");
	spi_release_bus(spi->slave);
	return 0;
}

static size_t exynos_spi_cbfs_read(struct cbfs_media *media, void *dest,
				   size_t offset, size_t count) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	int bytes;
	DEBUG_SPI("exynos_spi_cbfs_read(%u)\n", count);
	bytes = exynos_spi_read(spi->slave, dest, count, offset);
	return bytes;
}

static void *exynos_spi_cbfs_map(struct cbfs_media *media, size_t offset,
				 size_t count) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_map\n");
	// exynos: spi_rx_tx may work in 4 byte-width-transmission mode and
	// requires buffer memory address to be aligned.
	if (count % 4)
		count += 4 - (count % 4);
	return cbfs_simple_buffer_map(&spi->buffer, media, offset, count);
}

static void *exynos_spi_cbfs_unmap(struct cbfs_media *media,
				   const void *address) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_unmap\n");
	return cbfs_simple_buffer_unmap(&spi->buffer, address);
}

int initialize_exynos_spi_cbfs_media(struct cbfs_media *media,
				     void *buffer_address,
				     size_t buffer_size) {
	// TODO Replace static variable to support multiple streams.
	static struct exynos_spi_media context;
	static struct exynos_spi_slave eslave = {
		.slave = { .bus = 1, .rw = SPI_READ_FLAG, },
		.regs = samsung_get_base_spi1(),
		.fifo_size = 64,
		.half_duplex = 0,
	};
	DEBUG_SPI("initialize_exynos_spi_cbfs_media\n");

	context.slave = &eslave.slave;
	context.buffer.allocated = context.buffer.last_allocate = 0;
	context.buffer.buffer = buffer_address;
	context.buffer.size = buffer_size;
	media->context = (void*)&context;
	media->open = exynos_spi_cbfs_open;
	media->close = exynos_spi_cbfs_close;
	media->read = exynos_spi_cbfs_read;
	media->map = exynos_spi_cbfs_map;
	media->unmap = exynos_spi_cbfs_unmap;

	return 0;
}

int init_default_cbfs_media(struct cbfs_media *media) {
	return initialize_exynos_spi_cbfs_media(
			media,
			(void*)CONFIG_CBFS_CACHE_ADDRESS,
			CONFIG_CBFS_CACHE_SIZE);
}
