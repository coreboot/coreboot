/*
 * Copyright (C) 2011 Samsung Electronics
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

/* FIXME(dhendrix): pulled in a lot of extra crap such as partition and string
   libs*/
#include <assert.h>
#include <common.h>
#include <stdlib.h>
#include <string.h>

#include <console/console.h>
#include <console/loglevel.h>

#include <config.h>
#include <spi.h>

#include <cpu/samsung/exynos5250/gpio.h>
#include <cpu/samsung/exynos5250/clk.h>
#include <cpu/samsung/exynos5250/cpu.h>
#include <cpu/samsung/exynos5250/periph.h>
#include <cpu/samsung/exynos5250/pinmux.h>
#include <cpu/samsung/exynos5250/power.h>
#include <cpu/samsung/exynos5250/spi.h>

#include <cpu/samsung/exynos5-common/cpu.h>
#include <cpu/samsung/exynos5-common/exynos5-common.h>

#include <system.h>

#include <arch/io.h>
#include <lib.h>

#define OM_STAT		(0x1f << 1)

static void spi_rx_tx(struct exynos_spi *regs, int todo,
			void *dinp, void const *doutp, int i)
{
	uint *rxp = (uint *)(dinp + (i * (32 * 1024)));
	int rx_lvl, tx_lvl;
	uint out_bytes, in_bytes;

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

struct stream *stream_start(struct stream *stream, u64 base, u32 size)
{
//	struct exynos_spi *regs = (struct exynos_spi *)samsung_get_base_spi1();
	stream->base = base;
	stream->size = size;
	struct exynos_spi *regs = (void *)(u32)base;

	clock_set_rate(PERIPH_ID_SPI1, 50000000); /* set spi clock to 50Mhz */
	/* set the spi1 GPIO */
//	exynos_pinmux_config(PERIPH_ID_SPI1, PINMUX_FLAG_NONE);
	gpio_cfg_pin(GPIO_A24, 0x2);
	gpio_cfg_pin(GPIO_A25, 0x2);
	gpio_cfg_pin(GPIO_A26, 0x2);
	gpio_cfg_pin(GPIO_A27, 0x2);

	/* set pktcnt and enable it */
	writel(4 | SPI_PACKET_CNT_EN, &regs->pkt_cnt);
	/* set FB_CLK_SEL */
	writel(SPI_FB_DELAY_180, &regs->fb_clk);
	/* set CH_WIDTH and BUS_WIDTH as word */
	setbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	clrbits_le32(&regs->ch_cfg, SPI_CH_CPOL_L); /* CPOL: active high */

	/* clear rx and tx channel if set priveously */
	clrbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON);

	setbits_le32(&regs->swap_cfg, SPI_RX_SWAP_EN |
		SPI_RX_BYTE_SWAP |
		SPI_RX_HWORD_SWAP);

	/* do a soft reset */
	setbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);

	/* now set rx and tx channel ON */
	setbits_le32(&regs->ch_cfg, SPI_RX_CH_ON | SPI_TX_CH_ON | SPI_CH_HS_EN);
	clrbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT); /* CS low */
	return (void *)regs;
}

int stream_read(struct stream *stream, void *where, u32 len, u32 off)
{
	int upto, todo;
	int i;
	struct exynos_spi *regs = (void *)(u32)stream->base;
	if (off >= stream->size)
		return 0;
	/* Send read instruction (0x3h) followed by a 24 bit addr */
	writel((SF_READ_DATA_CMD << 24) | off, &regs->tx_data);

	/* waiting for TX done */
	while (!(readl(&regs->spi_sts) & SPI_ST_TX_DONE));

	for (upto = 0, i = 0; upto < len; upto += todo, i++) {
		todo = MIN(len - upto, (1 << 15));
		spi_rx_tx(regs, todo, (void *)(where),
					(void *)(off), i);
	}

	setbits_le32(&regs->cs_reg, SPI_SLAVE_SIG_INACT);/* make the CS high */

	/*
	 * Let put controller mode to BYTE as
	 * SPI driver does not support WORD mode yet
	 */
	clrbits_le32(&regs->mode_cfg, SPI_MODE_CH_WIDTH_WORD |
					SPI_MODE_BUS_WIDTH_WORD);
	writel(0, &regs->swap_cfg);

	return len;
}

void stream_fini(struct stream *stream)
{
	struct exynos_spi *regs = (void *)(u32)stream->base;

	/*
	 * Flush spi tx, rx fifos and reset the SPI controller
	 * and clear rx/tx channel
	 */
	clrsetbits_le32(&regs->ch_cfg, SPI_CH_HS_EN, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_CH_RST);
	clrbits_le32(&regs->ch_cfg, SPI_TX_CH_ON | SPI_RX_CH_ON);
}

