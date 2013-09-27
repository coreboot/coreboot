/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Samsung Electronics
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

#ifndef CPU_SAMSUNG_EXYNOS5250_SPI_H
#define CPU_SAMSUNG_EXYNOS5250_SPI_H

/* This driver serves as a CBFS media source. */
#include <cbfs.h>

/* SPI peripheral register map; padded to 64KB */
struct exynos_spi {
	unsigned int		ch_cfg;		/* 0x00 */
	unsigned char		reserved0[4];
	unsigned int		mode_cfg;	/* 0x08 */
	unsigned int		cs_reg;		/* 0x0c */
	unsigned char		reserved1[4];
	unsigned int		spi_sts;	/* 0x14 */
	unsigned int		tx_data;	/* 0x18 */
	unsigned int		rx_data;	/* 0x1c */
	unsigned int		pkt_cnt;	/* 0x20 */
	unsigned char		reserved2[4];
	unsigned int		swap_cfg;	/* 0x28 */
	unsigned int		fb_clk;		/* 0x2c */
	unsigned char		padding[0xffd0];
};

#define EXYNOS_SPI_MAX_FREQ	50000000

#define SPI_TIMEOUT_MS		10

#define SF_READ_DATA_CMD	0x3

/* SPI_CHCFG */
#define SPI_CH_HS_EN		(1 << 6)
#define SPI_CH_RST		(1 << 5)
#define SPI_SLAVE_MODE		(1 << 4)
#define SPI_CH_CPOL_L		(1 << 3)
#define SPI_CH_CPHA_B		(1 << 2)
#define SPI_RX_CH_ON		(1 << 1)
#define SPI_TX_CH_ON		(1 << 0)

/* SPI_MODECFG */
#define SPI_MODE_CH_WIDTH_WORD	(0x2 << 29)
#define SPI_MODE_BUS_WIDTH_WORD	(0x2 << 17)

/* SPI_CSREG */
#define SPI_SLAVE_SIG_INACT	(1 << 0)

/* SPI_STS */
#define SPI_ST_TX_DONE		(1 << 25)
#define SPI_FIFO_LVL_MASK	0x1ff
#define SPI_TX_LVL_OFFSET	6
#define SPI_RX_LVL_OFFSET	15

/* Feedback Delay */
#define SPI_CLK_BYPASS		(0 << 0)
#define SPI_FB_DELAY_90		(1 << 0)
#define SPI_FB_DELAY_180	(2 << 0)
#define SPI_FB_DELAY_270	(3 << 0)

/* Packet Count */
#define SPI_PACKET_CNT_EN	(1 << 16)

/* Swap config */
#define SPI_TX_SWAP_EN		(1 << 0)
#define SPI_TX_BYTE_SWAP	(1 << 2)
#define SPI_TX_HWORD_SWAP	(1 << 3)
#define SPI_TX_BYTE_SWAP	(1 << 2)
#define SPI_RX_SWAP_EN		(1 << 4)
#define SPI_RX_BYTE_SWAP	(1 << 6)
#define SPI_RX_HWORD_SWAP	(1 << 7)

/* API */
int exynos_spi_open(struct exynos_spi *regs);
int exynos_spi_read(struct exynos_spi *regs, void *dest, u32 len, u32 off);
int exynos_spi_close(struct exynos_spi *regs);

/* Serve as CBFS media source */
int initialize_exynos_spi_cbfs_media(struct cbfs_media *media,
				     void *buffer_address,
				     size_t buffer_size);
#endif
