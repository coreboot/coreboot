/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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

#include <arch/io.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/sdram.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <string.h>
#include <types.h>

#define DDR_PI_OFFSET			0x800
#define DDR_PHY_OFFSET			0x2000
#define DDRC0_PI_BASE_ADDR		(DDRC0_BASE_ADDR + DDR_PI_OFFSET)
#define DDRC0_PHY_BASE_ADDR		(DDRC0_BASE_ADDR + DDR_PHY_OFFSET)
#define DDRC1_PI_BASE_ADDR		(DDRC1_BASE_ADDR + DDR_PI_OFFSET)
#define DDRC1_PHY_BASE_ADDR		(DDRC1_BASE_ADDR + DDR_PHY_OFFSET)

static struct rk3399_ddr_pctl_regs * const rk3399_ddr_pctl[2] = {
	(void *)DDRC0_BASE_ADDR, (void *)DDRC1_BASE_ADDR };
static struct rk3399_ddr_pi_regs * const rk3399_ddr_pi[2] = {
	(void *)DDRC0_PI_BASE_ADDR, (void *)DDRC1_PI_BASE_ADDR };
static struct rk3399_ddr_publ_regs * const rk3399_ddr_publ[2] = {
	(void *)DDRC0_PHY_BASE_ADDR, (void *)DDRC1_PHY_BASE_ADDR };
static struct rk3399_msch_regs * const rk3399_msch[2] = {
	(void *)SERVER_MSCH0_BASE_ADDR, (void *)SERVER_MSCH1_BASE_ADDR };

/*
 * sys_reg bitfield struct
 * [31]		row_3_4_ch1
 * [30]		row_3_4_ch0
 * [29:28]	chinfo
 * [27]		rank_ch1
 * [26:25]	col_ch1
 * [24]		bk_ch1
 * [23:22]	cs0_row_ch1
 * [21:20]	cs1_row_ch1
 * [19:18]	bw_ch1
 * [17:16]	dbw_ch1;
 * [15:13]	ddrtype
 * [12]		channelnum
 * [11]		rank_ch0
 * [10:9]	col_ch0
 * [8]		bk_ch0
 * [7:6]	cs0_row_ch0
 * [5:4]	cs1_row_ch0
 * [3:2]	bw_ch0
 * [1:0]	dbw_ch0
*/
#define SYS_REG_ENC_ROW_3_4(n, ch)	((n) << (30 + (ch)))
#define SYS_REG_DEC_ROW_3_4(n, ch)	((n >> (30 + ch)) & 0x1)
#define SYS_REG_ENC_CHINFO(ch)		(1 << (28 + (ch)))
#define SYS_REG_ENC_DDRTYPE(n)		((n) << 13)
#define SYS_REG_ENC_NUM_CH(n)		(((n) - 1) << 12)
#define SYS_REG_DEC_NUM_CH(n)		(1 + ((n >> 12) & 0x1))
#define SYS_REG_ENC_RANK(n, ch)		(((n) - 1) << (11 + ((ch) * 16)))
#define SYS_REG_DEC_RANK(n, ch)		(1 + ((n >> (11 + 16 * ch)) & 0x1))
#define SYS_REG_ENC_COL(n, ch)		(((n) - 9) << (9 + ((ch) * 16)))
#define SYS_REG_DEC_COL(n, ch)		(9 + ((n >> (9 + 16 * ch)) & 0x3))
#define SYS_REG_ENC_BK(n, ch)		(((n) == 3 ? 0 : 1) \
						<< (8 + ((ch) * 16)))
#define SYS_REG_DEC_BK(n, ch)		(3 - ((n >> (8 + 16 * ch)) & 0x1))
#define SYS_REG_ENC_CS0_ROW(n, ch)	(((n) - 13) << (6 + ((ch) * 16)))
#define SYS_REG_DEC_CS0_ROW(n, ch)	(13 + ((n >> (6 + 16 * ch)) & 0x3))
#define SYS_REG_ENC_CS1_ROW(n, ch)	(((n) - 13) << (4 + ((ch) * 16)))
#define SYS_REG_DEC_CS1_ROW(n, ch)	(13 + ((n >> (4 + 16 * ch)) & 0x3))
#define SYS_REG_ENC_BW(n, ch)		((2 >> (n)) << (2 + ((ch) * 16)))
#define SYS_REG_DEC_BW(n, ch)		(2 >> ((n >> (2 + 16 * ch)) & 0x3))
#define SYS_REG_ENC_DBW(n, ch)		((2 >> (n)) << (0 + ((ch) * 16)))
#define SYS_REG_DEC_DBW(n, ch)		(2 >> ((n >> (0 + 16 * ch)) & 0x3))

#define DDR_STRIDE(n)		write32(&rk3399_pmusgrf->soc_con4,\
					(0x1F << (10 + 16)) | (n << 10))

