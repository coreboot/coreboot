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

#ifndef __SOC_ROCKCHIP_RK3288_GRF_H__
#define __SOC_ROCKCHIP_RK3288_GRF_H__

#include <soc/addressmap.h>
#include <soc/soc.h>
#include <types.h>

struct rk3288_grf_gpio_lh {
	u32 l;
	u32 h;
};
check_member(rk3288_grf_gpio_lh, h, 0x4);

struct rk3288_grf_regs {
	u32 reserved[3];
	union {
		u32 gpio1d_iomux;
		u32 iomux_lcdc;
	};
	u32 gpio2a_iomux;
	u32 gpio2b_iomux;
	union {
		u32 gpio2c_iomux;
		u32 iomux_i2c3;
	};
	u32 reserved2;
	union {
		u32 gpio3a_iomux;
		u32 iomux_emmcdata;
	};
	union {
		u32 gpio3b_iomux;
		u32 iomux_emmcpwren;
	};
	union {
		u32 gpio3c_iomux;
		u32 iomux_emmccmd;
	};
	u32 gpio3dl_iomux;
	u32 gpio3dh_iomux;
	u32 gpio4al_iomux;
	u32 gpio4ah_iomux;
	u32 gpio4bl_iomux;
	u32 reserved3;
	u32 gpio4c_iomux;
	u32 gpio4d_iomux;
	u32 reserved4;
	union {
		u32 gpio5b_iomux;
		u32 iomux_spi0;
	};
	u32 gpio5c_iomux;
	u32 reserved5;
	union {
		u32 gpio6a_iomux;
		u32 iomux_i2s;
	};
	union {
		u32 gpio6b_iomux;
		u32 iomux_i2c2;
		u32 iomux_i2sclk;
	};
	union {
		u32 gpio6c_iomux;
		u32 iomux_sdmmc0;
	};
	u32 reserved6;
	union {
		u32 gpio7a_iomux;
		u32 iomux_pwm0;
		u32 iomux_pwm1;
	};
	union {
		u32 gpio7b_iomux;
		u32 iomux_edp_hotplug;
	};
	union {
		u32 gpio7cl_iomux;
		u32 iomux_i2c5sda;
		u32 iomux_i2c4;
	};
	union {
		u32 gpio7ch_iomux;
		u32 iomux_uart2;
		u32 iomux_i2c5scl;
	};
	u32 reserved7;
	union {
		u32 gpio8a_iomux;
		u32 iomux_spi2csclk;
		u32 iomux_i2c1;
	};
	union {
		u32 gpio8b_iomux;
		u32 iomux_spi2txrx;
	};
	u32 reserved8[30];
	struct rk3288_grf_gpio_lh gpio_sr[8];
	u32 gpio1_p[8][4];
	u32 gpio1_e[8][4];
	u32 gpio_smt;
	u32 soc_con0;
	u32 soc_con1;
	u32 soc_con2;
	u32 soc_con3;
	u32 soc_con4;
	u32 soc_con5;
	u32 soc_con6;
	u32 soc_con7;
	u32 soc_con8;
	u32 soc_con9;
	u32 soc_con10;
	u32 soc_con11;
	u32 soc_con12;
	u32 soc_con13;
	u32 soc_con14;
	u32 soc_status[22];
	u32 reserved9[2];
	u32 peridmac_con[4];
	u32 ddrc0_con0;
	u32 ddrc1_con0;
	u32 cpu_con[5];
	u32 reserved10[3];
	u32 cpu_status0;
	u32 reserved11;
	u32 uoc0_con[5];
	u32 uoc1_con[5];
	u32 uoc2_con[4];
	u32 uoc3_con[2];
	u32 uoc4_con[2];
	u32 pvtm_con[3];
	u32 pvtm_status[3];
	u32 io_vsel;
	u32 saradc_testbit;
	u32 tsadc_testbit_l;
	u32 tsadc_testbit_h;
	u32 os_reg[4];
	u32 reserved12;
	u32 soc_con15;
	u32 soc_con16;
};
check_member(rk3288_grf_regs, soc_con16, 0x3a8);

