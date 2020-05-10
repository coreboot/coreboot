/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __CPU_TI_AM335X_CLOCK_H__
#define __CPU_TI_AM335X_CLOCK_H__

#include <stdint.h>

enum {
	CM_ST_NO_SLEEP = 0x0,
	CM_ST_SW_SLEEP = 0x1,
	CM_ST_SW_WKUP = 0x2
};

enum {
	CM_MODULEMODE_DISABLED = 0x0,
	CM_MODULEMODE_ENABLED = 0x2
};

enum {
	CM_FCLK_DIS = 0x0 << 18,
	CM_FCLK_EN = 0x1 << 18
};

/* Clock module peripheral registers */
struct am335x_cm_per_regs {
	uint32_t l4ls_st;		// 0x0
	uint32_t l3s_st;		// 0x4
	uint8_t _rsv0[4];		// 0x8-0xb
	uint32_t l3_st;			// 0xc
	uint8_t _rsv1[4];		// 0x10-0x13
	uint32_t cpgmac0;		// 0x14
	uint32_t lcdc;			// 0x18
	uint32_t usb0;			// 0x1c
	uint8_t _rsv2[4];		// 0x20-0x23
	uint32_t tptc0;			// 0x24
	uint32_t emif;			// 0x28
	uint32_t ocmcram;		// 0x2c
	uint32_t gpmc;			// 0x30
	uint32_t mcasp0;		// 0x34
	uint32_t uart5;			// 0x38
	uint32_t mmc0;			// 0x3c
	uint32_t elm;			// 0x40
	uint32_t i2c2;			// 0x44
	uint32_t i2c1;			// 0x48
	uint32_t spi0;			// 0x4c
	uint32_t spi1;			// 0x50
	uint8_t _rsv3[0xc];		// 0x54-0x5f
	uint32_t l4ls;			// 0x60
	uint8_t _rsv4[4];		// 0x64-0x67
	uint32_t mcasp1;		// 0x68
	uint32_t uart1;			// 0x6c
	uint32_t uart2;			// 0x70
	uint32_t uart3;			// 0x74
	uint32_t uart4;			// 0x78
	uint32_t timer7;		// 0x7c
	uint32_t timer2;		// 0x80
	uint32_t timer3;		// 0x84
	uint32_t timer4;		// 0x88
	uint8_t _rsv5[0x20];		// 0x90-0xab
	uint32_t gpio1;			// 0xac
	uint32_t gpio2;			// 0xb0
	uint32_t gpio3;			// 0xb4
	uint8_t _rsv6[4];		// 0xb8-0xbb
	uint32_t tpcc;			// 0xbc
	uint32_t dcan0;			// 0xc0
	uint32_t dcan1;			// 0xc4
	uint8_t _rsv7[4];		// 0xc8-0xcb
	uint32_t epwmss1;		// 0xcc
	uint8_t _rsv8[4];		// 0xd0-0xd3
	uint32_t epwmss0;		// 0xd4
	uint32_t epwmss2;		// 0xd8
	uint32_t l3_instr;		// 0xdc
	uint32_t l3;			// 0xe0
	uint32_t ieee5000;		// 0xe4
	uint32_t pru_icss;		// 0xe8
	uint32_t timer5;		// 0xec
	uint32_t timer6;		// 0xf0
	uint32_t mmc1;			// 0xf4
	uint32_t mmc2;			// 0xf8
	uint32_t tptc1;			// 0xfc
	uint32_t tptc2;			// 0x100
	uint8_t _rsv9[8];		// 0x104-0x10b
	uint32_t spinlock;		// 0x10c
	uint32_t mailbox0;		// 0x110
	uint8_t _rsv10[8];		// 0x114-0x11b
	uint32_t l4hs_st;		// 0x11c
	uint32_t l4hs;			// 0x120
	uint8_t _rsv11[8];		// 0x124-0x12b
	uint32_t ocpwp_l3_st;		// 0x12c
	uint32_t ocpwp;			// 0x130
	uint8_t _rsv12[0xb];		// 0x134-0x13f
	uint32_t pru_icss_st;		// 0x140
	uint32_t cpsw_st;		// 0x144
	uint32_t lcdc_st;		// 0x148
	uint32_t clkdiv32k;		// 0x14c
	uint32_t clk_24mhz_st;		// 0x150
} __packed;
static struct am335x_cm_per_regs * const am335x_cm_per = (void *)0x44e00000;