#define PRESET_SGRF_HOLD(n)	((0x1 << (6+16)) | ((n) << 6))
#define PRESET_GPIO0_HOLD(n)	((0x1 << (7+16)) | ((n) << 7))
#define PRESET_GPIO1_HOLD(n)	((0x1 << (8+16)) | ((n) << 8))

#define PHY_DRV_ODT_Hi_Z	(0x0)
#define PHY_DRV_ODT_240		(0x1)
#define PHY_DRV_ODT_120		(0x8)
#define PHY_DRV_ODT_80		(0x9)
#define PHY_DRV_ODT_60		(0xc)
#define PHY_DRV_ODT_48		(0xd)
#define PHY_DRV_ODT_40		(0xe)
#define PHY_DRV_ODT_34_3	(0xf)

static void copy_to_reg(u32 *dest, u32 *src, u32 n)
{
	int i;

	for (i = 0; i < n / sizeof(u32); i++) {
		write32(dest, *src);
		src++;
		dest++;
	}
}

static void ddr_move_to_access_state(u32 channel)
{
}

static void phy_dll_bypass_set(u32 channel,
	struct rk3399_ddr_publ_regs *ddr_publ_regs, u32 freq)
{
	if (freq <= 125*MHz) {
		/* phy_sw_master_mode_X */
		/* PHY_86/214/342/470 4bits offset_8 */
		setbits_le32(&ddr_publ_regs->denali_phy[86],
			     (0x3 << 2) << 8);
		setbits_le32(&ddr_publ_regs->denali_phy[214],
			     (0x3 << 2) << 8);
		setbits_le32(&ddr_publ_regs->denali_phy[342],
			     (0x3 << 2) << 8);
		setbits_le32(&ddr_publ_regs->denali_phy[470],
			     (0x3 << 2) << 8);

		/* phy_adrctl_sw_master_mode */
		/* PHY_547/675/803 4bits offset_16 */
		setbits_le32(&ddr_publ_regs->denali_phy[547],
			     (0x3 << 2) << 16);
		setbits_le32(&ddr_publ_regs->denali_phy[675],
			     (0x3 << 2) << 16);
		setbits_le32(&ddr_publ_regs->denali_phy[803],
			     (0x3 << 2) << 16);
	} else {
		/* phy_sw_master_mode_X */
		/* PHY_86/214/342/470 4bits offset_8 */
		clrbits_le32(&ddr_publ_regs->denali_phy[86],
			     (0x3 << 2) << 8);
		clrbits_le32(&ddr_publ_regs->denali_phy[214],
			     (0x3 << 2) << 8);
		clrbits_le32(&ddr_publ_regs->denali_phy[342],
			     (0x3 << 2) << 8);
		clrbits_le32(&ddr_publ_regs->denali_phy[470],
			     (0x3 << 2) << 8);

		/* phy_adrctl_sw_master_mode */
		/* PHY_547/675/803 4bits offset_16 */
		clrbits_le32(&ddr_publ_regs->denali_phy[547],
			     (0x3 << 2) << 16);
		clrbits_le32(&ddr_publ_regs->denali_phy[675],
			     (0x3 << 2) << 16);
		clrbits_le32(&ddr_publ_regs->denali_phy[803],
			     (0x3 << 2) << 16);
	}
}

static void set_memory_map(u32 channel,
			   const struct rk3399_sdram_params *sdram_params)
{
	struct rk3399_ddr_pctl_regs *ddr_pctl_regs =
		    rk3399_ddr_pctl[channel];
	u32 cs_map;
	u32 reduc;
	struct rk3399_ddr_pi_regs *ddr_pi_regs =
		rk3399_ddr_pi[channel];

	cs_map = (sdram_params->ch[channel].rank > 1) ? 3 : 1;
	reduc = (sdram_params->ch[channel].bw == 2) ? 0 : 1;

	clrsetbits_le32(&ddr_pctl_regs->denali_ctl[191], 0xF,
			(12 - sdram_params->ch[channel].col));

	clrsetbits_le32(&ddr_pctl_regs->denali_ctl[190],
			(0x3 << 16) | (0x7 << 24),
			((3 - sdram_params->ch[channel].bk) << 16) |
			((16 - sdram_params->ch[channel].cs0_row) << 24));

	clrsetbits_le32(&ddr_pctl_regs->denali_ctl[196],
			0x3 | (1 << 16), cs_map | (reduc << 16));

	/* PI_199 PI_COL_DIFF:RW:0:4 */
	clrsetbits_le32(&ddr_pi_regs->denali_pi[199],
			0xF, (12 - sdram_params->ch[channel].col));

	/* PI_155 PI_ROW_DIFF:RW:24:3 PI_BANK_DIFF:RW:16:2 */
	clrsetbits_le32(&ddr_pi_regs->denali_pi[155],
			(0x3 << 16) | (0x7 << 24),
			((3 - sdram_params->ch[channel].bk) << 16) |
			((16 - sdram_params->ch[channel].cs0_row) << 24));
	/* PI_41 PI_CS_MAP:RW:24:4 */
	clrsetbits_le32(&ddr_pi_regs->denali_pi[41],
			0xf << 24, cs_map << 24);
	if ((sdram_params->ch[channel].rank == 1) &&
		(sdram_params->dramtype == DDR3))
		write32(&ddr_pi_regs->denali_pi[34], 0x2EC7FFFF);
}

