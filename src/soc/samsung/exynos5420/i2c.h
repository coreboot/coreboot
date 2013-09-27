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

#ifndef CPU_SAMSUNG_EXYNOS5420_I2C_H
#define CPU_SAMSUNG_EXYNOS5420_I2C_H

#include "periph.h"

struct s3c24x0_i2c {
	u32	iiccon;
	u32	iicstat;
	u32	iicadd;
	u32	iicds;
	u32	iiclc;
} __attribute__ ((packed));

struct exynos5_hsi2c {
	u32	usi_ctl;
	u32	usi_fifo_ctl;
	u32	usi_trailing_ctl;
	u32	usi_clk_ctl;
	u32	usi_clk_slot;
	u32	spi_ctl;
	u32	uart_ctl;
	u32	res1;
	u32	usi_int_en;
	u32	usi_int_stat;
	u32	usi_modem_stat;
	u32	usi_error_stat;
	u32	usi_fifo_stat;
	u32	usi_txdata;
	u32	usi_rxdata;
	u32	res2;
	u32	usi_conf;
	u32	usi_auto_conf;
	u32	usi_timeout;
	u32	usi_manual_cmd;
	u32	usi_trans_status;
	u32	usi_timing_hs1;
	u32	usi_timing_hs2;
	u32	usi_timing_hs3;
	u32	usi_timing_fs1;
	u32	usi_timing_fs2;
	u32	usi_timing_fs3;
	u32	usi_timing_sla;
	u32	i2c_addr;
} __attribute__ ((packed));

struct s3c24x0_i2c_bus {
	int bus_num;
	struct s3c24x0_i2c *regs;
	enum periph_id periph_id;
	struct exynos5_hsi2c *hsregs;
	int is_highspeed;	/* High speed type, rather than I2C */
	int id;
	unsigned clk_cycle;
	unsigned clk_div;
};

void i2c_init(unsigned bus, int speed, int slaveadd);

#endif /* CPU_SAMSUNG_EXYNOS5420_I2C_H */
