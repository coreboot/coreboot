/*
 * Allwinner A10 platform dram register definition.
 *
 * Based on sun4i Linux kernel sources mach-sunxi/pm/standby/dram*.c
 * and earlier U-Boot Allwiner A10 SPL work
 *
 * Copyright (C) 2007-2012 Allwinner Technology Co., Ltd.
 *	Berg Xing <bergxing@allwinnertech.com>
 *	Tom Cubie <tangliang@allwinnertech.com>
 * Copyright (C) 2013  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 * Subject to the GNU GPL v2, or (at your option) any later version.
 */

#ifndef CPU_ALLWINNER_A10_DRAMC_H
#define CPU_ALLWINNER_A10_DRAMC_H

#include <types.h>

#define DRAM_CCR_COMMAND_RATE_1T (0x1 << 5)
#define DRAM_CCR_DQS_GATE (0x1 << 14)
#define DRAM_CCR_DQS_DRIFT_COMP (0x1 << 17)
#define DRAM_CCR_ITM_OFF (0x1 << 28)
#define DRAM_CCR_DATA_TRAINING (0x1 << 30)
#define DRAM_CCR_INIT (0x1 << 31)

#define DRAM_MEMORY_TYPE_DDR1 1
#define DRAM_MEMORY_TYPE_DDR2 2
#define DRAM_MEMORY_TYPE_DDR3 3
#define DRAM_MEMORY_TYPE_LPDDR2 4
#define DRAM_MEMORY_TYPE_LPDDR 5
#define DRAM_DCR_TYPE (0x1 << 0)
#define DRAM_DCR_TYPE_DDR2 0x0
#define DRAM_DCR_TYPE_DDR3 0x1
#define DRAM_DCR_IO_WIDTH(n) (((n) & 0x3) << 1)
#define DRAM_DCR_IO_WIDTH_MASK DRAM_DCR_IO_WIDTH(0x3)
#define DRAM_DCR_IO_WIDTH_8BIT 0x0
#define DRAM_DCR_IO_WIDTH_16BIT 0x1
#define DRAM_DCR_CHIP_DENSITY(n) (((n) & 0x7) << 3)
#define DRAM_DCR_CHIP_DENSITY_MASK DRAM_DCR_CHIP_DENSITY(0x7)
#define DRAM_DCR_CHIP_DENSITY_256M 0x0
#define DRAM_DCR_CHIP_DENSITY_512M 0x1
#define DRAM_DCR_CHIP_DENSITY_1024M 0x2
#define DRAM_DCR_CHIP_DENSITY_2048M 0x3
#define DRAM_DCR_CHIP_DENSITY_4096M 0x4
#define DRAM_DCR_CHIP_DENSITY_8192M 0x5
#define DRAM_DCR_BUS_WIDTH(n) (((n) & 0x7) << 6)
#define DRAM_DCR_BUS_WIDTH_MASK DRAM_DCR_BUS_WIDTH(0x7)
#define DRAM_DCR_BUS_WIDTH_32BIT 0x3
#define DRAM_DCR_BUS_WIDTH_16BIT 0x1
#define DRAM_DCR_BUS_WIDTH_8BIT 0x0
#define DRAM_DCR_NR_DLLCR_32BIT 5
#define DRAM_DCR_NR_DLLCR_16BIT 3
#define DRAM_DCR_NR_DLLCR_8BIT 2
#define DRAM_DCR_RANK_SEL(n) (((n) & 0x3) << 10)
#define DRAM_DCR_RANK_SEL_MASK DRAM_DCR_CMD_RANK(0x3)
#define DRAM_DCR_CMD_RANK_ALL (0x1 << 12)
#define DRAM_DCR_MODE(n) (((n) & 0x3) << 13)
#define DRAM_DCR_MODE_MASK DRAM_DCR_MODE(0x3)
#define DRAM_DCR_MODE_SEQ 0x0
#define DRAM_DCR_MODE_INTERLEAVE 0x1

#define DRAM_CSR_FAILED (0x1 << 20)

#define DRAM_MCR_MODE_NORM(n) (((n) & 0x3) << 0)
#define DRAM_MCR_MODE_NORM_MASK DRAM_MCR_MOD_NORM(0x3)
#define DRAM_MCR_MODE_DQ_OUT(n) (((n) & 0x3) << 2)
#define DRAM_MCR_MODE_DQ_OUT_MASK DRAM_MCR_MODE_DQ_OUT(0x3)
#define DRAM_MCR_MODE_ADDR_OUT(n) (((n) & 0x3) << 4)
#define DRAM_MCR_MODE_ADDR_OUT_MASK DRAM_MCR_MODE_ADDR_OUT(0x3)
#define DRAM_MCR_MODE_DQ_IN_OUT(n) (((n) & 0x3) << 6)
#define DRAM_MCR_MODE_DQ_IN_OUT_MASK DRAM_MCR_MODE_DQ_IN_OUT(0x3)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY(n) (((n) & 0x7) << 8)
#define DRAM_MCR_MODE_DQ_TURNON_DELAY_MASK DRAM_MCR_MODE_DQ_TURNON_DELAY(0x7)
#define DRAM_MCR_MODE_ADDR_IN (0x1 << 11)
#define DRAM_MCR_RESET (0x1 << 12)
#define DRAM_MCR_MODE_EN(n) (((n) & 0x3) << 13)
#define DRAM_MCR_MODE_EN_MASK DRAM_MCR_MOD_EN(0x3)
#define DRAM_MCR_DCLK_OUT (0x1 << 16)