static void set_ds_odt(u32 channel,
		       const struct rk3399_sdram_params *sdram_params)
{
	struct rk3399_ddr_publ_regs *ddr_publ_regs = rk3399_ddr_publ[channel];

	u32 tsel_idle_en, tsel_wr_en, tsel_rd_en;
	u32 tsel_idle_select, tsel_wr_select, tsel_rd_select;

	tsel_rd_select = PHY_DRV_ODT_240;
	tsel_wr_select = PHY_DRV_ODT_40;
	tsel_idle_select = PHY_DRV_ODT_240;

	if (sdram_params->odt == 1)
		tsel_rd_en = 1;
	else
		tsel_rd_en = 0;

	tsel_wr_en = 0;
	tsel_idle_en = 0;

	/*
	 * phy_dq_tsel_select_X 24bits DENALI_PHY_6/134/262/390 offset_0
	 * sets termination values for read/idle cycles and drive strength
	 * for write cycles for DQ/DM
	 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[6], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[134], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[262], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[390], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	/*
	 * phy_dqs_tsel_select_X 24bits DENALI_PHY_7/135/263/391 offset_0
	 * sets termination values for read/idle cycles and drive strength
	 * for write cycles for DQS
	 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[7], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[135], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[263], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	clrsetbits_le32(&ddr_publ_regs->denali_phy[391], 0xffffff,
			tsel_rd_select | (tsel_rd_select << 0x4) |
			(tsel_wr_select << 8) | (tsel_wr_select << 12) |
			(tsel_idle_select << 16) | (tsel_idle_select << 20));

	/* phy_adr_tsel_select_ 8bits DENALI_PHY_544/672/800 offset_0 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[544], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));
	clrsetbits_le32(&ddr_publ_regs->denali_phy[672], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));
	clrsetbits_le32(&ddr_publ_regs->denali_phy[800], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_addr_drive 29bits DENALI_PHY_928 offset_0 */
	clrsetbits_le32((&ddr_publ_regs->denali_phy[928]), 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_rst_drive 8bits DENALI_PHY_937 offset_0 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[937], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_cke_drive 8bits DENALI_PHY_935 offset_0 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[935], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_cs_drive 8bits DENALI_PHY_939 offset_0 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[939], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_clk_drive 8bits DENALI_PHY_929 offset_0 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[929], 0xff,
			tsel_wr_select | (tsel_wr_select << 0x4));

	/* phy_pad_fdbk_drive 23bit DENALI_PHY_924/925 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[924], 0xff,
			tsel_wr_select | (tsel_wr_select << 4));
	clrsetbits_le32(&ddr_publ_regs->denali_phy[925], 0xff,
			tsel_rd_select | (tsel_rd_select << 4));

	/* phy_dq_tsel_enable_X 3bits DENALI_PHY_5/133/261/389 offset_16 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[5], 0x7 << 16,
		(tsel_rd_en | (tsel_wr_en << 1) | (tsel_idle_en << 2)) << 16);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[133], 0x7 << 16,
		(tsel_rd_en | (tsel_wr_en << 1) | (tsel_idle_en << 2)) << 16);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[261], 0x7 << 16,
		(tsel_rd_en | (tsel_wr_en << 1) | (tsel_idle_en << 2)) << 16);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[389], 0x7 << 16,
		(tsel_rd_en | (tsel_wr_en << 1) | (tsel_idle_en << 2)) << 16);

	/* phy_dqs_tsel_enable_X 3bits DENALI_PHY_6/134/262/390 offset_24 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[6], 0x7 << 24,
			(tsel_rd_en | (tsel_wr_en << 1) |
			(tsel_idle_en << 2)) << 24);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[134], 0x7 << 24,
			(tsel_rd_en | (tsel_wr_en << 1) |
			(tsel_idle_en << 2)) << 24);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[262], 0x7 << 24,
			(tsel_rd_en | (tsel_wr_en << 1) |
			(tsel_idle_en << 2)) << 24);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[390], 0x7 << 24,
			(tsel_rd_en | (tsel_wr_en << 1) |
			(tsel_idle_en << 2)) << 24);

	/* phy_adr_tsel_enable_ 1bit DENALI_PHY_518/646/774 offset_8 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[518],
			0x1 << 8, tsel_wr_en << 8);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[646],
			0x1 << 8, tsel_wr_en << 8);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[774],
			0x1 << 8, tsel_wr_en << 8);

	/* phy_pad_addr_term tsel 1bit DENALI_PHY_933 offset_17 */
	clrsetbits_le32((&ddr_publ_regs->denali_phy[933]),
			0x1 << 17, tsel_wr_en << 17);

	/*
	 * pad_rst/cke/cs/clk_term tsel 1bits
	 * DENALI_PHY_938/936/940/934 offset_17
	 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[938],
			0x1 << 17, tsel_wr_en << 17);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[936],
			0x1 << 17, tsel_wr_en << 17);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[940],
			0x1 << 17, tsel_wr_en << 17);
	clrsetbits_le32(&ddr_publ_regs->denali_phy[934],
			0x1 << 17, tsel_wr_en << 17);

	/* phy_pad_fdbk_term 1bit DENALI_PHY_930 offset_17 */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[930],
			0x1 << 17, tsel_wr_en << 17);
}

