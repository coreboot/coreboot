/*
 * Definitions for clock control and gating on Allwinner CPUs
 *
 * Copyright (C) 2007-2011 Allwinner Technology Co., Ltd.
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_CLOCK_H
#define CPU_ALLWINNER_A10_CLOCK_H

#include "memmap.h"
#include <types.h>

/* CPU_AHB_APB0 config values */
#define CPU_CLK_SRC_MASK		(3 << 16)
#define  CPU_CLK_SRC_OSC24M		(1 << 16)
#define  CPU_CLK_SRC_PLL1		(2 << 16)
#define APB0_DIV_MASK			(3 << 8)
#define  APB0_DIV_1			(0 << 8)
#define  APB0_DIV_2			(1 << 8)
#define  APB0_DIV_4			(2 << 8)
#define  APB0_DIV_8			(3 << 8)
#define AHB_DIV_MASK			(3 << 4)
#define  AHB_DIV_1			(0 << 4)
#define  AHB_DIV_2			(1 << 4)
#define  AHB_DIV_4			(2 << 4)
#define  AHB_DIV_8			(3 << 4)
#define AXI_DIV_MASK			(3 << 0)
#define  AXI_DIV_1			(0 << 0)
#define  AXI_DIV_2			(1 << 0)
#define  AXI_DIV_3			(2 << 0)
#define  AXI_DIV_4			(3 << 0)

/* APB1_CLK_DIV values */
#define APB1_CLK_SRC_MASK		(3 << 24)
#define  APB1_CLK_SRC_OSC24M		(0 << 24)
#define  APB1_CLK_SRC_PLL6		(1 << 24)
#define  APB1_CLK_SRC_32K		(2 << 24)
#define APB1_RAT_N_MASK			(3 << 16)
#define  APB1_RAT_N(m)			(((m) & 0x3) << 16)
#define APB1_RAT_M_MASK			0x1f << 0)
#define  APB1_RAT_M(n)			(((n) & 0x1f) << 0)

/* PLL5_CFG values */
#define PLL5_PLL_ENABLE			(1 << 31)
#define PLL5_OUT_BYPASS_EN		(1 << 30)
#define PLL5_DDR_CLK_OUT_EN		(1 << 29)
#define PLL5_DIV_EXP_P_MASK		(0x3 << 16)
#define PLL5_DIV_EXP_P(ep)		((ep << 16) & PLL5_DIV_EXP_P_MASK)
#define  PLL5_DIV_P_1			(0x0 << 16)
#define  PLL5_DIV_P_2			(0x1 << 16)
#define  PLL5_DIV_P_4			(0x2 << 16)
#define  PLL5_DIV_P_8			(0x3 << 16)
#define PLL5_FACTOR_N_MASK		(0x1f << 8)
#define PLL5_FACTOR_N(n)		((n << 8) & PLL5_FACTOR_N_MASK)
#define PLL5_LDO_EN			(1 << 7)
#define PLL5_FACTOR_K_MASK		(0x3 << 4)
#define PLL5_FACTOR_K(k)		((((k) - 1) << 4) & PLL5_FACTOR_K_MASK)
#define PLL5_FACTOR_M1_MASK		(0x3 << 2)
#define PLL5_FACTOR_M1(m1)		(((m1) << 2) & PLL5_FACTOR_M1_MASK)
#define PLL5_FACTOR_M_MASK		(0x3 << 0)
#define PLL5_FACTOR_M(m)		((((m) - 1) << 0) & PLL5_FACTOR_M_MASK)

/* DRAM_CLK values*/
#define DRAM_CTRL_DCLK_OUT		(1 << 15)

/* SDx_CLK values */
#define SDx_CLK_GATE			(1 << 31)
#define SDx_CLK_SRC_MASK		(3 << 24)
#define  SDx_CLK_SRC_OSC24M		(0 << 24)
#define  SDx_CLK_SRC_PLL6		(1 << 24)
#define  SDx_CLK_SRC_PLL5		(2 << 24)
#define SDx_RAT_EXP_N_MASK		(3 << 16)
#define  SDx_RAT_EXP_N(n)		(((n) << 16) & SDx_RAT_EXP_N_MASK)
#define SDx_RAT_M_MASK			(0xf << 0)
#define  SDx_RAT_M(m)			((((m) - 1) << 0) & SDx_RAT_M_MASK)
/**
 * \brief Clock gating definitions
 *
 * The definitions are specified in the form:
 * 31:5 register offset from A1X_CCM_BASE for the clock register
 * 4:0  bit offset for the given peripheral
 *
 * The names have the form [periph_type][periph_number]
 *
 * These definitions are meant to be used with @ref a1x_periph_clock_enable and
 * @ref a1x_periph_clock_disable
 */