struct rk3288_sgrf_regs {
	u32 soc_con0;
	u32 soc_con1;
	u32 soc_con2;
	u32 soc_con3;
	u32 soc_con4;
	u32 soc_con5;
	u32 reserved1[(0x20-0x18)/4];
	u32 busdmac_con[2];
	u32 reserved2[(0x40-0x28)/4];
	u32 cpu_con[3];
	u32 reserved3[(0x50-0x4c)/4];
	u32 soc_con6;
	u32 soc_con7;
	u32 soc_con8;
	u32 soc_con9;
	u32 soc_con10;
	u32 soc_con11;
	u32 soc_con12;
	u32 soc_con13;
	u32 soc_con14;
	u32 soc_con15;
	u32 soc_con16;
	u32 soc_con17;
	u32 soc_con18;
	u32 soc_con19;
	u32 soc_con20;
	u32 soc_con21;
	u32 reserved4[(0x100-0x90)/4];
	u32 soc_status[2];
	u32 reserved5[(0x120-0x108)/4];
	u32 fast_boot_addr;
};
check_member(rk3288_sgrf_regs, fast_boot_addr, 0x0120);

static struct rk3288_grf_regs * const rk3288_grf = (void *)GRF_BASE;
static struct rk3288_sgrf_regs * const rk3288_sgrf = (void *)GRF_SECURE_BASE;

#define IOMUX_I2C1	RK_CLRSETBITS(3 << 10 | 3 << 8, 1 << 10 | 1 << 8)
#define IOMUX_I2C2	RK_SETBITS(1 << 4 | 1 << 2)
#define IOMUX_I2C3	RK_SETBITS(1 << 2 | 1 << 0)
#define IOMUX_I2C4	RK_SETBITS(1 << 8 | 1 << 4)
#define IOMUX_I2C5SDA	RK_CLRSETBITS(3 << 12, 1 << 12)
#define IOMUX_I2C5SCL	RK_CLRSETBITS(3 << 0, 1 << 0)
#define IOMUX_SPI0	RK_CLRSETBITS(0xff << 8, 1 << 14 | 1 << 12 | \
					      1 << 10 | 1 << 8)
#define IOMUX_SPI2_CSCLK RK_CLRSETBITS(3 << 14 | 3 << 12, 1 << 14 | 1 << 12)
#define IOMUX_SPI2_TXRX  RK_CLRSETBITS(3 << 2 | 3 << 0, 1 << 2 | 1 << 0)
#define IOMUX_I2S	RK_SETBITS(1 << 8 | 1 << 6 | 1 << 4 | 1 << 2 | 1 << 0)
#define IOMUX_I2SCLK	RK_SETBITS(1 << 0)
#define IOMUX_UART2	RK_CLRSETBITS(7 << 12 | 3 << 8, 1 << 12 | 1 << 8)
#define IOMUX_LCDC  RK_SETBITS(1 << 6 | 1 << 4 | 1 << 2 | 1 << 0)
#define IOMUX_SDMMC0 RK_CLRSETBITS(0x17ff, 1 << 12 | 1 << 10 | 1 << 8 |\
					   1 << 6 | 1 << 4 | 1 << 2 | 1 << 0)
#define IOMUX_EMMCDATA   RK_CLRSETBITS(0xffff, 2 << 14 | 2 << 12 | 2 << 10 |\
					       2 << 8 | 2 << 6 | 2 << 4 |\
					       2 << 2 | 2 << 0)
#define IOMUX_EMMCPWREN  RK_CLRSETBITS(0x3 << 2, 0x2 << 2)
#define IOMUX_EMMCCMD	 RK_CLRSETBITS(0x3f, 2 << 4 | 2 << 2 | 2 << 0)
#define IOMUX_PWM1	RK_SETBITS(1 << 2)
#define IOMUX_EDP_HOTPLUG	RK_CLRSETBITS(0x3 << 6, 0x2 << 6)
#define IOMUX_HDMI_EDP_I2C_SDA	RK_CLRSETBITS(0x3 << 12, 2 << 12)
#define IOMUX_HDMI_EDP_I2C_SCL	RK_CLRSETBITS(0x3 << 0, 2 << 0)

/* Use to mux a pin back to GPIO function. Since the selector for that is always
 * 0, we can just reuse RK mask/value patterns and mask out the "value" part. */
#define IOMUX_GPIO(iomux_clrsetbits) ((iomux_clrsetbits) & (0xffff << 16))

#endif	/* __SOC_ROCKCHIP_RK3288_GRF_H__ */