static void pctl_cfg(u32 channel,
		     const struct rk3399_sdram_params *sdram_params)
{
	struct rk3399_ddr_pctl_regs *ddr_pctl_regs = rk3399_ddr_pctl[channel];
	struct rk3399_ddr_pi_regs *ddr_pi_regs = rk3399_ddr_pi[channel];
	struct rk3399_ddr_publ_regs *ddr_publ_regs = rk3399_ddr_publ[channel];
	u32 tmp, tmp1, tmp2;
	u32 pwrup_srefresh_exit;

	/* workaround controller bug:
	 * Do not program DRAM_CLASS until NO_PHY_IND_TRAIN_INT is programmed
	 */
	copy_to_reg(&ddr_pctl_regs->denali_ctl[1],
		(u32 *)&sdram_params->pctl_regs.denali_ctl[1],
		sizeof(struct rk3399_ddr_pctl_regs) - 4);
	write32(&ddr_pctl_regs->denali_ctl[0],
		sdram_params->pctl_regs.denali_ctl[0]);
	copy_to_reg((u32 *)ddr_pi_regs,
		(u32 *)&sdram_params->pi_regs.denali_pi[0],
		sizeof(struct rk3399_ddr_pi_regs));
	/* rank count need to set for init */
	set_memory_map(channel, sdram_params);

	write32(&ddr_publ_regs->denali_phy[910], 0x6400);
	write32(&ddr_publ_regs->denali_phy[911], 0x01221102);
	write32(&ddr_publ_regs->denali_phy[912], 0x0);
	pwrup_srefresh_exit = read32(&ddr_pctl_regs->denali_ctl[68]) &
			      PWRUP_SREFRESH_EXIT;
	clrbits_le32(&ddr_pctl_regs->denali_ctl[68], PWRUP_SREFRESH_EXIT);

	/* PHY_DLL_RST_EN */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[957],
			0x3 << 24, 1 << 24);

	setbits_le32(&ddr_pi_regs->denali_pi[0], START);
	setbits_le32(&ddr_pctl_regs->denali_ctl[0], START);

	while (1) {
		tmp = read32(&ddr_publ_regs->denali_phy[920]);
		tmp1 = read32(&ddr_publ_regs->denali_phy[921]);
		tmp2 = read32(&ddr_publ_regs->denali_phy[922]);
		if ((((tmp >> 16) & 0x1) == 0x1) &&
		    (((tmp1 >> 16) & 0x1) == 0x1) &&
		    (((tmp1 >> 0) & 0x1) == 0x1) &&
		    (((tmp2 >> 0) & 0x1) == 0x1))
			break;
	}

	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[896],
		    (u32 *)&sdram_params->phy_regs.denali_phy[896],
		    (958 - 895) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[0],
		    (u32 *)&sdram_params->phy_regs.denali_phy[0],
		    (90 - 0 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[128],
		    (u32 *)&sdram_params->phy_regs.denali_phy[128],
		    (218 - 128 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[256],
		    (u32 *)&sdram_params->phy_regs.denali_phy[256],
		    (346 - 256 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[384],
		    (u32 *)&sdram_params->phy_regs.denali_phy[384],
		    (474 - 384 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[512],
		    (u32 *)&sdram_params->phy_regs.denali_phy[512],
		    (549 - 512 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[640],
		    (u32 *)&sdram_params->phy_regs.denali_phy[640],
		    (677 - 640 + 1) * 4);
	copy_to_reg((u32 *)&ddr_publ_regs->denali_phy[768],
		    (u32 *)&sdram_params->phy_regs.denali_phy[768],
		    (805 - 768 + 1) * 4);
	set_ds_odt(channel, sdram_params);

	/* phy_dqs_tsel_wr_timing_X 8bits DENALI_PHY_84/212/340/468 offset_8 */
	/* dqs_tsel_wr_end[7:4] add Half cycle */
	tmp = (read32(&ddr_publ_regs->denali_phy[84]) >> 8) & 0xff;
	clrsetbits_le32((&ddr_publ_regs->denali_phy[84]),
			0xff << 8, (tmp + 0x10) << 8);
	tmp = (read32(&ddr_publ_regs->denali_phy[212]) >> 8) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[212],
			0xff << 8, (tmp + 0x10) << 8);
	tmp = (read32(&ddr_publ_regs->denali_phy[340]) >> 8) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[340],
			0xff << 8, (tmp + 0x10) << 8);
	tmp = (read32(&ddr_publ_regs->denali_phy[468]) >> 8) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[468],
			0xff << 8, (tmp + 0x10) << 8);

	/* phy_dqs_tsel_wr_timing_X 8bits DENALI_PHY_83/211/339/467 offset_8 */
	/* dq_tsel_wr_end[7:4] add Half cycle */
	tmp = (read32(&ddr_publ_regs->denali_phy[83]) >> 16) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[83],
			0xff << 16, (tmp + 0x10) << 16);
	tmp = (read32(&ddr_publ_regs->denali_phy[211]) >> 16) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[211],
			0xff << 16, (tmp + 0x10) << 16);
	tmp = (read32(&ddr_publ_regs->denali_phy[339]) >> 16) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[339],
			0xff << 16, (tmp + 0x10) << 16);
	tmp = (read32(&ddr_publ_regs->denali_phy[467]) >> 16) & 0xff;
	clrsetbits_le32(&ddr_publ_regs->denali_phy[467],
			0xff << 16, (tmp + 0x10) << 16);

	/* PHY_DLL_RST_EN */
	clrsetbits_le32(&ddr_publ_regs->denali_phy[957],
			0x3 << 24, 0x2 << 24);

	/* FIXME:
	 * need to care ERROR bit
	 */
	while (!(read32(&ddr_pctl_regs->denali_ctl[203]) & (1 << 3)))
		;
	clrsetbits_le32(&ddr_pctl_regs->denali_ctl[68],
			PWRUP_SREFRESH_EXIT,
			pwrup_srefresh_exit);
}

