/*
 * Copyright 2014 Google Inc.
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef __NVIDIA_TEGRA132_SPI_H__
#define __NVIDIA_TEGRA132_SPI_H__

#include <soc/dma.h>
#include <spi-generic.h>
#include <stddef.h>

struct tegra_spi_regs {
	u32 command1;		/* 0x000: SPI_COMMAND1 */
	u32 command2;		/* 0x004: SPI_COMMAND2 */
	u32 timing1;		/* 0x008: SPI_CS_TIM1 */
	u32 timing2;		/* 0x00c: SPI_CS_TIM2 */
	u32 trans_status;	/* 0x010: SPI_TRANS_STATUS */
	u32 fifo_status;	/* 0x014: SPI_FIFO_STATUS */
	u32 tx_data;		/* 0x018: SPI_TX_DATA */
	u32 rx_data;		/* 0x01c: SPI_RX_DATA */
	u32 dma_ctl;		/* 0x020: SPI_DMA_CTL */
	u32 dma_blk;		/* 0x024: SPI_DMA_BLK */
	u32 rsvd[56];		/* 0x028-0x107: reserved */
	u32 tx_fifo;		/* 0x108: SPI_FIFO1 */
	u32 rsvd2[31];		/* 0x10c-0x187 reserved */
	u32 rx_fifo;		/* 0x188: SPI_FIFO2 */
	u32 spare_ctl;		/* 0x18c: SPI_SPARE_CTRL */
} __attribute__((packed));
check_member(tegra_spi_regs, spare_ctl, 0x18c);

enum spi_xfer_mode {
	XFER_MODE_NONE = 0,
	XFER_MODE_PIO,
	XFER_MODE_DMA,
};

struct tegra_spi_channel {
	struct tegra_spi_regs *regs;

	/* static configuration */
	struct spi_slave slave;
	unsigned int req_sel;

	int dual_mode;		/* for x2 transfers with bit interleaving */

	/* context (used internally) */
	u8 *in_buf, *out_buf;
	struct apb_dma_channel *dma_out, *dma_in;
	enum spi_xfer_mode xfer_mode;
};

struct tegra_spi_channel *tegra_spi_init(unsigned int bus);

#endif	/* __NVIDIA_TEGRA132_SPI_H__ */
