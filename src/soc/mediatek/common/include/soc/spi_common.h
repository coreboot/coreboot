/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 MediaTek Inc.
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

#ifndef MTK_COMMON_SPI_H
#define MTK_COMMON_SPI_H

#include <spi-generic.h>

enum {
	SPI_CFG1_CS_IDLE_SHIFT = 0,
	SPI_CFG1_PACKET_LOOP_SHIFT = 8,
	SPI_CFG1_PACKET_LENGTH_SHIFT = 16,

	SPI_CFG1_CS_IDLE_MASK = 0xff << SPI_CFG1_CS_IDLE_SHIFT,
	SPI_CFG1_PACKET_LOOP_MASK = 0xff << SPI_CFG1_PACKET_LOOP_SHIFT,
	SPI_CFG1_PACKET_LENGTH_MASK = 0x3ff << SPI_CFG1_PACKET_LENGTH_SHIFT,
};

enum {
	SPI_CMD_ACT_SHIFT = 0,
	SPI_CMD_RESUME_SHIFT = 1,
	SPI_CMD_RST_SHIFT = 2,
	SPI_CMD_PAUSE_EN_SHIFT = 4,
	SPI_CMD_DEASSERT_SHIFT = 5,
	SPI_CMD_CPHA_SHIFT = 8,
	SPI_CMD_CPOL_SHIFT = 9,
	SPI_CMD_RX_DMA_SHIFT = 10,
	SPI_CMD_TX_DMA_SHIFT = 11,
	SPI_CMD_TXMSBF_SHIFT = 12,
	SPI_CMD_RXMSBF_SHIFT = 13,
	SPI_CMD_RX_ENDIAN_SHIFT = 14,
	SPI_CMD_TX_ENDIAN_SHIFT = 15,
	SPI_CMD_FINISH_IE_SHIFT = 16,
	SPI_CMD_PAUSE_IE_SHIFT = 17,

	SPI_CMD_ACT_EN = BIT(SPI_CMD_ACT_SHIFT),
	SPI_CMD_RESUME_EN = BIT(SPI_CMD_RESUME_SHIFT),
	SPI_CMD_RST_EN = BIT(SPI_CMD_RST_SHIFT),
	SPI_CMD_PAUSE_EN = BIT(SPI_CMD_PAUSE_EN_SHIFT),
	SPI_CMD_DEASSERT_EN = BIT(SPI_CMD_DEASSERT_SHIFT),
	SPI_CMD_CPHA_EN = BIT(SPI_CMD_CPHA_SHIFT),
	SPI_CMD_CPOL_EN = BIT(SPI_CMD_CPOL_SHIFT),
	SPI_CMD_RX_DMA_EN = BIT(SPI_CMD_RX_DMA_SHIFT),
	SPI_CMD_TX_DMA_EN = BIT(SPI_CMD_TX_DMA_SHIFT),
	SPI_CMD_TXMSBF_EN = BIT(SPI_CMD_TXMSBF_SHIFT),
	SPI_CMD_RXMSBF_EN = BIT(SPI_CMD_RXMSBF_SHIFT),
	SPI_CMD_RX_ENDIAN_EN = BIT(SPI_CMD_RX_ENDIAN_SHIFT),
	SPI_CMD_TX_ENDIAN_EN = BIT(SPI_CMD_TX_ENDIAN_SHIFT),
	SPI_CMD_FINISH_IE_EN = BIT(SPI_CMD_FINISH_IE_SHIFT),
	SPI_CMD_PAUSE_IE_EN = BIT(SPI_CMD_PAUSE_IE_SHIFT),
};

enum spi_pad_mask {
	SPI_PAD0_MASK = 0x0,
	SPI_PAD1_MASK = 0x1,
	SPI_PAD2_MASK = 0x2,
	SPI_PAD3_MASK = 0x3,
	SPI_PAD_SEL_MASK = 0x3
};

struct mtk_spi_regs;

struct mtk_spi_bus {
	struct spi_slave slave;
	struct mtk_spi_regs *regs;
	int initialized;
	int state;
};

extern const struct spi_ctrlr spi_ctrlr;
extern struct mtk_spi_bus spi_bus[];

void mtk_spi_set_gpio_pinmux(unsigned int bus,
			     enum spi_pad_mask pad_select);
void mtk_spi_set_timing(struct mtk_spi_regs *regs, u32 sck_ticks, u32 cs_ticks);
void mtk_spi_init(unsigned int bus, enum spi_pad_mask pad_select,
		  unsigned int speed_hz);

#endif