static void select_per_cs_training_index(u32 channel, u32 rank)
{
	struct rk3399_ddr_publ_regs *ddr_publ_regs = rk3399_ddr_publ[channel];

	/*PHY_84 PHY_PER_CS_TRAINING_EN_0 1bit offset_16*/
	if ((read32(&ddr_publ_regs->denali_phy[84])>>16) & 1) {
		/*PHY_8/136/264/392
		*phy_per_cs_training_index_X 1bit offset_24
		*/
		clrsetbits_le32(&ddr_publ_regs->denali_phy[8],
				0x1 << 24, rank << 24);
		clrsetbits_le32(&ddr_publ_regs->denali_phy[136],
				0x1 << 24, rank << 24);
		clrsetbits_le32(&ddr_publ_regs->denali_phy[264],
				0x1 << 24, rank << 24);
		clrsetbits_le32(&ddr_publ_regs->denali_phy[392],
				0x1 << 24, rank << 24);
	}
}

static int data_training(u32 channel,
			 const struct rk3399_sdram_params *sdram_params,
			 u32 training_flag)
{
	struct rk3399_ddr_pi_regs *ddr_pi_regs = rk3399_ddr_pi[channel];
	struct rk3399_ddr_publ_regs *ddr_publ_regs = rk3399_ddr_publ[channel];
	u32 i, tmp;
	u32 obs_0, obs_1, obs_2, obs_3, obs_err = 0;
	u32 rank = sdram_params->ch[channel].rank;

	/* PHY_927 PHY_PAD_DQS_DRIVE  RPULL offset_22 */
	setbits_le32(&ddr_publ_regs->denali_phy[927], (1 << 22));

	if (training_flag == PI_FULL_TARINING) {
		if (sdram_params->dramtype == LPDDR4) {
			training_flag = PI_CA_TRAINING | PI_WRITE_LEVELING |
					PI_READ_GATE_TRAINING |
					PI_READ_LEVELING |
					PI_WDQ_LEVELING;
		} else if (sdram_params->dramtype == LPDDR3) {
			training_flag =  PI_READ_GATE_TRAINING;
		} else if (sdram_params->dramtype == DDR3) {
			training_flag =  PI_WRITE_LEVELING |
					 PI_READ_GATE_TRAINING |
					 PI_READ_LEVELING;
		}
	}

	/* PI_58 PI_16BIT_DRAM_CONNECT:RW_D:8:1 */
	if (sdram_params->ch[channel].bw == 1)
		setbits_le32(&ddr_pi_regs->denali_pi[58], 0x1 << 8);
	else
		clrbits_le32(&ddr_pi_regs->denali_pi[58], 0x1 << 8);

	/* ca tarining(LPDDR4,LPDDR3 support) */
	if ((training_flag & PI_CA_TRAINING) == PI_CA_TRAINING) {
		for (i = 0; i < rank; i++) {
			/* PI_100 PI_CALVL_EN:RW:8:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[100],
					0x3 << 8,
					0x2 << 8);
			/* PI_92 PI_CALVL_REQ:WR:16:1,PI_CALVL_CS:RW:24:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[92],
					(0x1 << 16) | (0x3 << 24),
					(0x1 << 16) | (i << 24));
			select_per_cs_training_index(channel, rank);
			while (1) {
				/* PI_174 PI_INT_STATUS:RD:8:18 */
				tmp = read32(&ddr_pi_regs->denali_pi[174]) >> 8;
				/* check status obs */
				/* PHY_532/660/789 phy_adr_calvl_obs1_:0:32 */
				obs_0 = read32(&ddr_publ_regs->denali_phy[532]);
				obs_1 = read32(&ddr_publ_regs->denali_phy[660]);
				obs_2 = read32(&ddr_publ_regs->denali_phy[789]);
				if (((obs_0 >> 30) & 0x3) ||
				    ((obs_1 >> 30) & 0x3) ||
				    ((obs_2 >> 30) & 0x3))
					obs_err = 1;
				if ((((tmp >> 11) & 0x1) == 0x1) &&
				    (((tmp >> 13) & 0x1) == 0x1) &&
				    (((tmp >> 5) & 0x1) == 0x0) &&
				    (obs_err == 0))
					break;
				else if ((((tmp >> 5) & 0x1) == 0x1) ||
					 (obs_err == 1))
					return -1;
			}
			/* clear interrupt,PI_175 PI_INT_ACK:WR:0:17 */
			write32((&ddr_pi_regs->denali_pi[175]), 0x00003f7c);
		}
	}

	/* write leveling(LPDDR4,LPDDR3,DDR3 support) */
	if ((training_flag & PI_WRITE_LEVELING) == PI_WRITE_LEVELING) {
		for (i = 0; i < rank; i++) {
			/* PI_60 PI_WRLVL_EN:RW:8:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[60],
					0x3 << 8,
					0x2 << 8);
			/* PI_59 PI_WRLVL_REQ:WR:8:1,PI_WRLVL_CS:RW:16:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[59],
					(0x1 << 8) | (0x3 << 16),
					(0x1 << 8) | (i << 16));
			select_per_cs_training_index(channel, rank);
			while (1) {

				/* PI_174 PI_INT_STATUS:RD:8:18 */
				tmp = read32(&ddr_pi_regs->denali_pi[174]) >> 8;

				/*
				 * check status obs,
				 * if error maybe can not get leveling done
				 * PHY_40/168/296/424
				 *     phy_wrlvl_status_obs_X:0:13
				 */
				obs_0 = read32(&ddr_publ_regs->denali_phy[40]);
				obs_1 = read32(&ddr_publ_regs->denali_phy[168]);
				obs_2 = read32(&ddr_publ_regs->denali_phy[296]);
				obs_3 = read32(&ddr_publ_regs->denali_phy[424]);
				if (((obs_0 >> 12) & 0x1) ||
				    ((obs_1 >> 12) & 0x1) ||
				    ((obs_2 >> 12) & 0x1) ||
				    ((obs_3 >> 12) & 0x1))
					obs_err = 1;
				if ((((tmp >> 10) & 0x1) == 0x1) &&
				    (((tmp >> 13) & 0x1) == 0x1) &&
				    (((tmp >> 4) & 0x1) == 0x0) &&
				    (obs_err == 0))
					break;
				else if ((((tmp >> 4) & 0x1) == 0x1) ||
					 (obs_err == 1))
					return -1;
			}
			/* clear interrupt,PI_175 PI_INT_ACK:WR:0:17 */
			write32((&ddr_pi_regs->denali_pi[175]), 0x00003f7c);
		}
	}

	/* read gate training(LPDDR4,LPDDR3,DDR3 support) */
	if ((training_flag & PI_READ_GATE_TRAINING) == PI_READ_GATE_TRAINING) {
		for (i = 0; i < rank; i++) {
			/* PI_80 PI_RDLVL_GATE_EN:RW:24:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[80],
					0x3 << 24,
					0x2 << 24);
			/* PI_74 PI_RDLVL_GATE_REQ:WR:16:1 */
			/* PI_RDLVL_CS:RW:24:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[74],
					(0x1 << 16) | (0x3 << 24),
					(0x1 << 16) | (i << 24));
			select_per_cs_training_index(channel, rank);
			while (1) {
				/* PI_174 PI_INT_STATUS:RD:8:18 */
				tmp = read32(&ddr_pi_regs->denali_pi[174]) >> 8;
				/* check status obs
				 * PHY_43/171/299/427
				 *     PHY_GTLVL_STATUS_OBS_x:16:8
				 */
				obs_0 = read32(&ddr_publ_regs->denali_phy[43]);
				obs_1 = read32(&ddr_publ_regs->denali_phy[171]);
				obs_2 = read32(&ddr_publ_regs->denali_phy[299]);
				obs_3 = read32(&ddr_publ_regs->denali_phy[427]);
				if (((obs_0 >> (16 + 6)) & 0x3) ||
				    ((obs_1 >> (16 + 6)) & 0x3) ||
				    ((obs_2 >> (16 + 6)) & 0x3) ||
				    ((obs_3 >> (16 + 6)) & 0x3))
					obs_err = 1;
				if ((((tmp >> 9) & 0x1) == 0x1) &&
				    (((tmp >> 13) & 0x1) == 0x1) &&
				    (((tmp >> 3) & 0x1) == 0x0) &&
				    (obs_err == 0))
					break;
				else if ((((tmp >> 3) & 0x1) == 0x1) ||
					 (obs_err == 1))
					return -1;
			}
			/* clear interrupt,PI_175 PI_INT_ACK:WR:0:17 */
			write32((&ddr_pi_regs->denali_pi[175]), 0x00003f7c);
		}
	}

	/* read leveling(LPDDR4,LPDDR3,DDR3 support) */
	if ((training_flag & PI_READ_LEVELING) == PI_READ_LEVELING) {
		for (i = 0; i < rank; i++) {
			/* PI_80 PI_RDLVL_EN:RW:16:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[80],
					0x3 << 16,
					0x2 << 16);
			/* PI_74 PI_RDLVL_REQ:WR:8:1,PI_RDLVL_CS:RW:24:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[74],
					(0x1 << 8) | (0x3 << 24),
					(0x1 << 8) | (i << 24));
			select_per_cs_training_index(channel, rank);
			while (1) {
				/* PI_174 PI_INT_STATUS:RD:8:18 */
				tmp = read32(&ddr_pi_regs->denali_pi[174]) >> 8;
				/* make sure status obs not report error bit
				 * PHY_46/174/302/430
				 *     phy_rdlvl_status_obs_X:16:8
				 */
				if ((((tmp >> 8) & 0x1) == 0x1) &&
				    (((tmp >> 13) & 0x1) == 0x1) &&
				    (((tmp >> 2) & 0x1) == 0x0))
					break;
				else if (((tmp >> 2) & 0x1) == 0x1)
					return -1;
			}
			/* clear interrupt,PI_175 PI_INT_ACK:WR:0:17 */
			write32((&ddr_pi_regs->denali_pi[175]), 0x00003f7c);
		}
	}

	/* wdq leveling(LPDDR4 support) */
	if ((training_flag & PI_WDQ_LEVELING) == PI_WDQ_LEVELING) {
		for (i = 0; i < rank; i++) {
			/* disable PI_WDQLVL_VREF_EN before wdq leveling? */
			/* PI_181 PI_WDQLVL_VREF_EN:RW:8:1 */
			clrbits_le32(&ddr_pi_regs->denali_pi[181], 0x1 << 8);
			/* PI_124 PI_WDQLVL_EN:RW:16:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[124],
					0x3 << 16,
					0x2 << 16);
			/* PI_121 PI_WDQLVL_REQ:WR:8:1,PI_WDQLVL_CS:RW:16:2 */
			clrsetbits_le32(&ddr_pi_regs->denali_pi[121],
					(0x1 << 8) | (0x3 << 16),
					(0x1 << 8) | (i << 16));
			select_per_cs_training_index(channel, rank);
			while (1) {
				/* PI_174 PI_INT_STATUS:RD:8:18 */
				tmp = read32(&ddr_pi_regs->denali_pi[174]) >> 8;
				if ((((tmp >> 12) & 0x1) == 0x1) &&
				    (((tmp >> 13) & 0x1) == 0x1) &&
				    (((tmp >> 6) & 0x1) == 0x0))
					break;
				else if (((tmp >> 6) & 0x1) == 0x1)
					return -1;
			}
			/* clear interrupt,PI_175 PI_INT_ACK:WR:0:17 */
			write32((&ddr_pi_regs->denali_pi[175]), 0x00003f7c);
		}
	}

	/* PHY_927 PHY_PAD_DQS_DRIVE  RPULL offset_22 */
	clrbits_le32(&ddr_publ_regs->denali_phy[927], (1 << 22));

	return 0;
}

