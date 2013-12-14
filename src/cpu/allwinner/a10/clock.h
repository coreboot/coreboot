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

/* APB0_GATING values */
#define APB0_GATE_KEYPAD		(1 << 10)
#define APB0_GATE_IR(x)			(((1 << (x)) & 0x3) << 6)
#define APB0_GATE_PIO			(1 << 5)
#define APB0_GATE_IIS			(1 << 3)
#define APB0_GATE_AC97			(1 << 2)
#define APB0_GATE_CODEC			(1 << 0)

/* APB1_GATING values */
#define APB1_GATE_UART(x)		(((1 << (x)) & 0xff) << 16)
#define APB1_GATE_PS2(x)		(((1 << (x)) & 0x3) << 6)
#define APB1_GATE_CAN			(1 << 4)
#define APB1_GATE_TWI(x)		(((1 << (x)) & 0x7) << 0)

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

#endif				/* CPU_ALLWINNER_A10_CLOCK_H */