/* Clock module wakeup registers */
struct am335x_cm_wkup_regs {
	uint32_t wkup_st;		// 0x0
	uint32_t wkup_control;		// 0x4
	uint32_t wkup_gpio0;		// 0x8
	uint32_t wkup_l4wkup;		// 0xc
	uint32_t wkup_timer0;		// 0x10
	uint32_t wkup_debugss;		// 0x14
	uint32_t l3_aon_st;		// 0x18
	uint32_t autoidle_dpll_mpu;	// 0x1c
	uint32_t idlest_dpll_mpu;	// 0x20
	uint32_t ssc_deltamstep_dpll_mpu;	// 0x24
	uint32_t ssc_modfreqdiv_dpll_mpu;	// 0x28
	uint32_t clksel_dpll_mpu;	// 0x2c
	uint32_t autoidle_dpll_ddr;	// 0x30
	uint32_t idlest_dpll_ddr;	// 0x34
	uint32_t ssc_deltamstep_dpll_ddr;	// 0x38
	uint32_t ssc_modfreqdiv_dpll_ddr;	// 0x3c
	uint32_t clksel_dpll_ddr;	// 0x40
	uint32_t autoidle_dpll_disp;	// 0x44
	uint32_t idlest_dpll_disp;	// 0x48
	uint32_t ssc_deltamstep_dpll_disp;	// 0x4c
	uint32_t ssc_modfreqdiv_dpll_disp;	// 0x50
	uint32_t clksel_dpll_disp;	// 0x54
	uint32_t autoidle_dpll_core;	// 0x58
	uint32_t idlest_dpll_core;	// 0x5c
	uint32_t ssc_deltamstep_dpll_core;	// 0x60
	uint32_t ssc_modfreqdiv_dpll_core;	// 0x64
	uint32_t clksel_dpll_core;	// 0x68
	uint32_t autoidle_dpll_per;	// 0x6c
	uint32_t idlest_dpll_per;	// 0x70
	uint32_t ssc_deltamstep_dpll_per;	// 0x74
	uint32_t ssc_modfreqdiv_dpll_per;	// 0x78
	uint32_t clkdcoldo_dpll_per;	// 0x7c
	uint32_t div_m4_dpll_core;	// 0x80
	uint32_t div_m5_dpll_core;	// 0x84
	uint32_t clkmode_dpll_mpu;	// 0x88
	uint32_t clkmode_dpll_per;	// 0x8c
	uint32_t clkmode_dpll_core;	// 0x90
	uint32_t clkmode_dpll_ddr;	// 0x94
	uint32_t clkmode_dpll_disp;	// 0x98
	uint32_t clksel_dpll_periph;	// 0x9c
	uint32_t div_m2_dpll_ddr;	// 0xa0
	uint32_t div_m2_dpll_disp;	// 0xa4
	uint32_t div_m2_dpll_mpu;	// 0xa8
	uint32_t div_m2_dpll_per;	// 0xac
	uint32_t wkup_wkup_m3;		// 0xb0
	uint32_t wkup_uart0;		// 0xb4
	uint32_t wkup_i2c0;		// 0xb8
	uint32_t wkup_adc_tsc;		// 0xbc
	uint32_t wkup_smartreflex0;	// 0xc0
	uint32_t wkup_timer1;		// 0xc4
	uint32_t wkup_smartreflex1;	// 0xc8
	uint32_t l4_wkup_aon_st;	// 0xcc
	uint8_t _rsv0[4];		// 0xd0-0xd3
	uint32_t wkup_wdt1;		// 0xd4
	uint32_t div_m6_dpll_core;	// 0xd8
} __packed;
static struct am335x_cm_wkup_regs * const am335x_cm_wkup = (void *)0x44e00400;

/* Clock module pll registers */
struct am335x_cm_dpll_regs {
	uint8_t _rsv0[4];		// 0x0-0x3
	uint32_t clksel_timer7_clk;	// 0x4
	uint32_t clksel_timer2_clk;	// 0x8
	uint32_t clksel_timer3_clk;	// 0xc
	uint32_t clksel_timer4_clk;	// 0x10
	uint32_t cm_mac_clksel;		// 0x14
	uint32_t clksel_timer5_clk;	// 0x18
	uint32_t clksel_timer6_clk;	// 0x1c
	uint32_t cm_cpts_rft_clksel;	// 0x20
	uint8_t _rsv1[4];		// 0x24-0x27
	uint32_t clksel_timer1ms_clk;	// 0x28
	uint32_t clksel_gfx_fclk;	// 0x2c
	uint32_t clksel_pru_icss_ocp_clk;	// 0x30
	uint32_t clksel_lcdc_pixel_clk;	// 0x34
	uint32_t clksel_wdt1_clk;	// 0x38
	uint32_t clksel_gpio0_dbclk;	// 0x3c
} __packed;
static struct am335x_cm_dpll_regs * const am335x_cm_dpll = (void *)0x44e00500;

/* Clock module mpu registers */
struct am335x_cm_mpu_regs {
	uint32_t st;			// 0x0
	uint32_t mpu;			// 0x4
} __packed;
static struct am335x_cm_mpu_regs * const am335x_cm_mpu = (void *)0x44e00600;

/* Clock module device registers */
struct am335x_cm_device_regs {
	uint32_t cm_clkout_ctrl;	// 0x0
} __packed;
static struct am335x_cm_device_regs * const am335x_cm_device =
		(void *)0x44e00700;

/* Clock module RTC registers */
struct am335x_cm_rtc_regs {
	uint32_t rtc;			// 0x0
	uint32_t st;			// 0x4
} __packed;
static struct am335x_cm_rtc_regs * const am335x_cm_rtc = (void *)0x44e00800;

/* Clock module graphics controller registers */
struct am335x_cm_gfx_regs {
	uint32_t l3_st;			// 0x0
	uint32_t gfx;			// 0x4
	uint8_t _rsv0[4];		// 0x8-0xb
	uint32_t l4ls_gfx_st;		// 0xc
	uint32_t mmucfg;		// 0x10
	uint32_t mmudata;		// 0x14
} __packed;
static struct am335x_cm_gfx_regs * const am335x_cm_gfx = (void *)0x44e00900;

/* Clock module efuse registers */
struct am335x_cm_cefuse_regs {
	uint32_t st;			// 0x0
	uint8_t _rsv0[0x1c];		// 0x4-0x1f
	uint32_t cefuse;		// 0x20
} __packed;
static struct am335x_cm_cefuse_regs * const am335x_cm_cefuse =
		(void *)0x44e00a00;

#endif	/* __CPU_TI_AM335X_CLOCK_H__ */