static void set_ddrconfig(const struct rk3399_sdram_params *sdram_params,
			  unsigned char channel, u32 ddrconfig)
{
	/* only need to set ddrconfig */
	struct rk3399_msch_regs *ddr_msch_regs = rk3399_msch[channel];
	unsigned int cs0_cap = 0;
	unsigned int cs1_cap = 0;

	cs0_cap = (1 << (sdram_params->ch[channel].cs0_row
			+ sdram_params->ch[channel].col
			+ sdram_params->ch[channel].bk
			+ sdram_params->ch[channel].bw - 20));
	if (sdram_params->ch[channel].rank > 1)
		cs1_cap = cs0_cap >> (sdram_params->ch[channel].cs0_row
				- sdram_params->ch[channel].cs1_row);
	if (sdram_params->ch[channel].row_3_4) {
		cs0_cap = cs0_cap * 3 / 4;
		cs1_cap = cs1_cap * 3 / 4;
	}

	write32(&ddr_msch_regs->ddrconf, ddrconfig | (ddrconfig << 6));
	write32(&ddr_msch_regs->ddrsize, ((cs0_cap / 32) & 0xff) |
					 (((cs1_cap / 32) & 0xff) << 8));
}

static void dram_all_config(const struct rk3399_sdram_params *sdram_params)
{
	u32 sys_reg = 0;
	unsigned int channel;
	unsigned int use;
	struct rk3399_msch_regs *ddr_msch_regs;
	const struct rk3399_msch_timings *noc_timing;

	sys_reg |= SYS_REG_ENC_DDRTYPE(sdram_params->dramtype);
	sys_reg |= SYS_REG_ENC_NUM_CH(sdram_params->num_channels);
	for (channel = 0, use = 0;
	     (use < sdram_params->num_channels) && (channel < 2); channel++) {
		struct rk3399_ddr_pctl_regs *ddr_pctl_regs =
		    rk3399_ddr_pctl[channel];
		const struct rk3399_sdram_channel *info =
			&sdram_params->ch[channel];
		ddr_msch_regs = rk3399_msch[channel];

		if (sdram_params->ch[channel].col == 0)
			continue;
		use++;
		sys_reg |= SYS_REG_ENC_ROW_3_4(info->row_3_4, channel);
		sys_reg |= SYS_REG_ENC_CHINFO(channel);
		sys_reg |= SYS_REG_ENC_RANK(info->rank, channel);
		sys_reg |= SYS_REG_ENC_COL(info->col, channel);
		sys_reg |= SYS_REG_ENC_BK(info->bk, channel);
		sys_reg |= SYS_REG_ENC_CS0_ROW(info->cs0_row, channel);
		if (sdram_params->ch[channel].rank > 1)
			sys_reg |= SYS_REG_ENC_CS1_ROW(info->cs1_row, channel);
		sys_reg |= SYS_REG_ENC_BW(info->bw, channel);
		sys_reg |= SYS_REG_ENC_DBW(info->dbw, channel);

		noc_timing = &sdram_params->ch[channel].noc_timings;
		write32(&ddr_msch_regs->ddrtiminga0.d32,
			noc_timing->ddrtiminga0.d32);
		write32(&ddr_msch_regs->ddrtimingb0.d32,
			noc_timing->ddrtimingb0.d32);
		write32(&ddr_msch_regs->ddrtimingc0.d32,
			noc_timing->ddrtimingc0.d32);
		write32(&ddr_msch_regs->devtodev0.d32,
			noc_timing->devtodev0.d32);
		write32(&ddr_msch_regs->ddrmode.d32,
			noc_timing->ddrmode.d32);

		/* rank 1 memory clock disable (dfi_dram_clk_disable = 1) */
		if (sdram_params->ch[channel].rank == 1)
			setbits_le32(&ddr_pctl_regs->denali_ctl[276], 1 << 17);
	}

	write32(&rk3399_pmugrf->os_reg2, sys_reg);
	DDR_STRIDE(sdram_params->stride);

	/* reboot hold register set */
	write32(&pmucru_ptr->pmucru_rstnhold_con[1],
		PRESET_SGRF_HOLD(0) | PRESET_GPIO0_HOLD(1) |
		PRESET_GPIO1_HOLD(1));
	clrsetbits_le32(&cru_ptr->glb_rst_con, 0x3, 0x3);
}

