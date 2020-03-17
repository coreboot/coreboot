/*
 * This file is part of the coreboot project.
 *
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

#ifndef SOC_MEDIATEK_MT8173_I2C_H
#define SOC_MEDIATEK_MT8173_I2C_H

#include <soc/i2c_common.h>

/* I2C Register */
struct mt_i2c_regs {
	uint32_t data_port;
	uint32_t slave_addr;
	uint32_t intr_mask;
	uint32_t intr_stat;
	uint32_t control;
	uint32_t transfer_len;
	uint32_t transac_len;
	uint32_t delay_len;
	uint32_t timing;
	uint32_t start;
	uint32_t ext_conf;
	uint32_t reserved1;
	uint32_t fifo_stat;
	uint32_t fifo_thresh;
	uint32_t fifo_addr_clr;
	uint32_t reserved2;
	uint32_t io_config;
	uint32_t debug;
	uint32_t hs;
	uint32_t reserved3;
	uint32_t softreset;
	uint32_t dcm;
	uint32_t reserved4[3];
	uint32_t debug_stat;
	uint32_t debug_ctrl;
	uint32_t transfer_aux_len;
};

check_member(mt_i2c_regs, debug_stat, 0x64);

void mtk_i2c_bus_init(uint8_t bus);

#endif /* SOC_MEDIATEK_MT8173_I2C_H */