enum a1x_clken {
	/* AXI module clock gating */
	A1X_CLKEN_DRAM_AXI = (0x5C << 5),
	/* AHB0 module clock gating */
	A1X_CLKEN_USB0 = (0x60 << 5),
	A1X_CLKEN_EHCI0,
	RSVD_0x60_2,
	A1X_CLKEN_EHCI1,
	RSVD_0x60_4,
	A1X_CLKEN_SS,
	A1X_CLKEN_DMA,
	A1X_CLKEN_BIST,
	A1X_CLKEN_MMC0,
	A1X_CLKEN_MMC1,
	A1X_CLKEN_MMC2,
	A1X_CLKEN_MMC3,
	A1X_CLKEN_NC,
	A1X_CLKEN_NAND,
	A1X_CLKEN_SDRAM,
	RSVD_0x60_15,
	A1X_CLKEN_ACE,
	A1X_CLKEN_EMAC,
	A1X_CLKEN_TS,
	RSVD_0x60_19,
	A1X_CLKEN_SPI0,
	A1X_CLKEN_SPI1,
	A1X_CLKEN_SPI2,
	A1X_CLKEN_SPI3,
	A1X_CLKEN_PATA,
	RSVD_0x60_25,
	A1X_CLKEN_GPS,
	/* AHB1 module clock gating */
	A1X_CLKEN_DRAM_VE = (0x64 << 5),
	A1X_CLKEN_TVD,
	A1X_CLKEN_TVE0,
	A1X_CLKEN_TVE1,
	A1X_CLKEN_LCD0,
	A1X_CLKEN_LCD1,
	RSVD_0x64_6,
	RSVD_0x64_7,
	A1X_CLKEN_CSI0,
	A1X_CLKEN_CSI1,
	RSVD_0x64_10,
	A1X_CLKEN_HDMI,
	A1X_CLKEN_DE_BE0,
	A1X_CLKEN_DE_BE1,
	A1X_CLKEN_DE_FE0,
	A1X_CLKEN_DE_FE1,
	RSVD_0x64_16,
	RSVD_0x64_17,
	A1X_CLKEN_MP,
	RSVD_0x64_19,
	A1X_CLKEN_MALI400,
	/* APB0 module clock gating */
	A1X_CLKEN_CODEC = (0x68 << 5),
	A1X_CLKEN_NC_APB,
	A1X_CLKEN_AC97,
	A1X_CLKEN_IIS,
	RSVD_0x68_4,
	A1X_CLKEN_PIO,
	A1X_CLKEN_IR0,
	A1X_CLKEN_IR1,
	RSVD_0x68_8,
	RSVD_0x68_9,
	A1X_CLKEN_KEYPAD,
	/* APB1 module clock gating */
	A1X_CLKEN_TWI0 = (0x6C << 5),
	A1X_CLKEN_TWI1,
	A1X_CLKEN_TWI2,
	RSVD_0x6C_3,
	A1X_CLKEN_CAN,
	A1X_CLKEN_SCR,
	A1X_CLKEN_PS20,
	A1X_CLKEN_PS21,
	RSVD_0x6C_8,
	RSVD_0x6C_9,
	RSVD_0x6C_10,
	RSVD_0x6C_11,
	RSVD_0x6C_12,
	RSVD_0x6C_13,
	RSVD_0x6C_14,
	RSVD_0x6C_15,
	A1X_CLKEN_UART0,
	A1X_CLKEN_UART1,
	A1X_CLKEN_UART2,
	A1X_CLKEN_UART3,
	A1X_CLKEN_UART4,
	A1X_CLKEN_UART5,
	A1X_CLKEN_UART6,
	A1X_CLKEN_UART7,
};

