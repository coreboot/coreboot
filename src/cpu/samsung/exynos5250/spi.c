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
#include "gpio.h"
#include "clk.h"
#include "spi.h"

#define OM_STAT		(0x1f << 1)
#define EXYNOS_BASE_SPI1 ((void *)0x12d30000)

#if defined(CONFIG_DEBUG_SPI) && CONFIG_DEBUG_SPI
# define DEBUG_SPI(x,...)	printk(BIOS_DEBUG, "EXYNOS_SPI: " x)
#else
# define DEBUG_SPI(x,...)
#endif

static void exynos_spi_rx_tx(struct exynos_spi *regs, int todo,
			     void *dinp, void const *doutp, int i)
{
	int rx_lvl, tx_lvl;
	unsigned int *rxp = (unsigned int *)(dinp + (i * (32 * 1024)));
	unsigned int out_bytes, in_bytes;

	// TODO In current implementation, every read/write must be aligned to
	// 4 bytes, otherwise you may get timeout or other unexpected results.
	ASSERT(todo % 4 == 0);

	out_bytes = in_bytes = todo;
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	writel(((todo * 8) / 32) | SPI_PACKET_CNT_EN, &regs->pkt_cnt);

	while (in_bytes) {
		uint32_t spi_sts;
		int temp;

		spi_sts = readl(&regs->spi_sts);
		rx_lvl = ((spi_sts >> 15) & 0x7f);
		tx_lvl = ((spi_sts >> 6) & 0x7f);
		while (tx_lvl < 32 && out_bytes) {
			// TODO The "writing" (tx) is not supported now; that's
			// why we write garbage to keep driving FIFO clock.
			temp = 0xffffffff;
			writel(temp, &regs->tx_data);
			out_bytes -= 4;
			tx_lvl += 4;
		}
		while (rx_lvl >= 4 && in_bytes) {
			temp = readl(&regs->rx_data);
			if (rxp)
				*rxp++ = temp;
			in_bytes -= 4;
			rx_lvl -= 4;
		}
	}
}

/* set up SPI channel */
int exynos_spi_open(struct exynos_spi *regs)
{
	/* set the spi1 GPIO */

	/* set pktcnt and enable it */
	writel(4 | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
	/* set FB_CLK_SEL */
	writel(SPI_FB_DELAY_180, &regs->fb_clk);
	/* set CH_WIDTH and BUS_WIDTH as word */
	setbits_le32(&regs->mode_cfg,
		     SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD);
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L); /* CPOL: active high */

	/* clear rx and tx channel if set previously */
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits_le32(&regs->swap_cfg,
		     SPI_RX_SWAP_EN | SPI_RX_BYTE_SWAP | SPI_RX_HWORD_SWAP);

	/* do a soft reset */
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);

	/* now set rx and tx channel ON */
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON | SPI_CH_HS_EN);
	return 0;
}

int exynos_spi_read(struct exynos_spi *regs, void *dest, u32 len, u32 off)
{
	int upto, todo;
	int i;
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT); /* CS low */

	/* Send read instruction (0x3h) followed by a 24 bit addr */
	writel((SF_READ_DATA_CMD << 24) | off, &regs->tx_data);

	/* waiting for TX done */
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < len; upto += todo, i++) {
		todo = MIN(len - upto, (1 << 15));
		exynos_spi_rx_tx(regs, todo, dest, (void *)(off), i);
	}

	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	return len;
}

int exynos_spi_close(struct exynos_spi *regs)
{
	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits_le32(&regs->mode_cfg,
		     SPI_MODE_CH_WIDTH_WORD | SPI_MODE_BUS_WIDTH_WORD);
	writel(0, &regs->swap_cfg);

	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrsetbits_le32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
	return 0;
}

// SPI as CBFS media.
struct exynos_spi_media {
	struct exynos_spi *regs;
	struct cbfs_simple_buffer buffer;
};

static int exynos_spi_cbfs_open(struct cbfs_media *media) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_open\n");
	return exynos_spi_open(spi->regs);
}

static int exynos_spi_cbfs_close(struct cbfs_media *media) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_close\n");
	return exynos_spi_close(spi->regs);
}

static size_t exynos_spi_cbfs_read(struct cbfs_media *media, void *dest,
				   size_t offset, size_t count) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	int bytes;
	DEBUG_SPI("exynos_spi_cbfs_read(%u)\n", count);
	bytes = exynos_spi_read(spi->regs, dest, count, offset);
	// Flush and re-open the device.
	exynos_spi_close(spi->regs);
	exynos_spi_open(spi->regs);
	return bytes;
}

static void *exynos_spi_cbfs_map(struct cbfs_media *media, size_t offset,
				 size_t count) {
	struct exynos_spi_media *spi = (struct exynos_spi_media*)media->context;
	DEBUG_SPI("exynos_spi_cbfs_map\n");
	// See exynos_spi_rx_tx for I/O alignment limitation.
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
	DEBUG_SPI("initialize_exynos_spi_cbfs_media\n");

	context.regs = EXYNOS_BASE_SPI1;
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
