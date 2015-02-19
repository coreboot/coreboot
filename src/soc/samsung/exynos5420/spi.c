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

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <soc/cpu.h>
#include <soc/spi.h>
#include <spi_flash.h>
#include <stdlib.h>
#include <string.h>

#define EXYNOS_SPI_MAX_TRANSFER_BYTES (65535)

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "EXYNOS_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

struct exynos_spi_slave {
	struct spi_slave slave;
	struct exynos_spi *regs;
	int initialized;
};

/* TODO(hungte) Move the SPI param list to per-board configuration, probably
 * Kconfig or mainboard.c */
static struct exynos_spi_slave exynos_spi_slaves[3] = {
	// SPI 0
	{
		.slave = { .bus = 0, },
		.regs = (void *)EXYNOS5_SPI0_BASE,
	},
	// SPI 1
	{
		.slave = { .bus = 1, .rw = SPI_READ_FLAG, },
		.regs = (void *)EXYNOS5_SPI1_BASE,
	},
	// SPI 2
	{
		.slave = { .bus = 2,
			   .rw = SPI_READ_FLAG | SPI_WRITE_FLAG, },
		.regs = (void *)EXYNOS5_SPI2_BASE,
	},
};

static inline struct exynos_spi_slave *to_exynos_spi(struct spi_slave *slave)
{
	return container_of(slave, struct exynos_spi_slave, slave);
}

static void spi_sw_reset(struct exynos_spi *regs, int word)
{
	const uint32_t orig_mode_cfg = read32(&regs->mode_cfg);
	uint32_t mode_cfg = orig_mode_cfg;
	const uint32_t orig_swap_cfg = read32(&regs->swap_cfg);
	uint32_t swap_cfg = orig_swap_cfg;

	mode_cfg &= ~(SPI_MODE_CH_WIDTH_MASK | SPI_MODE_BUS_WIDTH_MASK);
	if (word) {
		mode_cfg |= SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD;
		swap_cfg |= SPI_RX_SWAP_EN |
			    SPI_RX_BYTE_SWAP |
			    SPI_RX_HWORD_SWAP |
			    SPI_TX_SWAP_EN |
			    SPI_TX_BYTE_SWAP |
			    SPI_TX_HWORD_SWAP;
	} else {
		mode_cfg |= SPI_MODE_CH_WIDTH_BYTE | SPI_MODE_BUS_WIDTH_BYTE;
		swap_cfg = 0;
	}

	if (mode_cfg != orig_mode_cfg)
		write32(&regs->mode_cfg, mode_cfg);
	if (swap_cfg != orig_swap_cfg)
		write32(&regs->swap_cfg, swap_cfg);

	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);
}

void spi_init(void)
{
}

static void exynos_spi_init(struct exynos_spi *regs)
{
	// Set FB_CLK_SEL.
	write32(&regs->fb_clk, SPI_FB_DELAY_180);
	// CPOL: Active high.
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L);

	// Clear rx and tx channel if set priveously.
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits_le32(&regs->swap_cfg,
		     SPI_RX_SWAP_EN | SPI_RX_BYTE_SWAP | SPI_RX_HWORD_SWAP);
	clrbits_le32(&regs->ch_cfg, SPI_CH_HS_EN);

	// Do a soft reset, which will also enable both channels.
	spi_sw_reset(regs, 1);
}

struct spi_slave *spi_setup_slave(unsigned int bus, unsigned int cs)
{
	ASSERT(bus >= 0 && bus < 3);
	struct exynos_spi_slave *eslave = &exynos_spi_slaves[bus];
	if (!eslave->initialized) {
		exynos_spi_init(eslave->regs);
		eslave->initialized = 1;
	}
	return &eslave->slave;
}

int spi_claim_bus(struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	// TODO(hungte) Add some delay if too many transactions happen at once.
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
	return 0;
}