struct a10_ccm {
	u32 pll1_cfg;		/* 0x00 pll1 control */
	u32 pll1_tun;		/* 0x04 pll1 tuning */
	u32 pll2_cfg;		/* 0x08 pll2 control */
	u32 pll2_tun;		/* 0x0c pll2 tuning */
	u32 pll3_cfg;		/* 0x10 pll3 control */
	u8 res0[0x4];
	u32 pll4_cfg;		/* 0x18 pll4 control */
	u8 res1[0x4];
	u32 pll5_cfg;		/* 0x20 pll5 control */
	u32 pll5_tun;		/* 0x24 pll5 tuning */
	u32 pll6_cfg;		/* 0x28 pll6 control */
	u32 pll6_tun;		/* 0x2c pll6 tuning */
	u32 pll7_cfg;		/* 0x30 pll7 control */
	u32 pll1_tun2;		/* 0x34 pll5 tuning2 */
	u8 res2[0x4];
	u32 pll5_tun2;		/* 0x3c pll5 tuning2 */
	u8 res3[0xc];
	u32 pll_lock_dbg;	/* 0x4c pll lock time debug */
	u32 osc24m_cfg;		/* 0x50 osc24m control */
	u32 cpu_ahb_apb0_cfg;	/* 0x54 cpu,ahb and apb0 divide ratio */
	u32 apb1_clk_div_cfg;	/* 0x58 apb1 clock dividor */
	u32 axi_gate;		/* 0x5c axi module clock gating */
	u32 ahb_gate0;		/* 0x60 ahb module clock gating 0 */
	u32 ahb_gate1;		/* 0x64 ahb module clock gating 1 */
	u32 apb0_gate;		/* 0x68 apb0 module clock gating */
	u32 apb1_gate;		/* 0x6c apb1 module clock gating */
	u8 res4[0x10];
	u32 nand_sclk_cfg;	/* 0x80 nand sub clock control */
	u32 ms_sclk_cfg;	/* 0x84 memory stick sub clock control */
	u32 sd0_clk_cfg;	/* 0x88 sd0 clock control */
	u32 sd1_clk_cfg;	/* 0x8c sd1 clock control */
	u32 sd2_clk_cfg;	/* 0x90 sd2 clock control */
	u32 sd3_clk_cfg;	/* 0x94 sd3 clock control */
	u32 ts_clk_cfg;		/* 0x98 transport stream clock control */
	u32 ss_clk_cfg;		/* 0x9c */
	u32 spi0_clk_cfg;	/* 0xa0 */
	u32 spi1_clk_cfg;	/* 0xa4 */
	u32 spi2_clk_cfg;	/* 0xa8 */
	u32 pata_clk_cfg;	/* 0xac */
	u32 ir0_clk_cfg;	/* 0xb0 */
	u32 ir1_clk_cfg;	/* 0xb4 */
	u32 iis_clk_cfg;	/* 0xb8 */
	u32 ac97_clk_cfg;	/* 0xbc */
	u32 spdif_clk_cfg;	/* 0xc0 */
	u32 keypad_clk_cfg;	/* 0xc4 */
	u32 sata_clk_cfg;	/* 0xc8 */
	u32 usb_clk_cfg;	/* 0xcc */
	u32 gps_clk_cfg;	/* 0xd0 */
	u32 spi3_clk_cfg;	/* 0xd4 */
	u8 res5[0x28];
	u32 dram_clk_cfg;	/* 0x100 */
	u32 be0_clk_cfg;	/* 0x104 */
	u32 be1_clk_cfg;	/* 0x108 */
	u32 fe0_clk_cfg;	/* 0x10c */
	u32 fe1_clk_cfg;	/* 0x110 */
	u32 mp_clk_cfg;		/* 0x114 */
	u32 lcd0_ch0_clk_cfg;	/* 0x118 */
	u32 lcd1_ch0_clk_cfg;	/* 0x11c */
	u32 csi_isp_clk_cfg;	/* 0x120 */
	u8 res6[0x4];
	u32 tvd_clk_reg;	/* 0x128 */
	u32 lcd0_ch1_clk_cfg;	/* 0x12c */
	u32 lcd1_ch1_clk_cfg;	/* 0x130 */
	u32 csi0_clk_cfg;	/* 0x134 */
	u32 csi1_clk_cfg;	/* 0x138 */
	u32 ve_clk_cfg;		/* 0x13c */
	u32 audio_codec_clk_cfg;	/* 0x140 */
	u32 avs_clk_cfg;	/* 0x144 */
	u32 ace_clk_cfg;	/* 0x148 */
	u32 lvds_clk_cfg;	/* 0x14c */
	u32 hdmi_clk_cfg;	/* 0x150 */
	u32 mali_clk_cfg;	/* 0x154 */
	u8 res7[0x4];
	u32 mbus_clk_cfg;	/* 0x15c */
} __attribute__ ((packed));

void a1x_periph_clock_enable(enum a1x_clken periph);
void a1x_periph_clock_disable(enum a1x_clken periph);

void a1x_pll5_configure(u8 mul_n, u8 mul_k, u8 div_m, u8 exp_div_p);
void a1x_pll5_enable_dram_clock_output(void);
void a1x_ungate_dram_clock_output(void);
void a1x_gate_dram_clock_output(void);

/* Not available in bootblock */
void a1x_set_cpu_clock(u16 cpu_clk_mhz);

#endif				/* CPU_ALLWINNER_A10_CLOCK_H */