#define DRAM_DLLCR_NRESET (0x1 << 30)
#define DRAM_DLLCR_DISABLE (0x1 << 31)

#define DRAM_ZQCR0_IMP_DIV(n) (((n) & 0xff) << 20)
#define DRAM_ZQCR0_IMP_DIV_MASK DRAM_ZQCR0_IMP_DIV(0xff)

#define DRAM_IOCR_ODT_EN(n) ((((n) & 0x3) << 30) | ((n) & 0x3) << 0)
#define DRAM_IOCR_ODT_EN_MASK DRAM_IOCR_ODT_EN(0x3)

#define DRAM_MR_BURST_LENGTH(n) (((n) & 0x7) << 0)
#define DRAM_MR_BURST_LENGTH_MASK DRAM_MR_BURST_LENGTH(0x7)
#define DRAM_MR_CAS_LAT(n) (((n) & 0x7) << 4)
#define DRAM_MR_CAS_LAT_MASK DRAM_MR_CAS_LAT(0x7)
#define DRAM_MR_WRITE_RECOVERY(n) (((n) & 0x7) << 9)
#define DRAM_MR_WRITE_RECOVERY_MASK DRAM_MR_WRITE_RECOVERY(0x7)
#define DRAM_MR_POWER_DOWN (0x1 << 12)

#define DRAM_CSEL_MAGIC 0x16237495

struct a1x_dramc {
	u32 ccr;		/* 0x00 controller configuration register */
	u32 dcr;		/* 0x04 dram configuration register */
	u32 iocr;		/* 0x08 i/o configuration register */
	u32 csr;		/* 0x0c controller status register */
	u32 drr;		/* 0x10 dram refresh register */
	u32 tpr0;		/* 0x14 dram timing parameters register 0 */
	u32 tpr1;		/* 0x18 dram timing parameters register 1 */
	u32 tpr2;		/* 0x1c dram timing parameters register 2 */
	u32 gdllcr;		/* 0x20 global dll control register */
	u8 res0[0x28];
	u32 rslr0;		/* 0x4c rank system latency register */
	u32 rslr1;		/* 0x50 rank system latency register */
	u8 res1[0x8];
	u32 rdgr0;		/* 0x5c rank dqs gating register */
	u32 rdgr1;		/* 0x60 rank dqs gating register */
	u8 res2[0x34];
	u32 odtcr;		/* 0x98 odt configuration register */
	u32 dtr0;		/* 0x9c data training register 0 */
	u32 dtr1;		/* 0xa0 data training register 1 */
	u32 dtar;		/* 0xa4 data training address register */
	u32 zqcr0;		/* 0xa8 zq control register 0 */
	u32 zqcr1;		/* 0xac zq control register 1 */
	u32 zqsr;		/* 0xb0 zq status register */
	u32 idcr;		/* 0xb4 initializaton delay configure reg */
	u8 res3[0x138];
	u32 mr;			/* 0x1f0 mode register */
	u32 emr;		/* 0x1f4 extended mode register */
	u32 emr2;		/* 0x1f8 extended mode register */
	u32 emr3;		/* 0x1fc extended mode register */
	u32 dllctr;		/* 0x200 dll control register */
	u32 dllcr[5];		/* 0x204 dll control register 0(byte 0) */
	/* 0x208 dll control register 1(byte 1) */
	/* 0x20c dll control register 2(byte 2) */
	/* 0x210 dll control register 3(byte 3) */
	/* 0x214 dll control register 4(byte 4) */
	u32 dqtr0;		/* 0x218 dq timing register */
	u32 dqtr1;		/* 0x21c dq timing register */
	u32 dqtr2;		/* 0x220 dq timing register */
	u32 dqtr3;		/* 0x224 dq timing register */
	u32 dqstr;		/* 0x228 dqs timing register */
	u32 dqsbtr;		/* 0x22c dqsb timing register */
	u32 mcr;		/* 0x230 mode configure register */
	u8 res[0x8];
	u32 ppwrsctl;		/* 0x23c pad power save control */
	u32 apr;		/* 0x240 arbiter period register */
	u32 pldtr;		/* 0x244 priority level data threshold reg */
	u8 res5[0x8];
	u32 hpcr[32];		/* 0x250 host port configure register */
	u8 res6[0x10];
	u32 csel;		/* 0x2e0 controller select register */
};

struct dram_para {
	u32 clock;
	u32 type;
	u32 rank_num;
	u32 density;
	u32 io_width;
	u32 bus_width;
	u32 cas;
	u32 zq;
	u32 odt_en;
	u32 size;
	u32 tpr0;
	u32 tpr1;
	u32 tpr2;
	u32 tpr3;
	u32 tpr4;
	u32 tpr5;
	u32 emr1;
	u32 emr2;
	u32 emr3;
};

unsigned long dramc_init(struct dram_para *para);

#endif				/* CPU_ALLWINNER_A10_DRAMC_H */
