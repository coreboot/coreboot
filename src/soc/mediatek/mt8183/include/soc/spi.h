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

#ifndef MTK_MT8183_SPI_H
#define MTK_MT8183_SPI_H

#include <soc/spi_common.h>

#define SPI_BUS_NUMBER 6

/* SPI peripheral register map. */
typedef struct mtk_spi_regs {
	uint32_t spi_cfg0_reg;
	uint32_t spi_cfg1_reg;
	uint32_t spi_tx_src_reg;
	uint32_t spi_rx_dst_reg;
	uint32_t spi_tx_data_reg;
	uint32_t spi_rx_data_reg;
	uint32_t spi_cmd_reg;
	uint32_t spi_status0_reg;
	uint32_t spi_status1_reg;
	uint32_t spi_pad_macro_sel_reg;
	uint32_t spi_cfg2_reg;
	uint32_t spi_tx_src_64_reg;
	uint32_t spi_rx_dst_64_reg;
} mtk_spi_regs;

check_member(mtk_spi_regs, spi_pad_macro_sel_reg, 0x24);

enum {
	SPI_CFG0_CS_HOLD_SHIFT = 0,
	SPI_CFG0_CS_SETUP_SHIFT = 16,
};

enum {
	SPI_CFG2_SCK_LOW_SHIFT = 0,
	SPI_CFG2_SCK_HIGH_SHIFT = 16,
};


#endif