static void spi_transfer(struct exynos_spi *regs, void *in, const void *out,
			 u32 size)
{
	u8 *inb = in;
	const u8 *outb = out;

	int width = (size % 4) ? 1 : 4;

	while (size) {
		int packets = size / width;
		// The packet count field is 16 bits wide.
		packets = MIN(packets, (1 << 16) - 1);

		int out_bytes, in_bytes;
		out_bytes = in_bytes = packets * width;

		spi_sw_reset(regs, width == 4);
		write32(&regs->pkt_cnt, packets | SPI_PACKET_CNT_EN);

		while (out_bytes || in_bytes) {
			uint32_t spi_sts = read32(&regs->spi_sts);
			int rx_lvl = ((spi_sts >> 15) & 0x1ff);
			int tx_lvl = ((spi_sts >> 6) & 0x1ff);

			if (tx_lvl < 32 && tx_lvl < out_bytes) {
				uint32_t data = 0xffffffff;

				if (outb) {
					memcpy(&data, outb, width);
					outb += width;
				}
				write32(&regs->tx_data, data);

				out_bytes -= width;
			}

			if (rx_lvl >= width) {
				uint32_t data = read32(&regs->rx_data);

				if (inb) {
					memcpy(inb, &data, width);
					inb += width;
				}

				in_bytes -= width;
			}
		}

		size -= packets * width;
	}
}

int spi_xfer(struct spi_slave *slave, const void *dout, unsigned int bytes_out,
	     void *din, unsigned int bytes_in)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;

	if (bytes_out && bytes_in) {
		u32 min_size = MIN(bytes_out, bytes_in);

		spi_transfer(regs, din, dout, min_size);

		bytes_out -= min_size;
		bytes_in -= min_size;

		din = (uint8_t *)din + min_size;
		dout = (const uint8_t *)dout + min_size;
	}

	if (bytes_in)
		spi_transfer(regs, din, NULL, bytes_in);
	else if (bytes_out)
		spi_transfer(regs, NULL, dout, bytes_out);

	return 0;
}

void spi_release_bus(struct spi_slave *slave)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);
}

static int exynos_spi_read(struct spi_slave *slave, void *dest, uint32_t len,
			   uint32_t off)
{
	struct exynos_spi *regs = to_exynos_spi(slave)->regs;
	u32 command;
	spi_claim_bus(slave);

	// Send address.
	ASSERT(off < (1 << 24));
	command = htonl(SF_READ_DATA_CMD << 24 | off);
	spi_transfer(regs, NULL, &command, sizeof(command));

	// Read the data.
	spi_transfer(regs, dest, NULL, len);
	spi_release_bus(slave);

	return len;
}

// SPI as CBFS media.
struct exynos_spi_media {
	struct spi_slave *slave;
	struct cbfs_simple_buffer buffer;
};

static int exynos_spi_cbfs_open(struct cbfs_media *media)
{
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_open\n");
	return spi_claim_bus(spi->slave);
}

static int exynos_spi_cbfs_close(struct cbfs_media *media)
{
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_close\n");
	spi_release_bus(spi->slave);
	return 0;
}

static size_t exynos_spi_cbfs_read(struct cbfs_media *media, void *dest,
				   size_t offset, size_t count)
{
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	int bytes;
	DEBUG_SPI("exynos_spi_cbfs_read(%u)\n", count);
	bytes = exynos_spi_read(spi->slave, dest, count, offset);
	return bytes;
}

static void *exynos_spi_cbfs_map(struct cbfs_media *media, size_t offset,
				 size_t count)
{
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_map\n");
	// exynos: spi_rx_tx may work in 4 byte-width-transmission mode and
	// requires buffer memory address to be aligned.
	if (count % 4)
		count += 4 - (count % 4);
	return cbfs_simple_buffer_map(&spi->buffer, media, offset, count);
}

static void *exynos_spi_cbfs_unmap(struct cbfs_media *media,
				   const void *address)
{
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_unmap\n");
	return cbfs_simple_buffer_unmap(&spi->buffer, address);
}

int initialize_exynos_spi_cbfs_media(struct cbfs_media *media,
				     void *buffer_address,
				     size_t buffer_size)
{
	// TODO Replace static variable to support multiple streams.
	static struct exynos_spi_media context;
	static struct exynos_spi_slave *eslave = &exynos_spi_slaves[1];
	DEBUG_SPI("initialize_exynos_spi_cbfs_media\n");

	context.slave = &eslave->slave;
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