void sdram_init(const struct rk3399_sdram_params *sdram_params)
{
	int channel;

	printk(BIOS_INFO, "Starting SDRAM initialization...\n");

	if ((sdram_params->dramtype == DDR3
			&& sdram_params->ddr_freq > 800*MHz) ||
	    (sdram_params->dramtype == LPDDR3
			&& sdram_params->ddr_freq > 928*MHz) ||
	    (sdram_params->dramtype == LPDDR4
			&& sdram_params->ddr_freq > 800*MHz))
		die("SDRAM frequency is to high!");

	rkclk_configure_ddr(sdram_params->ddr_freq);

	for (channel = 0; channel < 2; channel++) {
		struct rk3399_ddr_publ_regs *ddr_publ_regs =
			rk3399_ddr_publ[channel];

		phy_dll_bypass_set(channel, ddr_publ_regs,
				   sdram_params->ddr_freq);

		if (channel >= sdram_params->num_channels)
			continue;

		pctl_cfg(channel, sdram_params);

		/* LPDDR2/LPDDR3 need to wait DAI complete, max 10us */
		if (sdram_params->dramtype == LPDDR3)
			udelay(10);

		if (data_training(channel, sdram_params, PI_FULL_TARINING))
			die("SDRAM initialization failed!");

		set_ddrconfig(sdram_params, channel,
			      sdram_params->ch[channel].ddrconfig);
		ddr_move_to_access_state(channel);
	}
	dram_all_config(sdram_params);

	printk(BIOS_INFO, "Finish SDRAM initialization...\n");
}

size_t sdram_size_mb(void)
{
	return CONFIG_DRAM_SIZE_MB;
}
