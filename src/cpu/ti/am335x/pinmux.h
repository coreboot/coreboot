/*
 * Copyright (C) 2013 Texas Instruments Incorporated - http://www.ti.com/
 * Copyright (C) 2013 Google Inc.
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

#ifndef __CPU_TI_AM335X_PINMUX_H
#define __CPU_TI_AM335X_PINMUX_H

#include <stdint.h>

// PAD Control Fields
#define SLEWCTRL	(0x1 << 6)
#define RXACTIVE	(0x1 << 5)
#define PULLDOWN_EN	(0x0 << 4) // Pull down
#define PULLUP_EN	(0x1 << 4) // Pull up
#define PULLUDEN	(0x0 << 3) // Pull up enabled
#define PULLUDDIS	(0x1 << 3) // Pull up disabled
#define MODE(val)	val

void am335x_pinmux_uart0(void);
void am335x_pinmux_uart1(void);
void am335x_pinmux_uart2(void);
void am335x_pinmux_uart3(void);
void am335x_pinmux_uart4(void);
void am335x_pinmux_uart5(void);

void am335x_pinmux_mmc0(int cd, int sk_evm);
void am335x_pinmux_mmc1(void);

void am335x_pinmux_i2c0(void);
void am335x_pinmux_i2c1(void);

void am335x_pinmux_spi0(void);

void am335x_pinmux_gpio0_7(void);

void am335x_pinmux_rgmii1(void);
void am335x_pinmux_mii1(void);

void am335x_pinmux_nand(void);

#define AM335X_PINMUX_REG_ADDR 0x44e10800

struct am335x_pinmux_regs {
	uint32_t gpmc_ad0;
	uint32_t gpmc_ad1;
	uint32_t gpmc_ad2;
	uint32_t gpmc_ad3;
	uint32_t gpmc_ad4;
	uint32_t gpmc_ad5;
	uint32_t gpmc_ad6;
	uint32_t gpmc_ad7;
	uint32_t gpmc_ad8;
	uint32_t gpmc_ad9;
	uint32_t gpmc_ad10;
	uint32_t gpmc_ad11;
	uint32_t gpmc_ad12;
	uint32_t gpmc_ad13;
	uint32_t gpmc_ad14;
	uint32_t gpmc_ad15;
	uint32_t gpmc_a0;
	uint32_t gpmc_a1;
	uint32_t gpmc_a2;
	uint32_t gpmc_a3;
	uint32_t gpmc_a4;
	uint32_t gpmc_a5;
	uint32_t gpmc_a6;
	uint32_t gpmc_a7;
	uint32_t gpmc_a8;
	uint32_t gpmc_a9;
	uint32_t gpmc_a10;
	uint32_t gpmc_a11;
	uint32_t gpmc_wait0;
	uint32_t gpmc_wpn;
	uint32_t gpmc_be1n;
	uint32_t gpmc_csn0;
	uint32_t gpmc_csn1;
	uint32_t gpmc_csn2;
	uint32_t gpmc_csn3;
	uint32_t gpmc_clk;
	uint32_t gpmc_advn_ale;
	uint32_t gpmc_oen_ren;
	uint32_t gpmc_wen;
	uint32_t gpmc_be0n_cle;
	uint32_t lcd_data0;
	uint32_t lcd_data1;
	uint32_t lcd_data2;
	uint32_t lcd_data3;
	uint32_t lcd_data4;
	uint32_t lcd_data5;
	uint32_t lcd_data6;
	uint32_t lcd_data7;
	uint32_t lcd_data8;
	uint32_t lcd_data9;
	uint32_t lcd_data10;
	uint32_t lcd_data11;
	uint32_t lcd_data12;
	uint32_t lcd_data13;
	uint32_t lcd_data14;
	uint32_t lcd_data15;
	uint32_t lcd_vsync;
	uint32_t lcd_hsync;
	uint32_t lcd_pclk;
	uint32_t lcd_ac_bias_en;
	uint32_t mmc0_dat3;
	uint32_t mmc0_dat2;
	uint32_t mmc0_dat1;
	uint32_t mmc0_dat0;
	uint32_t mmc0_clk;
	uint32_t mmc0_cmd;
	uint32_t mii1_col;
	uint32_t mii1_crs;
	uint32_t mii1_rxerr;
	uint32_t mii1_txen;
	uint32_t mii1_rxdv;
	uint32_t mii1_txd3;
	uint32_t mii1_txd2;
	uint32_t mii1_txd1;
	uint32_t mii1_txd0;
	uint32_t mii1_txclk;
	uint32_t mii1_rxclk;
	uint32_t mii1_rxd3;
	uint32_t mii1_rxd2;
	uint32_t mii1_rxd1;
	uint32_t mii1_rxd0;
	uint32_t rmii1_refclk;
	uint32_t mdio_data;
	uint32_t mdio_clk;
	uint32_t spi0_sclk;
	uint32_t spi0_d0;
	uint32_t spi0_d1;
	uint32_t spi0_cs0;
	uint32_t spi0_cs1;
	uint32_t ecap0_in_pwm0_out;
	uint32_t uart0_ctsn;
	uint32_t uart0_rtsn;
	uint32_t uart0_rxd;
	uint32_t uart0_txd;
	uint32_t uart1_ctsn;
	uint32_t uart1_rtsn;
	uint32_t uart1_rxd;
	uint32_t uart1_txd;
	uint32_t i2c0_sda;
	uint32_t i2c0_scl;
	uint32_t mcasp0_aclkx;
	uint32_t mcasp0_fsx;
	uint32_t mcasp0_axr0;
	uint32_t mcasp0_ahclkr;
	uint32_t mcasp0_aclkr;
	uint32_t mcasp0_fsr;
	uint32_t mcasp0_axr1;
	uint32_t mcasp0_ahclkx;
	uint32_t xdma_event_intr0;
	uint32_t xdma_event_intr1;
	uint32_t nresetin_out;
	uint32_t porz;
	uint32_t nnmi;
	uint32_t osc0_in;
	uint32_t osc0_out;
	uint32_t rsvd1;
	uint32_t tms;
	uint32_t tdi;
	uint32_t tdo;
	uint32_t tck;
	uint32_t ntrst;
	uint32_t emu0;
	uint32_t emu1;
	uint32_t osc1_in;
	uint32_t osc1_out;
	uint32_t pmic_power_en;
	uint32_t rtc_porz;
	uint32_t rsvd2;
	uint32_t ext_wakeup;
	uint32_t enz_kaldo_1p8v;
	uint32_t usb0_dm;
	uint32_t usb0_dp;
	uint32_t usb0_ce;
	uint32_t usb0_id;
	uint32_t usb0_vbus;
	uint32_t usb0_drvvbus;
	uint32_t usb1_dm;
	uint32_t usb1_dp;
	uint32_t usb1_ce;
	uint32_t usb1_id;
	uint32_t usb1_vbus;
	uint32_t usb1_drvvbus;
	uint32_t ddr_resetn;
	uint32_t ddr_csn0;
	uint32_t ddr_cke;
	uint32_t ddr_ck;
	uint32_t ddr_nck;
	uint32_t ddr_casn;
	uint32_t ddr_rasn;
	uint32_t ddr_wen;
	uint32_t ddr_ba0;
	uint32_t ddr_ba1;
	uint32_t ddr_ba2;
	uint32_t ddr_a0;
	uint32_t ddr_a1;
	uint32_t ddr_a2;
	uint32_t ddr_a3;
	uint32_t ddr_a4;
	uint32_t ddr_a5;
	uint32_t ddr_a6;
	uint32_t ddr_a7;
	uint32_t ddr_a8;
	uint32_t ddr_a9;
	uint32_t ddr_a10;
	uint32_t ddr_a11;
	uint32_t ddr_a12;
	uint32_t ddr_a13;
	uint32_t ddr_a14;
	uint32_t ddr_a15;
	uint32_t ddr_odt;
	uint32_t ddr_d0;
	uint32_t ddr_d1;
	uint32_t ddr_d2;
	uint32_t ddr_d3;
	uint32_t ddr_d4;
	uint32_t ddr_d5;
	uint32_t ddr_d6;
	uint32_t ddr_d7;
	uint32_t ddr_d8;
	uint32_t ddr_d9;
	uint32_t ddr_d10;
	uint32_t ddr_d11;
	uint32_t ddr_d12;
	uint32_t ddr_d13;
	uint32_t ddr_d14;
	uint32_t ddr_d15;
	uint32_t ddr_dqm0;
	uint32_t ddr_dqm1;
	uint32_t ddr_dqs0;
	uint32_t ddr_dqsn0;
	uint32_t ddr_dqs1;
	uint32_t ddr_dqsn1;
	uint32_t ddr_vref;
	uint32_t ddr_vtp;
	uint32_t ddr_strben0;
	uint32_t ddr_strben1;
	uint32_t ain7;
	uint32_t ain6;
	uint32_t ain5;
	uint32_t ain4;
	uint32_t ain3;
	uint32_t ain2;
	uint32_t ain1;
	uint32_t ain0;
	uint32_t vrefp;
	uint32_t vrefn;
};

#endif
