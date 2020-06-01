/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <delay.h>
#include <soc/emi.h>
#include <soc/dramc_pi_api.h>
#include <soc/dramc_param.h>
#include <soc/dramc_register.h>
#include <soc/infracfg.h>
#include <string.h>
#include <timer.h>

void dramc_cke_fix_onoff(enum cke_type option, u8 chn)
{
	u8 on = 0, off = 0;

	/* if CKE is dynamic, set both CKE fix On and Off as 0 */
	if (option != CKE_DYNAMIC) {
		on = option;
		off = 1 - option;
	}

	SET32_BITFIELDS(&ch[chn].ao.ckectrl,
			CKECTRL_CKEFIXON, on,
			CKECTRL_CKEFIXOFF, off);
}

static void dvfs_settings(u8 freq_group)
{
	u8 dll_idle;

	switch (freq_group) {
	case LP4X_DDR1600:
		dll_idle = 0x18;
		break;
	case LP4X_DDR2400:
		dll_idle = 0x10;
		break;
	case LP4X_DDR3200:
		dll_idle = 0xc;
		break;
	case LP4X_DDR3600:
		dll_idle = 0xa;
		break;
	default:
		die("Invalid DDR frequency group %u\n", freq_group);
		return;
	}

	dll_idle = dll_idle << 1;
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		setbits32(&ch[chn].ao.dvfsdll, 0x1 << 5);
		setbits32(&ch[chn].phy.dvfs_emi_clk, 0x1 << 29);
		clrsetbits32(&ch[chn].ao.shuctrl2, 0x7f, dll_idle);

		setbits32(&ch[chn].phy.misc_ctrl0, 0x3 << 19);
		setbits32(&ch[chn].phy.dvfs_emi_clk, 0x1 << 24);
		setbits32(&ch[chn].ao.dvfsdll, 0x1 << 7);
	}
}

static void ddr_phy_pll_setting(u8 chn, u8 freq_group)
{
	u8 cap_sel, mid_cap_sel;
	u8 vth_sel = 0x2;
	u8 ca_dll_mode[2];
	u32 sdm_pcw, delta;

	switch (freq_group) {
	case LP4X_DDR1600:
		mid_cap_sel = 0x0;
		cap_sel = 0x3;
		sdm_pcw = 0x7b00;
		delta = 0;
		break;
	case LP4X_DDR2400:
		mid_cap_sel = 0x3;
		cap_sel = 0x0;
		sdm_pcw = 0x5c00;
		delta = 0;
		break;
	case LP4X_DDR3200:
		mid_cap_sel = 0x2;
		cap_sel = 0x0;
		sdm_pcw = 0x7b00;
		delta = 0xc03;
		break;
	case LP4X_DDR3600:
		mid_cap_sel = 0x1;
		cap_sel = 0x0;
		sdm_pcw = 0x8a00;
		delta = 0xd96;
		break;
	default:
		die("Invalid DDR frequency group %u\n", freq_group);
		return;
	}

	if (freq_group == LP4X_DDR1600)
		ca_dll_mode[CHANNEL_A] = DLL_SLAVE;
	else
		ca_dll_mode[CHANNEL_A] = DLL_MASTER;
	ca_dll_mode[CHANNEL_B] = DLL_SLAVE;

	clrbits32(&ch[chn].phy.shu[0].pll[4], 0xffff);
	clrbits32(&ch[chn].phy.shu[0].pll[6], 0xffff);
	setbits32(&ch[chn].phy.misc_shu_opt, (chn + 1) << 18);
	clrsetbits32(&ch[chn].phy.ckmux_sel, 0x3 << 18 | 0x3 << 16, 0x0);
	clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[0], 0x3 << 18, 0x1 << 18);

	SET32_BITFIELDS(&ch[chn].ao.dvfsdll, DVFSDLL_R_BYPASS_1ST_DLL_SHU1,
			ca_dll_mode[chn] == DLL_SLAVE);

	bool is_master = (ca_dll_mode[chn] == DLL_MASTER);
	u8 phdet_out = is_master ? 0x0 : 0x1;
	u8 phdet_in = is_master ? 0x0 : 0x1;
	u8 gain = is_master ? 0x6 : 0x7;
	u8 idle_cnt = is_master ? 0x9 : 0x7;
	u8 fast_psjp = is_master ? 0x1 : 0x0;

	clrsetbits32(&ch[chn].phy.shu[0].ca_dll[0],
		(0x1 << 31) | (0x1 << 30) | (0xf << 20) | (0xf << 16) |
		(0xf << 12) | (0x1 << 10) | (0x1 << 9) | (0x1 << 4),
		(phdet_out << 31) | (phdet_in << 30) |
		(gain << 20) | (idle_cnt << 16) |
		(0x8 << 12) |
		(0x1 << 10) | (0x1 << 9) | (fast_psjp << 4));

	u8 pd_ck_sel = is_master ? 0x1 : 0x0;
	u8 fastpj_ck_sel = is_master ? 0x0 : 0x1;

	clrsetbits32(&ch[chn].phy.shu[0].ca_dll[1],
		(0x1 << 2) | (0x1 << 0),
		(pd_ck_sel << 2) | (fastpj_ck_sel << 0));

	clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[6],
		0x1 << 7, (is_master ? 0x1 : 0x0) << 7);

	struct reg_value regs_bak[] = {
		{&ch[chn].phy.b[0].dq[7]},
		{&ch[chn].phy.b[1].dq[7]},
		{&ch[chn].phy.ca_cmd[7]},
	};

	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		regs_bak[i].value = read32(regs_bak[i].addr);

	for (size_t b = 0; b < 2; b++)
		setbits32(&ch[chn].phy.b[b].dq[7],
			0x1 << 6 | 0x1 << 4 | 0x1 << 2 | 0x1 << 0);
	setbits32(&ch[chn].phy.ca_cmd[7], 0x1 << 6 | 0x1 << 4 | 0x1 << 2 | 0x1 << 0);
	setbits32(&ch[chn].phy.ca_cmd[2], 0x1 << 21);

	/* 26M */
	SET32_BITFIELDS(&ch[chn].phy.misc_cg_ctrl0, MISC_CG_CTRL0_CLK_MEM_SEL, 0);

	/* MID FINE_TUNE */
	clrbits32(&ch[chn].phy.shu[0].b[0].dq[6], (0x1 << 26) | (0x1 << 27));
	clrbits32(&ch[chn].phy.shu[0].b[1].dq[6], (0x1 << 26) | (0x1 << 27));
	clrbits32(&ch[chn].phy.shu[0].ca_cmd[6], (0x1 << 26) | (0x1 << 27));
	clrbits32(&ch[chn].phy.pll4, (0x1 << 16) | (0x1 << 22));

	/* PLL */
	clrbits32(&ch[chn].phy.pll1, 0x1 << 31);
	clrbits32(&ch[chn].phy.pll2, 0x1 << 31);

	/* DLL */
	clrbits32(&ch[chn].phy.ca_dll_fine_tune[2], 0x1 << 0);
	clrbits32(&ch[chn].phy.b[0].dll_fine_tune[2], 0x1 << 0);
	clrbits32(&ch[chn].phy.b[1].dll_fine_tune[2], 0x1 << 0);
	setbits32(&ch[chn].phy.b[0].dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 11) | (0x1 << 13) | (0x1 << 14) |
		(0x1 << 15) | (0x1 << 17) | (0x1 << 19) | (0x1 << 27) | (0x1 << 31));
	setbits32(&ch[chn].phy.b[1].dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 11) | (0x1 << 13) | (0x1 << 14) |
		(0x1 << 15) | (0x1 << 17) | (0x1 << 19) | (0x1 << 27) | (0x1 << 31));
	setbits32(&ch[chn].phy.ca_dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 11) | (0x1 << 13) | (0x1 << 15) |
		(0x1 << 16) | (0x1 << 17) | (0x1 << 19) | (0x1 << 27) | (0x1 << 31));

	/* RESETB */
	clrbits32(&ch[chn].phy.ca_dll_fine_tune[0], 0x1 << 3);
	clrbits32(&ch[chn].phy.b[0].dll_fine_tune[0], 0x1 << 3);
	clrbits32(&ch[chn].phy.b[1].dll_fine_tune[0], 0x1 << 3);

	udelay(1);

	/* MPLL 52M */
	clrsetbits32(&ch[chn].phy.shu[0].pll[8],
		(0x7 << 0) | (0x3 << 18), (0x0 << 0) | (0x1 << 18));
	clrsetbits32(&ch[chn].phy.shu[0].pll[10],
		(0x7 << 0) | (0x3 << 18), (0x0 << 0) | (0x1 << 18));
	clrsetbits32(&ch[chn].phy.shu[0].pll[5],
		(0xffff << 16) | 0x1 << 0, sdm_pcw << 16);
	clrsetbits32(&ch[chn].phy.shu[0].pll[7],
		(0xffff << 16) | 0x1 << 0, sdm_pcw << 16);

	setbits32(&ch[chn].phy.ca_dll_fine_tune[0], 0x1 << 1);
	setbits32(&ch[chn].phy.b[0].dll_fine_tune[0], 0x1 << 1);
	setbits32(&ch[chn].phy.b[1].dll_fine_tune[0], 0x1 << 1);

	clrbits32(&ch[chn].phy.ca_dll_fine_tune[1], 0x1 << 11);
	clrbits32(&ch[chn].phy.b[0].dll_fine_tune[1], 0x1 << 19);
	clrbits32(&ch[chn].phy.b[1].dll_fine_tune[1], 0x1 << 19);

	clrsetbits32(&ch[chn].phy.shu[0].b[0].dq[6],
		(0x3 << 22) | (0x3 << 24) | (0x3 << 28),
		(mid_cap_sel << 22) | (vth_sel << 24) | (cap_sel << 28));
	clrsetbits32(&ch[chn].phy.shu[0].b[1].dq[6],
		(0x3 << 22) | (0x3 << 24) | (0x3 << 28),
		(mid_cap_sel << 22) | (vth_sel << 24) | (cap_sel << 28));
	clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[6],
		(0x3 << 22) | (0x3 << 24) | (0x3 << 28),
		(mid_cap_sel << 22) | (vth_sel << 24) | (cap_sel << 28));

	/* RESETB */
	setbits32(&ch[chn].phy.ca_dll_fine_tune[0], 0x1 << 3);
	setbits32(&ch[chn].phy.b[0].dll_fine_tune[0], 0x1 << 3);
	setbits32(&ch[chn].phy.b[1].dll_fine_tune[0], 0x1 << 3);
	udelay(1);

	/* PLL EN */
	setbits32(&ch[chn].phy.pll1, 0x1 << 31);
	setbits32(&ch[chn].phy.pll2, 0x1 << 31);
	udelay(100);

	/* MIDPI Init 1 */
	setbits32(&ch[chn].phy.pll4, (0x1 << 16) | (0x1 << 22));
	udelay(1);

	/* MIDPI Init 2 */
	u8 midpi_en;
	u8 midpi_ckdiv4_en;

	if (freq_group > LP4X_DDR1600) {
		midpi_en = 0x1;
		midpi_ckdiv4_en = 0x0;
	} else {
		midpi_en = 0x0;
		midpi_ckdiv4_en = 0x1;
	}

	u32 dq6_clear = (0x1 << 26) | (0x1 << 27);
	u32 dq6_set = (midpi_en << 26) | (midpi_ckdiv4_en << 27);

	clrsetbits32(&ch[chn].phy.shu[0].b[0].dq[6], dq6_clear, dq6_set);
	clrsetbits32(&ch[chn].phy.shu[0].b[1].dq[6], dq6_clear, dq6_set);
	clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[6], dq6_clear, dq6_set);

	udelay(1);
	clrsetbits32(&ch[chn].phy.ca_dll_fine_tune[3], 0x1 << 19,
		(0x1 << 13) | (0x1 << 15) | (0x1 << 16) | (0x1 << 17) |
		((chn ? 0 : 1) << 19));
	setbits32(&ch[chn].phy.b[0].dll_fine_tune[3],
		(0x1 << 11) | (0x1 << 13) | (0x1 << 14) | (0x1 << 15) |
		(0x1 << 17));
	setbits32(&ch[chn].phy.b[1].dll_fine_tune[3],
		(0x1 << 11) | (0x1 << 13) | (0x1 << 14) | (0x1 << 15) |
		(0x1 << 17));

	clrbits32(&ch[chn].phy.ca_dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 13) |
		(0x1 << 15) | (0x1 << 16) | (0x1 << 17) |
		(0x1 << 19) | (0x1 << 27) | (0x1 << 31));
	clrbits32(&ch[chn].phy.b[0].dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 13) | (0x1 << 14) |
		(0x1 << 15)  | (0x1 << 17) |
		(0x1 << 19) | (0x1 << 27) | (0x1 << 31));
	clrbits32(&ch[chn].phy.b[1].dll_fine_tune[2],
		(0x1 << 10) | (0x1 << 13) |
		(0x1 << 14) | (0x1 << 15) | (0x1 << 17) |
		(0x1 << 19) | (0x1 << 27) | (0x1 << 31));

	setbits32(&ch[chn].phy.ca_dll_fine_tune[2], 0x1 << 11);
	clrbits32(&ch[chn].phy.b[0].dll_fine_tune[2], 0x1 << 11);
	clrbits32(&ch[chn].phy.b[1].dll_fine_tune[2], 0x1 << 11);
	udelay(2);

	setbits32(&ch[chn].phy.misc_cg_ctrl0, 0x1 << 4);
	udelay(1);

	/* DLL */
	setbits32(&ch[chn].phy.ca_dll_fine_tune[2], 0x1 << 0);
	udelay(1);
	setbits32(&ch[chn].phy.b[0].dll_fine_tune[2], 0x1 << 0);
	setbits32(&ch[chn].phy.b[1].dll_fine_tune[2], 0x1 << 0);
	udelay(1);

	clrbits32(&ch[chn].phy.ca_cmd[2], 0x1 << 21);
	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		write32(regs_bak[i].addr, regs_bak[i].value);

	dramc_cke_fix_onoff(CKE_DYNAMIC, CHANNEL_A);
	dramc_cke_fix_onoff(CKE_DYNAMIC, CHANNEL_B);

	if (freq_group == LP4X_DDR3200 || freq_group == LP4X_DDR3600) {
		setbits32(&ch[chn].phy.shu[0].pll[5], 0x1 << 0);
		setbits32(&ch[chn].phy.shu[0].pll[7], 0x1 << 0);
		setbits32(&ch[chn].phy.shu[0].pll[14], 0x1 << 1);
		setbits32(&ch[chn].phy.shu[0].pll20, 0x1 << 1);
		clrsetbits32(&ch[chn].phy.shu[0].pll[14],
			     0xffff << 16, 0x0208 << 16);
		clrsetbits32(&ch[chn].phy.shu[0].pll20,
			     0xffff << 16, 0x0208 << 16);
		clrsetbits32(&ch[chn].phy.shu[0].pll[15],
			     0xffffffff << 0, delta << 16);
		clrsetbits32(&ch[chn].phy.shu[0].pll21,
			     0xffffffff << 0, delta << 16);
	}
}

static void dramc_gating_mode(u8 mode)
{
	u8 vref_sel = 0, burst = 0;

	if (mode) {
		vref_sel = 2;
		burst = 1;
	}

	for (u8 b = 0; b < 2; b++) {
		clrsetbits32(&ch[0].phy.b[b].dq[6], 0x3 << 14, vref_sel << 14);
		setbits32(&ch[0].phy.b[b].dq[9], 0x1 << 5);
	}

	clrsetbits32(&ch[0].ao.stbcal1, 0x1 << 5, burst << 5);
	setbits32(&ch[0].ao.stbcal, 0x1 << 30);

	for (u8 b = 0; b < 2; b++) {
		clrbits32(&ch[0].phy.b[b].dq[9], (0x1 << 4) | (0x1 << 0));
		udelay(1);
		setbits32(&ch[0].phy.b[b].dq[9], (0x1 << 4) | (0x1 << 0));
	}
}

static void update_initial_settings(u8 freq_group)
{
	u8 operate_fsp = get_freq_fsq(freq_group);
	u16 rx_vref = 0x16;

	if (operate_fsp == FSP_1)
		rx_vref = 0xb;

	if (operate_fsp == FSP_1) {
		setbits32(&ch[0].ao.shu[0].odtctrl, 0x1 << 0);
		setbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
		setbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);
	} else {
		clrbits32(&ch[0].ao.shu[0].odtctrl, 0x1 << 0);
		clrbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
		clrbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);
	}

	for (size_t b = 0; b < 2; b++)
		for (size_t r = 0; r < 2; r++)
			clrbits32(&ch[0].phy.r[r].b[b].rxdvs[2],
				 (0x1 << 23) | (0x1 << 28) | (0x3 << 30));
	clrbits32(&ch[0].phy.shu[0].ca_cmd[7], 0xf << 0);

	setbits32(&ch[0].phy.ca_cmd[3], 0x1 << 10);
	setbits32(&ch[0].phy.ca_cmd[10], 0x1 << 5);
	clrsetbits32(&ch[0].phy.ca_cmd[6], 0x3 << 14, 0x1 << 14);
	setbits32(&ch[0].phy.b[0].dq[3], 0x7 << 5);
	setbits32(&ch[0].phy.b[1].dq[3], 0x7 << 5);
	setbits32(&ch[0].phy.ca_cmd[3], (0x1 << 5) | (0x1 << 7));
	clrbits32(&ch[0].phy.b[0].dq[3], 0x1 << 1);
	clrbits32(&ch[0].phy.b[1].dq[3], 0x1 << 1);
	setbits32(&ch[0].phy.b[0].dq[5], 0x1 << 31);
	setbits32(&ch[0].phy.b[1].dq[5], 0x1 << 31);
	setbits32(&ch[0].phy.ca_cmd[5], 0x1 << 31);

	clrsetbits32(&ch[0].phy.ca_cmd[6], 0xf << 16, 0x3 << 16);
	clrsetbits32(&ch[0].phy.misc_imp_ctrl0, (0x1 << 5) | (0x1 << 6),
		(0x1 << 5) | (0x0 << 6));
	setbits32(&ch[0].phy.b[0].dq[6], 0x1 << 9);
	setbits32(&ch[0].phy.b[1].dq[6], 0x1 << 9);
	setbits32(&ch[0].phy.ca_cmd[6], 0x1 << 9);
	clrsetbits32(&ch[0].phy.b[0].dq[6], 0x3 << 0, 0x1 << 0);
	clrsetbits32(&ch[0].phy.b[1].dq[6], 0x1 << 0, 0x1 << 0);
	clrsetbits32(&ch[0].phy.ca_cmd[6], 0x3 << 0, 0x1 << 0);

	setbits32(&ch[0].phy.ca_cmd[6], 0x1 << 6);
	setbits32(&ch[0].phy.b[0].dq[6], 0x1 << 3);
	setbits32(&ch[0].phy.b[1].dq[6], 0x1 << 3);
	setbits32(&ch[0].phy.ca_cmd[6], 0x1 << 3);
	setbits32(&ch[0].phy.b[0].dq[6], 0x1 << 5);
	setbits32(&ch[0].phy.b[1].dq[6], 0x1 << 5);
	setbits32(&ch[0].phy.ca_cmd[6], 0x1 << 5);

	for (u8 b = 0; b < 2; b++) {
		clrsetbits32(&ch[0].phy.shu[0].b[b].dq[5], 0x3f << 0, rx_vref << 0);
		clrsetbits32(&ch[0].phy.b[b].dq[5], 0x3f << 8, rx_vref << 8);
	}

	setbits32(&ch[0].phy.b[0].dq[8], (0x1 << 0) | (0x1 << 1) | (0x1 << 2));
	setbits32(&ch[0].phy.b[1].dq[8], (0x1 << 0) | (0x1 << 1) | (0x1 << 2));
	setbits32(&ch[0].phy.ca_cmd[9], (0x1 << 0) | (0x1 << 1) | (0x1 << 2));
	dramc_gating_mode(1);

	setbits32(&ch[0].phy.ca_cmd[8], 0x1 << 19);
	clrbits32(&ch[0].phy.ca_cmd[8], 0x1 << 18);
	clrsetbits32(&ch[0].ao.shu[0].misc, 0xf << 0, 0x2 << 0);
	clrsetbits32(&ch[0].ao.shu[0].dqsg, (0x3f << 20) | (0x1 << 16),
		(0x2a << 20) | (0x1 << 16));

	clrbits32(&ch[0].phy.shu[0].b[0].dq[5], 0x3f << 8);
	clrbits32(&ch[0].phy.shu[0].b[1].dq[5], 0x3f << 8);
	clrbits32(&ch[0].phy.shu[0].ca_cmd[5], 0x3f << 8);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		clrbits32(&ch[chn].phy.shu[0].b[0].dq[6], 0x3f << 0);
		clrbits32(&ch[chn].phy.shu[0].b[1].dq[6], 0x3f << 0);
		clrbits32(&ch[chn].phy.shu[0].ca_cmd[6], 0x3f << 0);
	}
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	/* IMP Tracking Init Settings */
	clrsetbits32(&ch[0].ao.shu[0].impcal1,
		(0x7 << 0) | (0x7 << 17) | (0xff << 20) | (0xf << 28),
		(0x4 << 0) | (0x4 << 17) | (0x10 << 20) | (0x8 << 28));

	setbits32(&ch[0].ao.srefctrl, 0xf << 12);
	setbits32(&ch[0].ao.pre_tdqsck[0], 0x1 << 17);
	setbits32(&ch[0].ao.shu[0].misc, 0xf << 12);
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[8],
		(0xffff << 0) | (0x1 << 15) | (0x3ff << 22),
		(0x7fff << 0) | (0x0 << 15) | (0x3ff << 22));
	clrsetbits32(&ch[0].phy.shu[0].b[1].dq[8],
		(0xffff << 0) | (0x1 << 15) | (0x3ff << 22),
		(0x7fff << 0) | (0x0 << 15) | (0x3ff << 22));
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[8],
		(0xffff << 0) | (0x1 << 15) | (0x3ff << 22),
		(0x7fff << 0) | (0x0 << 15) | (0x3ff << 22));
	setbits32(&ch[0].phy.misc_ctrl3, 0x1 << 26);
	clrbits32(&ch[0].phy.shu[0].b[0].dq[7], (0xf << 8) | (0x1 << 12) | (0x1 << 13));
	clrbits32(&ch[0].phy.shu[0].b[1].dq[7], (0xf << 8) | (0x1 << 12) | (0x1 << 13));
	clrsetbits32(&ch[0].ao.clkar, (0xffff << 0) | (0x1 << 15),
		(0x7fff << 0) | (0x1 << 15));

	clrbits32(&ch[0].ao.shu[0].dqsg_retry, 0x1 << 29);
	clrbits32(&ch[0].ao.write_lev, 0x1 << 2);
	setbits32(&ch[0].ao.dummy_rd, 0x1 << 24);
	clrbits32(&ch[0].ao.stbcal2, (0x1 << 0) | (0x1 << 1));
	setbits32(&ch[0].ao.eyescan, (0x1 << 8) | (0x1 << 9) | (0x1 << 10));
	setbits32(&ch[0].ao.shu[0].odtctrl, (0x1 << 2) | (0x1 << 3));

	setbits32(&ch[0].phy.shu[0].b[0].dll[0], 0x1 << 0);
	setbits32(&ch[0].phy.shu[0].b[1].dll[0], 0x1 << 0);
	setbits32(&ch[0].phy.ca_dll_fine_tune[1], 0x1 << 21);

	setbits32(&ch[0].ao.perfctl0, (0x1 << 15) | (0x1 << 19) | (0x1 << 26));
	setbits32(&ch[0].ao.srefctrl, 0x1 << 22);
	clrsetbits32(&ch[0].ao.shuctrl1, 0xff << 0, 0x1a << 0);
	setbits32(&ch[0].phy.b[0].dq[6], (0x1 << 7) | (0x1 << 12));
	setbits32(&ch[0].phy.b[1].dq[6], (0x1 << 7) | (0x1 << 12));
	setbits32(&ch[0].phy.ca_cmd[6], (0x1 << 7) | (0x1 << 12));
	setbits32(&ch[0].ao.stbcal2, 0x1 << 16);
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[7],
		(0x7 << 29) | (0x1 << 28) | (0x7 << 25) | (0x1 << 24),
		(0x0 << 29) | (0x1 << 28) | (0x1 << 25) | (0x1 << 24));
	clrsetbits32(&ch[0].phy.shu[0].b[1].dq[7],
		(0x7 << 29) | (0x1 << 28) | (0x7 << 25) | (0x1 << 24),
		(0x0 << 29) | (0x1 << 28) | (0x1 << 25) | (0x1 << 24));

	/* Disable RODT tracking */
	clrbits32(&ch[0].ao.shu[0].rodtenstb, 0x1 << 0);

	/* Rx Gating tracking settings */
	clrsetbits32(&ch[0].ao.shu[0].dqsg,
		(0x1 << 11) | (0xf << 12), (0x1 << 11) | (0x9 << 12));
	clrbits32(&ch[0].ao.shu[0].rk[0].dqscal, (0x1 << 7) | (0x1 << 15));
	clrbits32(&ch[0].ao.shu[0].rk[1].dqscal, (0x1 << 7) | (0x1 << 15));
	clrsetbits32(&ch[0].ao.shu[0].stbcal,
		(0x7 << 4) | (0x1 << 8), (0x1 << 4) | (0x1 << 8));

	clrsetbits32(&ch[0].phy.b[0].dq[9], 0xff << 8, 0x4 << 8);
	clrsetbits32(&ch[0].phy.b[1].dq[9], 0xff << 8, 0x4 << 8);
	clrbits32(&ch[0].phy.ca_cmd[10], 0xff << 8);

	setbits32(&ch[0].phy.shu[0].b[0].dq[8], 0x1 << 24);
	setbits32(&ch[0].phy.shu[0].b[1].dq[8], 0x1 << 24);

	/* Enable WDQS */
	clrsetbits32(&ch[0].phy.shu[0].b[0].dll[1],
		(0x1 << 10) | (0x1 << 16) | (0x1 << 17),
		(0x1 << 10) | (!operate_fsp << 16) | (0x1 << 17));
	clrsetbits32(&ch[0].phy.shu[0].b[1].dll[1],
		(0x1 << 10) | (0x1 << 16) | (0x1 << 17),
		(0x1 << 10) | (!operate_fsp << 16) | (0x1 << 17));
	setbits32(&ch[0].ao.shu[0].odtctrl, (0x1 << 0) | (0x1 << 30) | (0x1 << 31));
	setbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
	setbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);
	setbits32(&ch[0].ao.drsctrl, 0x1 << 19);
	setbits32(&ch[0].ao.refctrl0, 0x1 << 28);
	setbits32(&ch[0].ao.zqcs, 0x1 << 19);
	setbits32(&ch[0].ao.dummy_rd, 0x3 << 26);
	setbits32(&ch[0].ao.shuctrl2, 0x1 << 8);
	clrsetbits32(&ch[0].ao.shuctrl3, 0xff << 24, 0xb << 24);
	setbits32(&ch[0].phy.misc_ctrl3, 0x1 << 27);
	setbits32(&ch[0].phy.b[0].dll_fine_tune[1], 0x3 << 20);
	setbits32(&ch[0].phy.b[1].dll_fine_tune[1], 0x3 << 20);
	setbits32(&ch[0].phy.ca_dll_fine_tune[1], 0x1 << 20);
	clrbits32(&ch[0].phy.misc_ctrl0, 0x1 << 27);
	setbits32(&ch[0].phy.misc_rxdvs[2], 0x1 << 8);
	setbits32(&ch[0].ao.clkctrl, 0x1 << 7);
	setbits32(&ch[0].ao.refctrl1, 0x1 << 7);
	clrsetbits32(&ch[0].ao.shuctrl, (0x1 << 2) | (0x3 << 6) | (0x3 << 26),
		(0x0 << 2) | (0x3 << 6) | (0x3 << 26));
	setbits32(&ch[0].ao.shuctrl2, (0x1 << 31) | (0x3 << 10));
	clrbits32(&ch[0].ao.stbcal2, 0xf << 4);
	clrbits32(&ch[0].ao.pre_tdqsck[0], 0x3 << 19);

	setbits32(&ch[0].ao.ckectrl, 0x1 << 22);
	clrsetbits32(&ch[0].phy.ca_tx_mck, (0x1 << 31) | (0x1f << 21) | (0x1f << 26),
		(0x1 << 31) | (0xa << 21) | (0xa << 26));
	setbits32(&ch[0].ao.ckectrl, 0x1 << 23);
	clrbits32(&ch[0].ao.shu[0].rodtenstb, 0x1 << 31);

	/* CA prebit shift and delay */
	SET32_BITFIELDS(&ch[0].ao.shu[0].selph_ca7,
		SHU_SELPH_CA7_DLY_RA0, 0x0,
		SHU_SELPH_CA7_DLY_RA1, 0x0,
		SHU_SELPH_CA7_DLY_RA2, 0x0,
		SHU_SELPH_CA7_DLY_RA3, 0x0,
		SHU_SELPH_CA7_DLY_RA4, 0x0,
		SHU_SELPH_CA7_DLY_RA5, 0x0);
	SET32_BITFIELDS(&ch[0].phy.shu[0].rk[0].ca_cmd[9],
		SHU1_R0_CA_CMD9_RG_RK0_ARPI_CMD, 0x20);
	SET32_BITFIELDS(&ch[0].phy.shu[0].rk[1].ca_cmd[9],
		SHU1_R1_CA_CMD9_RG_RK1_ARPI_CMD, 0x20);
}

static void dramc_power_on_sequence(void)
{
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		clrbits32(&ch[chn].phy.misc_ctrl1, 0x1 << 13);

	dramc_cke_fix_onoff(CKE_FIXOFF, CHANNEL_A);
	dramc_cke_fix_onoff(CKE_FIXOFF, CHANNEL_B);

	udelay(200);
	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		setbits32(&ch[chn].phy.misc_ctrl1, 0x1 << 13);

	for (size_t chn = 0; chn < CHANNEL_MAX; chn++)
		setbits32(&ch[chn].ao.dramc_pd_ctrl, 0x1 << 26);

	udelay(2000);
	dramc_cke_fix_onoff(CKE_FIXON, CHANNEL_A);
	dramc_cke_fix_onoff(CKE_FIXON, CHANNEL_B);
	udelay(2);
}

static void ddr_phy_reserved_rg_setting(u8 freq_group)
{
	u32 hyst_sel = 0, midpi_cap_sel = 0, lp3_sel = 0;

	if (get_freq_fsq(freq_group) == FSP_0) {
		hyst_sel = 1;
		midpi_cap_sel = 1;
	}

	if (freq_group == LP4X_DDR1600)
		lp3_sel = 1;

	/* fine tune */
	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[6], 0xffff << 6,
			(0x1 << 6) | ((!chn) << 7) | (hyst_sel << 8) |
			(midpi_cap_sel << 9) | (0x1 << 10) | (0x3 << 17) | (lp3_sel << 20));

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		clrsetbits32(&ch[chn].phy.shu[0].ca_dll[1],
			(0xf << 9) | (0x1f << 16) | (0x7ff << 21),
			(0x1 << 8) | (0x7 << 13) | (0x4 << 16));

		for (u8 b = 0; b < 2; b++) {
			clrsetbits32(&ch[chn].phy.shu[0].b[b].dq[6],
				(0x1f << 6) | (0x3f << 11) | (0x7 << 19),
				(0x1 << 6) | (hyst_sel << 8) | (midpi_cap_sel << 9)
				| (0x1 << 10)  | (0x3 << 17) | (lp3_sel << 20));

			clrsetbits32(&ch[chn].phy.shu[0].b[b].dll[1],
				(0x3 << 8) | (0x3 << 11) | (0x7 << 14) | (0x3fff << 18),
				(0x1 << 10) | (0x1 << 13) | (0x1 << 17));
		}
	}
}

static void dramc_duty_set_clk_delay(u8 chn, s8 clkDelay)
{
	u8 dly, dlyb, revb0, revb1;

	dly = (clkDelay < 0) ? -clkDelay : 0;
	dlyb = (clkDelay < 0) ? 0 : clkDelay;
	revb0 = dly ? 1 : 0;
	revb1 = dlyb ? 1 : 0;

	for (u8 r = 0; r < RANK_MAX; r++) {
		clrsetbits32(&ch[chn].phy.shu[0].rk[r].ca_cmd[1],
			(0xf << 24) | (0xf << 28), (dly << 24) | (dly << 28));
		clrsetbits32(&ch[chn].phy.shu[0].rk[r].ca_cmd[0],
			(0xf << 24) | (0xf << 28), (dlyb << 24) | (dlyb << 28));
	}
	clrsetbits32(&ch[chn].phy.shu[0].ca_cmd[3],
		(0x3 << 8), (revb0 << 8) | (revb1 << 9));
}

static void dramc_duty_set_dqs_delay(u8 chn, const s8 *s_dqsDelay)
{
	u8 dly, dlyb, revb0, revb1;
	s8 dqsDelay;

	for (u8 r = 0; r < RANK_MAX; r++)
		for (u8 dqs = 0; dqs < DQS_NUMBER; dqs++) {
			dqsDelay = s_dqsDelay[dqs];

			dly = (dqsDelay < 0) ? -dqsDelay : 0;
			dlyb = (dqsDelay < 0) ? 0 : dqsDelay;
			revb0 = dly ? 1 : 0;
			revb1 = dlyb ? 1 : 0;
			clrsetbits32(&ch[chn].phy.shu[0].rk[r].b[dqs].dq[1],
				(0xf << 24) | (0xf << 28) | (0xf << 16) | (0xf << 20),
				(dly << 24) | (dly << 28) | (dlyb << 16) | (dlyb << 20));
		}
	clrsetbits32(&ch[chn].phy.shu[0].b[0].dll[1],
		0x3 << 8, (revb0 << 8) | (revb1 << 9));
}

static void dramc_duty_calibration(const struct sdram_params *params, u8 freq_group)
{
	switch (params->source) {
	case DRAMC_PARAM_SOURCE_SDRAM_CONFIG:
		break;
	case DRAMC_PARAM_SOURCE_FLASH:
		dramc_dbg("bypass duty calibration\n");

		for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
			dramc_duty_set_clk_delay(chn, params->duty_clk_delay[chn]);
			dramc_duty_set_dqs_delay(chn, params->duty_dqs_delay[chn]);
		}
		return;
	default:
		die("Invalid DRAM param source %u\n", params->source);
		return;
	}

	s8 clkDelay[CHANNEL_MAX] = {0x0};
	s8 dqsDelay[CHANNEL_MAX][DQS_NUMBER] = {0x0};

	switch (freq_group) {
	case LP4X_DDR1600:
		clkDelay[CHANNEL_A] = 2;
		clkDelay[CHANNEL_B] = 1;
		dqsDelay[CHANNEL_A][0] = 0;
		dqsDelay[CHANNEL_A][1] = 0;
		dqsDelay[CHANNEL_B][0] = -1;
		dqsDelay[CHANNEL_B][1] = 0;
		break;
	case LP4X_DDR2400:
		clkDelay[CHANNEL_A] = clkDelay[CHANNEL_B] = 0;
		dqsDelay[CHANNEL_A][0] = 0;
		dqsDelay[CHANNEL_A][1] = -2;
		dqsDelay[CHANNEL_B][0] = 0;
		dqsDelay[CHANNEL_B][1] = -2;
		break;
	case LP4X_DDR3200:
		clkDelay[CHANNEL_A] = clkDelay[CHANNEL_B] = 1;
		dqsDelay[CHANNEL_A][0] = 1;
		dqsDelay[CHANNEL_A][1] = -2;
		dqsDelay[CHANNEL_B][0] = 1;
		dqsDelay[CHANNEL_B][1] = -2;
		break;
	case LP4X_DDR3600:
		clkDelay[CHANNEL_A] = 2;
		clkDelay[CHANNEL_B] = 1;
		dqsDelay[CHANNEL_A][0] = 0;
		dqsDelay[CHANNEL_A][1] = 0;
		dqsDelay[CHANNEL_B][0] = -1;
		dqsDelay[CHANNEL_B][1] = 0;
		break;
	default:
		die("Invalid DDR frequency group %u\n", freq_group);
		return;
	}

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		dramc_duty_set_clk_delay(chn, clkDelay[chn]);
		dramc_duty_set_dqs_delay(chn, dqsDelay[chn]);
	}
}

static u8 dramc_zq_calibration(u8 chn, u8 rank)
{
	const u32 TIMEOUT_US = 100;

	struct reg_value regs_bak[] = {
		{&ch[chn].ao.mrs},
		{&ch[chn].ao.dramc_pd_ctrl},
		{&ch[chn].ao.ckectrl},
	};

	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		regs_bak[i].value = read32(regs_bak[i].addr);

	setbits32(&ch[chn].ao.dramc_pd_ctrl, 0x1 << 26);
	dramc_cke_fix_onoff(CKE_FIXON, chn);

	SET32_BITFIELDS(&ch[chn].ao.mrs, MRS_MRSRK, rank);
	SET32_BITFIELDS(&ch[chn].ao.mpc_option, MPC_OPTION_MPCRKEN, 1);
	SET32_BITFIELDS(&ch[chn].ao.spcmd, SPCMD_ZQCEN, 1);

	if (!wait_us(TIMEOUT_US, read32(&ch[chn].nao.spcmdresp) & 0x1 << 4)) {
		dramc_dbg("ZQCAL Start fail (time out)\n");
		return 1;
	}

	SET32_BITFIELDS(&ch[chn].ao.spcmd, SPCMD_ZQCEN, 0);

	udelay(1);
	SET32_BITFIELDS(&ch[chn].ao.spcmd, SPCMD_ZQLATEN, 1);

	if (!wait_us(TIMEOUT_US, read32(&ch[chn].nao.spcmdresp) & 0x1 << 6)) {
		dramc_dbg("ZQCAL Latch fail (time out)\n");
		return 1;
	}

	SET32_BITFIELDS(&ch[chn].ao.spcmd, SPCMD_ZQLATEN, 0);
	udelay(1);
	for (size_t i = 0; i < ARRAY_SIZE(regs_bak); i++)
		write32(regs_bak[i].addr, regs_bak[i].value);

	return 0;
}

static void dramc_mode_reg_init(u8 freq_group, struct mr_value *mr)
{
	u8 *MR01Value = mr->MR01Value;
	u8 MR02Value[FSP_MAX] = {0x12, 0x12};
	u8 MR03Value = 0x30;
	u8 MR11Value[FSP_MAX] = {0x0, 0x23};
	u8 MR12Value[CHANNEL_MAX][RANK_MAX][FSP_MAX] = {
		{{0x5d, 0x5d}, {0x5d, 0x5d} }, {{0x5d, 0x5d}, {0x5d, 0x5d} },
	};
	u8 MR13Value;
	u8 MR14Value[CHANNEL_MAX][RANK_MAX][FSP_MAX] = {
		{{0x5d, 0x10}, {0x5d, 0x10} }, {{0x5d, 0x10}, {0x5d, 0x10} },
	};

	u8 MR22Value[FSP_MAX] = {0x38, 0x34};

	MR01Value[FSP_0] = 0x6;
	MR01Value[FSP_1] = 0x6;

	if (freq_group == LP4X_DDR1600) {
		MR02Value[0] = 0x12;
		MR02Value[1] = 0x00;

		MR01Value[FSP_0] |= (0x5 << 4);
		MR01Value[FSP_1] |= (0x5 << 4);
	} else if (freq_group == LP4X_DDR2400) {
		MR02Value[0] = 0x24;
		MR02Value[1] = 0x2d;

		MR01Value[FSP_0] |= (0x5 << 4);
		MR01Value[FSP_1] |= (0x5 << 4);
	} else if (freq_group == LP4X_DDR3200) {
		MR02Value[0] = 0x12;
		MR02Value[1] = 0x2d;

		MR01Value[FSP_0] |= (0x5 << 4);
		MR01Value[FSP_1] |= (0x5 << 4);
	} else if (freq_group == LP4X_DDR3600) {
		MR02Value[0] = 0x1a;
		MR02Value[1] = 0x36;

		MR01Value[FSP_0] |= (0x6 << 4);
		MR01Value[FSP_1] |= (0x6 << 4);
	}

	u8 operate_fsp = get_freq_fsq(freq_group);
	dramc_dbg("%s operate_fsp:%d, freq:%d\n", __func__, operate_fsp, freq_group);

	u8 chn, rank;
	u32 broadcast_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	dramc_power_on_sequence();

	for (chn = 0; chn < CHANNEL_MAX; chn++) {
		for (rank = 0; rank < 2; rank++) {
			clrsetbits32(&ch[chn].ao.mrs, 0x3 << 24, rank << 24);

			dramc_zq_calibration(chn, rank);

			for (uint32_t fsp = FSP_0; fsp < FSP_MAX; fsp++) {
				dramc_dbg("chn:%d,rank:%d,fsp%d\n", chn, rank, fsp);

				if (fsp == FSP_0)
					MR13Value = (1 << 4) | (1 << 3);
				else
					MR13Value |= 0x40;
				dramc_mode_reg_write(chn, 0xd, MR13Value);
				dramc_mode_reg_write(chn, 0xc,
					MR12Value[chn][rank][fsp]);
				dramc_mode_reg_write(chn, 0x1, MR01Value[fsp]);
				dramc_mode_reg_write(chn, 0x2, MR02Value[fsp]);
				dramc_mode_reg_write(chn, 0xb, MR11Value[fsp]);

				dramc_mode_reg_write(chn, 0x16, MR22Value[fsp]);
				dramc_mode_reg_write(chn, 0xe,
					MR14Value[chn][rank][fsp]);

				/* MR3 set write-DBI and read-DBI */
				dramc_mode_reg_write(chn, 0x3, MR03Value);
			}

			if (operate_fsp == FSP_0)
				MR13Value &= 0x3f;
			else
				MR13Value |= 0xc0;
			dramc_mode_reg_write(chn, 0xd, MR13Value);
		}

		clrsetbits32(&ch[chn].ao.shu[0].hwset_mr13,
			(0x1fff << 0) | (0xff << 16),
			(13 << 0) | ((MR13Value | (0x1 << 3)) << 16));
		clrsetbits32(&ch[chn].ao.shu[0].hwset_vrcg,
			(0x1fff << 0) | (0xff << 16),
			(13 << 0) | ((MR13Value | (0x1 << 3)) << 16));
		clrsetbits32(&ch[chn].ao.shu[0].hwset_mr2,
			(0x1fff << 0) | (0xff << 16),
			(2 << 0) | (MR02Value[operate_fsp] << 16));
	}

	mr->MR13Value = MR13Value;

	clrsetbits32(&ch[0].ao.mrs, 0x3 << 24, RANK_0 << 24);
	clrsetbits32(&ch[1].ao.mrs, 0x3 << 24, RANK_0 << 24);
	dramc_set_broadcast(broadcast_bak);
}

static void auto_refresh_cke_off(void)
{
	u32 broadcast_bak = dramc_get_broadcast();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++)
		setbits32(&ch[chn].ao.refctrl0, 0x1 << 29);

	udelay(3);
	dramc_cke_fix_onoff(CKE_FIXOFF, CHANNEL_A);
	dramc_cke_fix_onoff(CKE_FIXOFF, CHANNEL_B);

	dramc_set_broadcast(broadcast_bak);
}

static void dramc_setting_DDR1600(void)
{
	clrsetbits32(&ch[0].ao.shu[0].rankctl,
		(0xf << 20) | (0xf << 24) | (0xf << 28),
		(0x0 << 20) | (0x0 << 24) | (0x2 << 28));
	clrbits32(&ch[0].ao.shu[0].ckectrl, 0x3 << 24);
	clrbits32(&ch[0].ao.shu[0].odtctrl, (0x1 << 0) | (0x3 << 30));

	clrbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
	clrbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);

	clrsetbits32(&ch[0].ao.shu[0].selph_dqs0, 0x77777777, SELPH_DQS0_1600);
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs1, 0x77777777, SELPH_DQS1_1600);
	clrsetbits32(&ch[0].ao.shu[0].wodt, (0x1 << 29) | (0x1 << 31),
		(0x0 << 29) | (0x1 << 31));
	clrsetbits32(&ch[0].ao.shu[0].dqs2dq_tx, 0x1f << 0, 0x4 << 0);

	for (size_t rank = 0; rank < 2; rank++) {
		int value = ((rank == 0) ? 0x1a : 0x1e);
		clrbits32(&ch[0].ao.shu[0].rk[rank].dqsien, (0x7f << 0) | (0x7f << 8));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].fine_tune,
			(0x3f << 0) | (0x3f << 8) | (0x3f << 16) | (0x3f << 24),
			(value << 0) | (value << 8) | (value << 16) | (value << 24));

		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[0],
			(0x7 << 8) | (0x7 << 12) |
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28),
			(0x2 << 8) | (0x2 << 12) |
			(0x1 << 16) | (0x1 << 20) | (0x1 << 24) | (0x1 << 28));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[1],
			(0x7 << 8) | (0x7 << 12) |
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24)	| (0x7 << 28),
			(0x2 << 8) | (0x2 << 12) |
			(0x1 << 16) | (0x1 << 20) | (0x1 << 24) | (0x1 << 28));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[2],
			0x77777777, _SELPH_DQS_BITS(0x1, 0x7));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[3],
			0x77777777, _SELPH_DQS_BITS(0x1, 0x7));
	}

	clrsetbits32(&ch[0].ao.shu[0].dqsg_retry, (0x1 << 2) | (0xf << 8),
		(0x0 << 2) | (0x3 << 8));
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[5], 0x7 << 20, 0x4 << 20);
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[7],
		(0x3 << 4) | (0x1 << 7) | (0x1 << 13),
		(0x2 << 4) | (0x0 << 7) | (0x0 << 13));
	clrsetbits32(&ch[0].phy.shu[0].b[1].dq[5], 0x7 << 20, 0x4 << 20);
	clrbits32(&ch[0].phy.shu[0].b[1].dq[7], (0x1 << 7) | (0x1 << 13));

	for (size_t r = 0; r < 2; r++) {
		int value = ((r == 0) ? 0x1a : 0x26);
		for (size_t b = 0; b < 2; b++)
			clrsetbits32(&ch[0].phy.shu[0].rk[r].b[b].dq[7],
				(0x3f << 8) | (0x3f << 16),
				(value << 8) | (value << 16));
	}
}

static void dramc_setting_DDR2400(void)
{
	clrsetbits32(&ch[0].ao.shu[0].rankctl,
		(0xf << 20) | (0xf << 24) | (0xf << 28),
		(0x2 << 20) | (0x2 << 24) | (0x4 << 28));
	clrsetbits32(&ch[0].ao.shu[0].ckectrl, 0x3 << 24, 0x3 << 24);
	setbits32(&ch[0].ao.shu[0].odtctrl, (0x1 << 0) | (0x1 << 30) | (0x1 << 31));

	setbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
	setbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);

	clrsetbits32(&ch[0].ao.shu[0].selph_dqs0, 0x77777777, SELPH_DQS0_2400);
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs1, 0x77777777, SELPH_DQS1_2400);
	clrsetbits32(&ch[0].ao.shu[0].wodt,
		(0x1 << 29) | (0x1 << 31), (0x1 << 29) | (0x0 << 31));
	clrsetbits32(&ch[0].ao.shu[0].dqs2dq_tx, 0x1f << 0, 0x7 << 0);

	for (size_t rank = 0; rank < 2; rank++) {
		int value = ((rank == 0) ? 0x19 : 0x1f);
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].dqsien,
			(0x7f << 0) | (0x7f << 8), (value << 0) | (value  << 8));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].fine_tune,
			(0x3f << 0) | (0x3f << 8) | (0x3f << 16) | (0x3f << 24),
			(0x14 << 0) | (0x14 << 8) | (0x14 << 16) | (0x14 << 24));

		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[0],
			(0x7 << 8) | (0x7 << 12) |
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28),
			(0x3 << 8) | (0x3 << 12) |
			(0x3 << 16) | (0x3 << 20) | (0x3 << 24) | (0x3 << 28));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[1],
			(0x7 << 8) | (0x7 << 12) |
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28),
			(0x3 << 8) | (0x3 << 12) |
			(0x3 << 16) | (0x3 << 20) | (0x3 << 24) | (0x3 << 28));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[2],
			0x77777777, _SELPH_DQS_BITS(0x2, 0x0));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[3],
			0x77777777, _SELPH_DQS_BITS(0x2, 0x0));
	}

	clrsetbits32(&ch[0].ao.shu[0].dqsg_retry,
		(0x1 << 2) | (0xf << 8), (0x1 << 2) | (0x4 << 8));
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[5], 0x7 << 20, 0x3 << 20);
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[7],
		(0x3 << 4) | (0x1 << 7) | (0x1 << 13),
		(0x1 << 4) | (0x1 << 7) | (0x1 << 13));
	clrsetbits32(&ch[0].phy.shu[0].b[1].dq[5], 0x7 << 20, 0x3 << 20);
	clrsetbits32(&ch[0].phy.shu[0].b[1].dq[7],
		(0x1 << 7) | (0x1 << 13), (0x1 << 7) | (0x1 << 13));

	for (size_t r = 0; r < 2; r++) {
		for (size_t b = 0; b < 2; b++)
			clrsetbits32(&ch[0].phy.shu[0].rk[r].b[b].dq[7],
				(0x3f << 8) | (0x3f << 16), (0x14 << 8) | (0x14 << 16));
	}
}

static void dramc_setting_DDR3600(void)
{
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs0, 0x77777777, SELPH_DQS0_3600);
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs1, 0x77777777, SELPH_DQS1_3600);
}

static void dramc_setting(const struct sdram_params *params, u8 freq_group,
			  const struct dram_impedance *impedance)
{
	u8 chn;

	auto_refresh_cke_off();
	dramc_set_broadcast(DRAMC_BROADCAST_OFF);

	for (chn = 0; chn < CHANNEL_MAX; chn++)
		setbits32(&ch[chn].phy.ckmux_sel, (0x1 << 0) | (0x1 << 1));

	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	setbits32(&ch[0].phy.misc_cg_ctrl0, 0x1 << 0);

	/* 26M */
	clrbits32(&ch[0].phy.misc_cg_ctrl0, 0x3 << 4);
	clrbits32(&ch[0].phy.misc_ctrl0, 0x1 << 17);

	clrbits32(&ch[0].phy.misc_spm_ctrl1, 0xf << 0);
	write32(&ch[0].phy.misc_spm_ctrl2, 0x0);
	write32(&ch[0].phy.misc_spm_ctrl0, 0x0);
	write32(&ch[0].phy.misc_cg_ctrl2, 0x6003bf);
	write32(&ch[0].phy.misc_cg_ctrl4, 0x333f3f00);
	setbits32(&ch[0].phy.shu[0].pll[1], (0x1 << 4) | (0x7 << 1));
	clrsetbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x3f << 0, 0x10 << 0);
	clrbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x0f << 0);

	for (size_t b = 0; b <= 2; b += 2)
		clrsetbits32(&ch[0].phy.shu[0].pll[4 + b],
			 (0x3 << 18) | (0x3 << 24) | (0x3 << 26),
			 (0x2 << 18) | (0x1 << 24) | (0x1 << 26));

	clrbits32(&ch[0].phy.shu[0].pll[14], 0x1 << 1);
	clrbits32(&ch[0].phy.shu[0].pll20, 0x1 << 1);
	clrbits32(&ch[0].phy.ca_cmd[2], (0x3 << 16) | (0x3 << 20));
	for (size_t b = 0; b < 2; b++)
		clrbits32(&ch[0].phy.b[b].dq[2], (0x3 << 16) | (0x3 << 20));
	for (size_t b = 0; b < 2; b++)
		clrsetbits32(&ch[0].phy.b[b].dq[9], 0x7 << 28, 0x1 << 28);
	clrbits32(&ch[0].phy.ca_cmd[10], 0x7 << 28);

	setbits32(&ch[0].phy.b0_rxdvs[0], 0x1 << 28);
	setbits32(&ch[0].phy.b1_rxdvs[0], 0x1 << 28);
	setbits32(&ch[0].phy.b0_rxdvs[0], 0x1 << 9);
	setbits32(&ch[0].phy.b1_rxdvs[0], 0x1 << 9);

	for (size_t b = 0; b < 2; b++) {
		for (size_t r = 0; r < 2; r++)
			setbits32(&ch[0].phy.r[r].b[b].rxdvs[2], 0x1 << 29);
		clrsetbits32(&ch[0].phy.shu[0].b[b].dq[5], 0x7 << 20, 0x3 << 20);

		for (size_t r = 0; r < 2; r++) {
			clrsetbits32(&ch[0].phy.r[r].b[b].rxdvs[1],
				(0xffff << 0) | (0xffff << 16), (0x2 << 0) | (0x2 << 16));
			clrsetbits32(&ch[0].phy.r[r].b[b].rxdvs[2],
				(0x1 << 23) | (0x1 << 28) | (0x3 << 30),
				(0x1 << 23) | (0x1 << 28) | (0x2 << 30));
		}
	}

	clrbits32(&ch[0].phy.b0_rxdvs[0], 0x1 << 28);
	clrbits32(&ch[0].phy.b1_rxdvs[0], 0x1 << 28);

	for (size_t b = 0; b < 2; b++) {
		setbits32(&ch[0].phy.b[b].dq[9], 0x1 << 0);
		for (size_t r = 0; r < 2; r++)
			clrsetbits32(&ch[0].phy.shu[0].rk[r].b[b].dq[7],
				(0x3f << 8) | (0x3f << 16), (0x1f << 8) | (0x1f << 16));

		clrsetbits32(&ch[0].phy.b[b].dq[4],
			(0x7f << 0) | (0x7f << 8), (0x10 << 0) | (0x10 << 8));
		clrsetbits32(&ch[0].phy.b[b].dq[5],
			(0xff << 0) | (0x3f << 8) | (0x1 << 16) | (0xf << 20) | (0x1 << 24),
			(0x10 << 0) | (0xe << 8) | (0x1 << 16) | (0x1 << 20) | (0x0 << 24));
		clrsetbits32(&ch[0].phy.b[b].dq[6],
			(0x1 << 4) | (0x1 << 7) | (0x1 << 12) | (0x3 << 14) |
			(0xf << 16) | (0x1 << 24),
			(0x0 << 4) | (0x1 << 7) | (0x1 << 12) | (0x0 << 14) |
			(0x3 << 16) | (0x1 << 24));
		clrsetbits32(&ch[0].phy.b[b].dq[5],
			(0xff << 0) | (0x1 << 25), (0x0 << 0) | (0x1 << 25));
	}

	setbits32(&ch[0].phy.ca_cmd[3], (0x3 << 2) | (0x1 << 7));
	clrsetbits32(&ch[0].phy.ca_cmd[6], (0x1 << 6) | (0x3 << 14) | (0x1 << 16),
		(0x0 << 6) | (0x0 << 14) | (0x0 << 16));

	clrbits32(&ch[0].phy.pll3, 0x1 << 0);
	setbits32(&ch[0].phy.b[0].dq[3], 0x1 << 3);
	setbits32(&ch[0].phy.b[1].dq[3], 0x1 << 3);

	udelay(1);
	clrsetbits32(&ch[0].phy.shu[0].pll[8],
		(0x7 << 0) | (0x3 << 18), (0x0 << 0) | (0x1 << 18));

	udelay(1);
	clrbits32(&ch[0].phy.shu[0].pll[9],
		 (0x3 << 8) | (0x1 << 12) | (0x3 << 14) | (0x1 << 16));
	clrbits32(&ch[0].phy.shu[0].pll[11],
		 (0x3 << 8) | (0x1 << 12) | (0x3 << 14) | (0x1 << 16));
	udelay(1);

	clrsetbits32(&ch[0].phy.shu[0].pll[10],
		(0x7 << 0) | (0x3 << 18), (0x0 << 0) | (0x1 << 18));
	udelay(1);

	/* PLL EN */
	/* MID FINE_TUNE Init 1 */
	clrsetbits32(&ch[0].phy.pll4, (0x3 << 18) | (0x1 << 21), 0x3 << 18);

	udelay(1);
	clrsetbits32(&ch[0].phy.shu[0].pll[0], 0xffff << 0, 0x3 << 0);

	udelay(1);
	setbits32(&ch[0].phy.ca_dll_fine_tune[1], 0x1 << 21);

	for (size_t b = 0; b < 2; b++)
		setbits32(&ch[0].phy.b[b].dq[3], (0x3 << 1) | (0x1 << 10));

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	setbits32(&ch[0].phy.shu[0].ca_dll[0], 0x1 << 0);
	setbits32(&ch[1].phy.shu[0].ca_dll[0], 0x1 << 0);
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	for (size_t b = 0; b < 2; b++)
		clrsetbits32(&ch[0].phy.shu[0].b[b].dll[0],
			(0x1 << 4) | (0x3 << 9) | (0xf << 12) |
			(0xf << 16) | (0xf << 20) | (0x1 << 30),
			(0x0 << 4) | (0x3 << 9) | (0x8 << 12) |
			(0x7 << 16) | (0x7 << 20) | (0x1 << 30));

	clrbits32(&ch[0].phy.shu[0].ca_cmd[5], 0x3f << 0);
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[0],
		(0x1 << 4) | (0x7 << 12) | (0x1 << 20),
		(0x1 << 4) | (0x4 << 12) | (0x1 << 20));

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	clrsetbits32(&ch[0].phy.shu[0].ca_cmd[6], 0xffff << 6, 0x3 << 6);
	clrsetbits32(&ch[1].phy.shu[0].ca_cmd[6], 0xffff << 6, 0x1 << 6);
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	for (size_t b = 0; b < 2; b++)
		clrsetbits32(&ch[0].phy.shu[0].b[b].dq[6], 0xffff << 6, 0x1 << 6);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	for (chn = 0; chn < CHANNEL_MAX; chn++)
		clrsetbits32(&ch[chn].phy.misc_shu_opt,
			(0x1 << 0) | (0x3 << 2) | (0x1 << 8) |
			(0x3 << 10) | (0x1 << 16) | (0x3 << 18),
			(0x1 << 0) | (0x2 << 2) | (0x1 << 8) |
			(0x2 << 10) | (0x1 << 16) | ((0x1 + chn) << 18));

	udelay(9);
	clrsetbits32(&ch[0].phy.shu[0].ca_dll[1], (0x1 << 0) | (0x1 << 2), 0x1 << 2);
	clrsetbits32(&ch[1].phy.shu[0].ca_dll[1], (0x1 << 0) | (0x1 << 2), 0x1 << 0);
	dramc_set_broadcast(DRAMC_BROADCAST_ON);

	for (size_t b = 0; b < 2; b++)
		clrsetbits32(&ch[0].phy.shu[0].b[b].dll[1],
			(0x1 << 0) | (0x1 << 2), (0x1 << 0) | (0x0 << 2));
	udelay(1);

	clrbits32(&ch[0].phy.pll2, 0x1 << 31);
	clrsetbits32(&ch[0].phy.misc_cg_ctrl0, 0xffffffff, 0xf);
	udelay(1);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	ddr_phy_reserved_rg_setting(freq_group);
	for (chn = 0; chn < CHANNEL_MAX; chn++)
		ddr_phy_pll_setting(chn, freq_group);

	dramc_set_broadcast(DRAMC_BROADCAST_ON);
	setbits32(&ch[0].ao.drsctrl, 0x1 << 29);

	/* Set Run time MRR CKE fix to 1 in tMRRI old mode
	 * to avoid no ACK from precharge all */
	setbits32(&ch[0].ao.ckectrl, 0x1 << 27);
	clrsetbits32(&ch[0].ao.dramctrl,
		(0x1 << 15) | (0x1 << 17) | (0x1 << 23),
		(0x0 << 15) | (0x1 << 17) | (0x1 << 23));
	setbits32(&ch[0].ao.spcmdctrl, (0x1 << 1) | (0x1 << 8) | (0x1 << 9) | (0x1 << 10));
	setbits32(&ch[0].phy.b[0].dq[9], 0x1 << 4);
	setbits32(&ch[0].phy.b[1].dq[9], 0x1 << 4);

	clrsetbits32(&ch[0].ao.shu[0].rk[1].dqsien,
		(0x7f << 0) | (0x7f << 8) | (0x7f << 16) | (0x7f << 24),
		(0xf << 0) | (0xf << 8) | (0xf << 16) | (0xf << 24));
	clrsetbits32(&ch[0].ao.stbcal1,
		(0x1 << 4) | (0x1 << 8) | (0x1 << 12), (0x1 << 4) | (0x1 << 8));
	clrsetbits32(&ch[0].ao.shu[0].dqsg_retry,
		(0x1 << 3) | (0xf << 8) | (0x1 << 21) | (0x1 << 31),
		(0x1 << 3) | (0x6 << 8) | (0x1 << 21) | (0x1 << 31));

	for (size_t i = 0; i < 4; i++) {
		clrsetbits32(&ch[0].ao.shu[0].drving[i],
			(0x1f << 0) | (0x1f << 5) | (0x1f << 10) |
			(0x1f << 15) | (0x1f << 20) | (0x1f << 25),
			(0xa << 0) | (0xa << 5) | (0xa << 10) |
			(0xa << 15) | (0xa << 20) | (0xa << 25));
	}

	clrsetbits32(&ch[0].ao.shuctrl2,
		(0x3f << 0) | (0x1 << 12) | (0x1 << 14) |
		(0x1 << 15) | (0xff << 16) | (0x1 << 24),
		(0xa << 0) | (0x1 << 12) | (0x1 << 14) |
		(0x1 << 15) | (0x1 << 16) | (0x0 << 24));
	setbits32(&ch[0].ao.dvfsdll, 0x1 << 0);
	setbits32(&ch[0].ao.ddrconf0,
		(0x1 << 12) | (0x1 << 15) | (0x1 << 20) | (0x1 << 26));
	setbits32(&ch[0].ao.stbcal2, (0x1 << 4) | (0x7 << 28));
	clrbits32(&ch[0].ao.stbcal2, 0x1 << 29);
	setbits32(&ch[0].ao.clkar, 0x1 << 19);

	for (size_t b = 0; b < 2; b++)
		clrsetbits32(&ch[0].phy.b[b].dq[9], 0x7 << 20, 0x1 << 20);
	clrsetbits32(&ch[0].phy.ca_cmd[10], 0x7 << 20, 0x0 << 20);
	setbits32(&ch[0].phy.misc_ctrl0,
		(0xf << 0) | (0x1 << 9) | (0x1 << 24) | (0x1 << 31));

	setbits32(&ch[0].phy.misc_ctrl1,
		 (0x1 << 2) | (0x1 << 3) | (0x1 << 15) | (0x1 << 24));
	clrsetbits32(&ch[0].phy.b0_rxdvs[0], 0x1 << 24, 0x1 << 24);
	clrsetbits32(&ch[0].phy.b1_rxdvs[0], 0x1 << 24, 0x1 << 24);
	clrsetbits32(&ch[0].phy.ca_rxdvs0, 0x1 << 24, 0x0 << 24);
	clrbits32(&ch[0].phy.ca_cmd[7], (0x1 << 4) | (0x1 << 6));
	clrbits32(&ch[0].phy.b[0].dq[7], 0x1 << 6);
	clrbits32(&ch[0].phy.b[1].dq[7], 0x1 << 6);

	clrsetbits32(&ch[0].ao.shu[0].conf[0],
		(0x3f << 0) | (0x1 << 7) | (0xf << 12) | (0x1 << 24) |
		(0x1 << 29) | (0x3 << 30),
		(0x3f << 0) | (0x1 << 7) | (0x1 << 12) | (0x1 << 24) |
		(0x1 << 29) | (0x2 << 30));
	clrsetbits32(&ch[0].ao.shu[0].odtctrl,
		(0x1 << 0) | (0x1 << 1) | (0x7f << 16) | (0x1 << 30) | (0x1 << 31),
		(0x1 << 0) | (0x1 << 1) | (0x1 << 16) | (0x1 << 30) | (0x1 << 31));
	setbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 15);
	setbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 15);

	clrsetbits32(&ch[0].ao.refctrl0, 0xf << 24, 0x5 << 24);
	clrbits32(&ch[0].ao.shu[0].selph_ca1,
		 (0x7 << 0) | (0x7 << 4) | (0x7 << 8) | (0x7 << 12) |
		 (0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28));
	clrsetbits32(&ch[0].ao.shu[0].selph_ca2,
		 (0x7 << 0) | (0x7 << 4) | (0x7 << 8) | (0x7 << 16) | (0x7 << 24),
		 (0x0 << 0) | (0x0 << 4) | (0x0 << 8) | (0x7 << 16) | (0x0 << 24));
	clrbits32(&ch[0].ao.shu[0].selph_ca3,
		 (0x7 << 0) | (0x7 << 4) | (0x7 << 8) | (0x7 << 12) |
		 (0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28));
	clrbits32(&ch[0].ao.shu[0].selph_ca4,
		 (0x7 << 0) | (0x7 << 4) | (0x7 << 8) | (0x7 << 12)  |
		 (0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28));
	clrbits32(&ch[0].ao.shu[0].selph_ca5, 0x7 << 8);
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs0, 0x77777777, SELPH_DQS0_3200);
	clrsetbits32(&ch[0].ao.shu[0].selph_dqs1, 0x77777777, SELPH_DQS1_3200);

	for (size_t rank = 0; rank < 2; rank++) {
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[0],
			0x77777777, _SELPH_DQS_BITS(0x3, 0x3));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[1],
			0x77777777, _SELPH_DQS_BITS(0x3, 0x3));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[2],
			0x77777777, _SELPH_DQS_BITS(0x6, 0x2));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[3],
			0x77777777, _SELPH_DQS_BITS(0x6, 0x2));
	}

	for (int b = 0; b < 2; b++) {
		clrsetbits32(&ch[0].phy.shu[0].rk[0].b[b].dq[7],
			(0x3f << 8) | (0x3f << 16), (0x1a << 8) | (0x1a << 16));
		clrsetbits32(&ch[0].phy.shu[0].rk[1].b[b].dq[7],
			(0x3f << 8) | (0x3f << 16), (0x14 << 8) | (0x14 << 16));
	}
	udelay(1);

	for (size_t b = 0; b < 2; b++) {
		setbits32(&ch[0].phy.b[b].dq[9], 0x1 << 5);
		clrsetbits32(&ch[0].phy.b[b].dq[6], 0x3 << 14, 0x1 << 14);
	}
	setbits32(&ch[0].ao.stbcal, 0x1 << 31);
	clrsetbits32(&ch[0].ao.srefctrl, (0xf << 24) | (0x1 << 30), 0x8 << 24);
	clrsetbits32(&ch[0].ao.shu[0].ckectrl,
		(0x3 << 24) | (0x3 << 28), (0x3 << 24) | (0x3 << 28));
	setbits32(&ch[0].ao.shu[0].pipe, (0x1 << 30) | (0x1 << 31));
	setbits32(&ch[0].ao.ckectrl, (0x1 << 13) | (0x1 << 31));
	setbits32(&ch[0].ao.rkcfg, 0x1 << 2);
	clrsetbits32(&ch[0].ao.shu[0].conf[2],
		(0x7 << 16) | (0x1 << 28), (0x7 << 16) | (0x1 << 28));
	clrsetbits32(&ch[0].ao.spcmdctrl, 0x1 << 26, 0x1 << 26);
	clrsetbits32(&ch[0].ao.shuctrl1, 0xff << 0, 0x40 << 0);

	setbits32(&ch[0].ao.shuctrl, 0x1 << 16);
	clrbits32(&ch[0].ao.refctrl1, (0x1 << 1) | (0x1 << 2) | (0x1 << 3) | (0x1 << 6));
	clrsetbits32(&ch[0].ao.refratre_filter, (0x1 << 15) | (0x1 << 23),
		(0x1 << 15) | (0x0 << 23));
	clrbits32(&ch[0].ao.dramctrl, 0x1 << 9);
	setbits32(&ch[0].ao.misctl0, (0x1 << 19) | (0x1 << 24) | (0x1 << 31));
	setbits32(&ch[0].ao.perfctl0,
		(0x1 << 0) | (0x1 << 1) | (0x1 << 4) | (0x1 << 8) |
		(0x1 << 9) | (0x1 << 10) | (0x1 << 11) | (0x1 << 14) | (0x1 << 17));
	clrsetbits32(&ch[0].ao.arbctl, 0xff << 0, 0x80 << 0);
	clrsetbits32(&ch[0].ao.padctrl, (0x3 << 0) | (0x1 << 3), (0x1 << 0) | (0x1 << 3));
	setbits32(&ch[0].ao.dramc_pd_ctrl, 0x1 << 8);
	setbits32(&ch[0].ao.clkctrl, 0x1 << 29);
	clrsetbits32(&ch[0].ao.refctrl0, (0x1 << 0) | (0x7 << 12), (0x1 << 0) | (0x4 << 12));
	clrsetbits32(&ch[0].ao.shu[0].rankctl, (0xf << 20) | (0xf << 24) | (0xf << 28),
		(0x4 << 20) | (0x4 << 24) | (0x6 << 28));
	udelay(2);

	clrsetbits32(&ch[0].ao.shu[0].rk[0].dqsien,
		(0x7f << 0) | (0x7f << 8), (0x19 << 0) | (0x19 << 8));
	clrsetbits32(&ch[0].ao.shu[0].rk[1].dqsien,
		(0x7f << 0) | (0x7f << 8) | (0x7f << 16) | (0x7f << 24),
		(0x1b << 0) | (0x1b << 8) | (0x0 << 16) | (0x0 << 24));

	setbits32(&ch[0].ao.dramctrl, 0x1 << 19);
	clrsetbits32(&ch[0].ao.zqcs, 0xff << 0, 0x56 << 0);
	udelay(1);

	clrsetbits32(&ch[0].ao.shu[0].conf[3], 0x1ff << 16, 0xff << 16);
	setbits32(&ch[0].ao.refctrl0, 0x1 << 30);
	setbits32(&ch[0].ao.srefctrl, 0x1 << 30);
	setbits32(&ch[0].ao.mpc_option, 0x1 << 17);
	setbits32(&ch[0].ao.dramc_pd_ctrl, 0x1 << 30);
	setbits32(&ch[0].ao.dramc_pd_ctrl, 0x1 << 0);
	clrsetbits32(&ch[0].ao.eyescan, (0x1 << 1) | (0xf << 16), (0x0 << 1) | (0x1 << 16));
	setbits32(&ch[0].ao.stbcal1, (0x1 << 10) | (0x1 << 11));
	clrsetbits32(&ch[0].ao.test2_1, 0xfffffff << 4, 0x10000 << 4);
	clrsetbits32(&ch[0].ao.test2_2, 0xfffffff << 4, 0x400 << 4);
	clrsetbits32(&ch[0].ao.test2_3,
		(0x1 << 7) | (0x7 << 8) | (0x1 << 28),
		(0x1 << 7) | (0x4 << 8) | (0x1 << 28));
	clrbits32(&ch[0].ao.rstmask, 0x1 << 29);
	clrbits32(&ch[0].ao.rstmask, 0x1 << 30);

	udelay(1);
	clrsetbits32(&ch[0].ao.hw_mrr_fun, (0xf << 0) | (0xf << 4), (0x8 << 0) | (0x6 << 4));

	clrbits32(&ch[0].ao.dramctrl, 0x1 << 0);
	clrsetbits32(&ch[0].ao.perfctl0,
		(0x1 << 18) | (0x1 << 19), (0x0 << 18) | (0x1 << 19));
	setbits32(&ch[0].ao.spcmdctrl, 0x1 << 28);
	clrbits32(&ch[0].ao.rstmask, 0x1 << 28);
	setbits32(&ch[0].ao.rkcfg, 0x1 << 11);
	setbits32(&ch[0].ao.mpc_option, 0x1 << 17);
	setbits32(&ch[0].ao.eyescan, 0x1 << 2);
	setbits32(&ch[0].ao.shu[0].wodt, 0x1 << 29);
	setbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 7);
	setbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 7);
	clrsetbits32(&ch[0].ao.shu[0].rankctl, 0xf << 20, 0x4 << 20);

	for (size_t r = 0; r < 2; r++) {
		clrsetbits32(&ch[0].ao.shu[0].rk[r].selph_dq[0],
			(0x7 << 0) | (0x7 << 4), (0x2 << 0) | (0x2 << 4));
		clrsetbits32(&ch[0].ao.shu[0].rk[r].selph_dq[1],
			(0x7 << 0) | (0x7 << 4), (0x2 << 0) | (0x2 << 4));
	}
	udelay(5);

	clrsetbits32(&ch[0].ao.stbcal1, 0xffff << 16, 0x3 << 16);
	clrsetbits32(&ch[0].ao.stbcal1, 0xffff << 16, 0x1 << 16);
	clrsetbits32(&ch[0].ao.stbcal,
		(0x1 << 0) | (0x1 << 22) | (0x1 << 24) | (0x1 << 26) | (0x1 << 27),
		(0x1 << 0) | (0x0 << 22) | (0x0 << 24) | (0x1 << 26) | (0x1 << 27));
	setbits32(&ch[0].ao.stbcal1, 0x1 << 6);
	clrsetbits32(&ch[0].ao.shu[0].dqsg,
		(0x1 << 11) | (0xf << 12), (0x1 << 11) | (0x9 << 12));
	clrbits32(&ch[0].phy.misc_ctrl0, 0xf << 0);
	setbits32(&ch[0].ao.shu[0].stbcal, 0x1 << 8);
	setbits32(&ch[0].ao.stbcal, 0x1 << 17);
	clrbits32(&ch[0].phy.shu[0].b[0].dq[7], 0x1 << 14);
	clrbits32(&ch[0].phy.shu[0].b[1].dq[7], 0x1 << 14);
	clrsetbits32(&ch[0].ao.shu[0].stbcal, 0x7 << 4, 0x1 << 4);

	if (freq_group == LP4X_DDR1600)
		clrsetbits32(&ch[0].ao.shu[0].stbcal, 0x3 << 0, 0x0 << 0);
	else
		clrsetbits32(&ch[0].ao.shu[0].stbcal, 0x3 << 0, 0x2 << 0);
	setbits32(&ch[0].ao.refctrl1, (0x1 << 0) | (0x1 << 5));
	setbits32(&ch[0].ao.dqsoscr, (0x1 << 23) | (0x1 << 27));
	clrbits32(&ch[0].ao.rstmask, (0x1 << 24) | (0x1 << 25) | (0x1 << 26));
	clrsetbits32(&ch[0].ao.rkcfg, 0x7 << 4, 0x1 << 4);
	udelay(12);

	clrsetbits32(&ch[0].ao.shu[0].rankctl,
		(0xf << 24) | (0xf << 28), (0x4 << 24) | 0x6 << 28);
	clrbits32(&ch[0].ao.shu[0].wodt, 0x1 << 31);
	clrsetbits32(&ch[0].ao.shu[0].rk[0].fine_tune,
		(0x3f << 0) | (0x3f << 8) | (0x3f << 16) | (0x3f << 24),
		(0x1a << 0) | (0x1a << 8) | (0x1a << 16) | (0x1a << 24));
	clrsetbits32(&ch[0].ao.shu[0].rk[1].fine_tune,
		(0x3f << 0) | (0x3f << 8) | (0x3f << 16) | (0x3f << 24),
		(0x14 << 0) | (0x14 << 8) | (0x14 << 16) | (0x14 << 24));
	for (u8 rank = 0; rank < 2; rank++) {
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[2],
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28),
			(0x4 << 16) | (0x4 << 20) | (0x4 << 24) | (0x4 << 28));
		clrsetbits32(&ch[0].ao.shu[0].rk[rank].selph_dq[3],
			(0x7 << 16) | (0x7 << 20) | (0x7 << 24) | (0x7 << 28),
			(0x4 << 16) | (0x4 << 20) | (0x4 << 24) | (0x4 << 28));
	}
	clrsetbits32(&ch[0].ao.shu[0].dqsg_retry,
		(0x1 << 2) | (0xf << 8) | (0x1 << 14) | (0x3 << 24),
		(0x1 << 2) | (0x5 << 8) | (0x0 << 14) | (0x1 << 24));
	setbits32(&ch[0].phy.shu[0].b[0].dq[7], (0x1 << 12) | (0x1 << 13));
	setbits32(&ch[0].phy.shu[0].b[1].dq[7], (0x1 << 12) | (0x1 << 13));
	clrbits32(&ch[0].ao.shu[0].dqs2dq_tx, 0x1f << 0);

	/* The default dramc init settings were tuned at frequency of 3200Mbps.
	   For other frequencies uses dramc_setting_DDRxxx() to overwrite
	   the default settings. */
	switch (freq_group) {
	case LP4X_DDR1600:
		dramc_setting_DDR1600();
		break;
	case LP4X_DDR2400:
		dramc_setting_DDR2400();
		break;
	case LP4X_DDR3200:
		/* Do nothing */
		break;
	case LP4X_DDR3600:
		dramc_setting_DDR3600();
		break;
	default:
		die("Invalid DDR frequency group %u\n", freq_group);
		return;
	}

	update_initial_settings(freq_group);
	dramc_sw_impedance_save_reg(freq_group, impedance);

	clrbits32(&ch[0].ao.test2_4, 0x1 << 17);
	clrsetbits32(&ch[0].ao.shu[0].conf[3], 0x1ff << 0, 0x5 << 0);
	udelay(1);

	setbits32(&ch[0].ao.refctrl0, (0x1 << 17) | (0x1 << 18));
	setbits32(&ch[0].ao.shuctrl2, (0x1 << 24) | (0x1 << 25));
	setbits32(&ch[0].ao.refctrl0, 0x1 << 29);
	setbits32(&ch[0].ao.dramctrl, 0x1 << 26);
	clrsetbits32(&ch[0].ao.dummy_rd,
		(0x1 << 4) | (0x1 << 11) | (0x1 << 13) |
		(0x1 << 14) | (0x3 << 16) | (0x1 << 22),
		(0x1 << 4) | (0x1 << 11) | (0x1 << 13) |
		(0x1 << 14) | (0x2 << 16) | (0x1 << 22));
	clrsetbits32(&ch[0].ao.test2_4, 0x7 << 28, 0x4 << 28);
	clrbits32(&ch[0].ao.dramctrl, 0x1 << 0);
	udelay(1);

	dramc_set_broadcast(DRAMC_BROADCAST_OFF);
	clrsetbits32(&ch[0].ao.shuctrl, (0x1 << 5) | (0x1 << 17), (0x0 << 5) | (0x1 << 17));
	setbits32(&ch[0].ao.shuctrl2, 0x1 << 12);
	clrsetbits32(&ch[1].ao.shuctrl, (0x1 << 5) | (0x1 << 17), (0x1 << 5) | (0x0 << 17));
	clrbits32(&ch[1].ao.shuctrl2, 0x1 << 12);
}

struct ac_time {
	u8 dqsinctl;
	u8 datlat;
	u8 trcd;
	u8 trrd;
	u8 twr;
	u8 twtr;
	u8 trc;
	u8 tras;
	u8 trp;
	u8 trpab;
	u8 tfaw;
	u8 trtw_odt_on;
	u8 trtp;
	u8 txp;
	u8 refcnt;
	u8 trfc;
	u8 trfcpb;
	u8 tzqcs;
	u8 refcnt_fr_clk;
	u8 txrefcnt;
	u8 tmrr2w_odt_on;
	u8 twtpd;
	u8 trtpd;
	u8 xrtw2w;
	u8 xrtw2r;
	u8 xrtr2w;
	u8 xrtr2r;
	u8 twtr_05T;
	u8 trtw_odt_on_05T;
	u8 twtpd_05T;
	u8 trtpd_05T;
	u8 tfaw_05T;
	u8 trrd_05T;
	u8 twr_05T;
	u8 tras_05T;
	u8 trpab_05T;
	u8 trp_05T;
	u8 trcd_05T;
	u8 trtp_05T;
	u8 txp_05T;
	u8 trfc_05T;
	u8 trfcpb_05T;
	u8 trc_05T;
	u8 r_dmcatrain_intv;
	u8 r_dmmrw_intv;
	u8 r_dmfspchg_prdcnt;
	u8 ckeprd;
	u8 ckelckcnt;
	u8 zqlat2;
};

static const struct ac_time ac_timing_tbl[LP4X_DDRFREQ_MAX] = {
	/* LP4x-1600, 800MHz, RDBI_OFF, normal mode */
	[LP4X_DDR1600] = {
		.tras = 0,	.tras_05T = 0,
		.trp = 2,	.trp_05T = 0,
		.trpab = 0,	.trpab_05T = 1,
		.trc = 4,	.trc_05T = 0,
		.trfc = 44,	.trfc_05T = 0,
		.trfcpb = 16,	.trfcpb_05T = 0,
		.txp = 0,	.txp_05T = 0,
		.trtp = 1,	.trtp_05T = 1,
		.trcd = 3,	.trcd_05T = 0,
		.twr = 7,	.twr_05T = 1,
		.twtr = 4,	.twtr_05T = 1,
		.trrd = 0,	.trrd_05T = 0,
		.tfaw = 0,	.tfaw_05T = 0,
		.trtw_odt_on = 4,	.trtw_odt_on_05T = 0,
		.refcnt = 48,
		.refcnt_fr_clk = 101,
		.txrefcnt = 62,
		.tzqcs = 16,
		.xrtw2w = 5,
		.xrtw2r = 3,
		.xrtr2w = 3,
		.xrtr2r = 8,
		.r_dmcatrain_intv = 8,
		.r_dmmrw_intv = 0xf,
		.r_dmfspchg_prdcnt = 50,
		.trtpd = 6,	.trtpd_05T = 0,
		.twtpd = 6,	.twtpd_05T = 0,
		.tmrr2w_odt_on = 5,
		.ckeprd = 1,
		.ckelckcnt = 0,
		.zqlat2 = 6,
		.dqsinctl = 1,	 .datlat = 10,
	},
	/* LP4x-2400, 1200MHz, RDBI_OFF, normal mode */
	[LP4X_DDR2400] = {
		.tras = 4,	.tras_05T = 1,
		.trp = 3,	.trp_05T = 1,
		.trpab = 1,	.trpab_05T = 0,
		.trc = 10,	.trc_05T = 0,
		.trfc = 72,	.trfc_05T = 0,
		.trfcpb = 30,	.trfcpb_05T = 0,
		.txp = 0,	.txp_05T = 1,
		.trtp = 1,	.trtp_05T = 0,
		.trcd = 4,	.trcd_05T = 1,
		.twr = 10,	.twr_05T = 1,
		.twtr = 6,	.twtr_05T = 1,
		.trrd = 1,	.trrd_05T = 0,
		.tfaw = 3,	.tfaw_05T = 0,
		.trtw_odt_on = 7,	.trtw_odt_on_05T = 0,
		.refcnt = 73,
		.refcnt_fr_clk = 101,
		.txrefcnt = 91,
		.tzqcs = 25,
		.xrtw2w = 5,
		.xrtw2r = 3,
		.xrtr2w = 6,
		.xrtr2r = 8,
		.r_dmcatrain_intv = 9,
		.r_dmmrw_intv = 0xf,
		.r_dmfspchg_prdcnt = 75,
		.trtpd = 9,	.trtpd_05T = 0,
		.twtpd = 9,	.twtpd_05T = 0,
		.tmrr2w_odt_on = 8,
		.ckeprd = 2,
		.ckelckcnt = 0,
		.zqlat2 = 9,
		.dqsinctl = 3,	 .datlat = 13,
	},
	/* LP4x-3200, 1600MHz, RDBI_OFF, normal mode */
	[LP4X_DDR3200] = {
		.tras = 8,	.tras_05T = 1,
		.trp = 5,	.trp_05T = 1,
		.trpab = 1,	.trpab_05T = 0,
		.trc = 16,	.trc_05T = 1,
		.trfc = 100,	.trfc_05T = 0,
		.trfcpb = 44,	.trfcpb_05T = 0,
		.txp = 1,	.txp_05T = 0,
		.trtp = 2,	.trtp_05T = 1,
		.trcd = 6,	.trcd_05T = 1,
		.twr = 12,	.twr_05T = 1,
		.twtr = 7,	.twtr_05T = 0,
		.trrd = 2,	.trrd_05T = 0,
		.tfaw = 7,	.tfaw_05T = 0,
		.trtw_odt_on = 7,	.trtw_odt_on_05T = 0,
		.refcnt = 97,
		.refcnt_fr_clk = 101,
		.txrefcnt = 119,
		.tzqcs = 34,
		.xrtw2w = 5,
		.xrtw2r = 3,
		.xrtr2w = 6,
		.xrtr2r = 9,
		.r_dmcatrain_intv = 11,
		.r_dmmrw_intv = 0xf,
		.r_dmfspchg_prdcnt = 100,
		.trtpd = 11,	.trtpd_05T = 0,
		.twtpd = 12,	.twtpd_05T = 1,
		.tmrr2w_odt_on = 10,
		.ckeprd = 2,
		.ckelckcnt = 0,
		.zqlat2 = 12,
		.dqsinctl = 4,	 .datlat = 15,
	},
	/* LP4x-3600, 1800MHz, RDBI_OFF, normal mode */
	[LP4X_DDR3600] = {
		.tras = 11,	.tras_05T = 1,
		.trp = 6,	.trp_05T = 1,
		.trpab = 1,	.trpab_05T = 1,
		.trc = 20,	.trc_05T = 1,
		.trfc = 118,	.trfc_05T = 1,
		.trfcpb = 53,	.trfcpb_05T = 1,
		.txp = 1,	.txp_05T = 1,
		.trtp = 2,	.trtp_05T = 0,
		.trcd = 7,	.trcd_05T = 1,
		.twr = 14,	.twr_05T = 1,
		.twtr = 8,	.twtr_05T = 0,
		.trrd = 3,	.trrd_05T = 0,
		.tfaw = 10,	.tfaw_05T = 0,
		.trtw_odt_on = 8,	.trtw_odt_on_05T = 0,
		.refcnt = 113,
		.refcnt_fr_clk = 101,
		.txrefcnt = 138,
		.tzqcs = 40,
		.xrtw2w = 5,
		.xrtw2r = 3,
		.xrtr2w = 7,
		.xrtr2r = 9,
		.r_dmcatrain_intv = 13,
		.r_dmmrw_intv = 0xf,
		.r_dmfspchg_prdcnt = 117,
		.trtpd = 12,	.trtpd_05T = 0,
		.twtpd = 13,	.twtpd_05T = 0,
		.tmrr2w_odt_on = 11,
		.ckeprd = 3,
		.ckelckcnt = 0,
		.zqlat2 = 14,
		.dqsinctl = 6,	 .datlat = 18,
	},
};

static void ddr_update_ac_timing(u8 freq_group)
{
	struct ac_time ac_t;
	u32 temp, r2w_odt_onoff = ODT_ON;
	u8 new_datlat;
	u8 root = 0, tx_rank_inctl = 0, tx_dly = 0;
	u8 trtw = 0, trtw_05t = 0, tmrr2w = 0;

	memcpy(&ac_t, &ac_timing_tbl[freq_group], sizeof(struct ac_time));
	new_datlat = ac_timing_tbl[freq_group].datlat - 2;

	if (freq_group == LP4X_DDR1600) {
		root = 0; tx_rank_inctl = 0; tx_dly = 1;
	} else {
		root = (freq_group == LP4X_DDR3600) ? 1 : 0;
		tx_rank_inctl = 1; tx_dly = 2;
	}

	if (r2w_odt_onoff == ODT_ON) {
		trtw = ac_t.trtw_odt_on;
		trtw_05t = ac_t.trtw_odt_on_05T;
		tmrr2w = ac_t.tmrr2w_odt_on;
	}

	for (u8 chn = 0; chn < CHANNEL_MAX; chn++) {
		clrsetbits32(&ch[chn].ao.shu[0].actim[0],
			(0xf << 24) | (0x7 << 16) | (0x1f << 8) | (0xf << 0),
			(ac_t.trcd << 24) | (ac_t.trrd << 16) |
			(ac_t.twr << 8) | (ac_t.twtr << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim[1],
			(0x1f << 24) | (0xf << 16) | (0xf << 8) | (0x7 << 0),
			(ac_t.trc << 24) | (ac_t.tras << 16) |
			(ac_t.trp << 8) | (ac_t.trpab << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim[2],
			(0x1f << 24) | (0xf << 16) | (0x7 << 8) | (0x7 << 0),
			(ac_t.tfaw << 24) | (trtw << 16) |
			(ac_t.trtp << 8) | (ac_t.txp << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim[3],
			(0xff << 16) | (0xff << 24) | (0xff << 0),
			(ac_t.trfc << 16) | (ac_t.refcnt << 24) | (ac_t.trfcpb << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim[4],
			(0xff << 24) | (0xff << 16) | (0x3ff << 0),
			(ac_t.tzqcs << 24) | (ac_t.refcnt_fr_clk << 16) |
			(ac_t.txrefcnt << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim[5],
			(0xf << 24) | (0x1f << 8) | (0x1f << 0),
			(tmrr2w << 24) | (ac_t.twtpd << 8) | (ac_t.trtpd << 0));
		clrsetbits32(&ch[chn].ao.shu[0].actim_xrt,
			(0xf << 24) | (0x7 << 16) | (0xf << 8) | (0x1f << 0),
			(ac_t.xrtw2w << 24) | (ac_t.xrtw2r << 16) |
			(ac_t.xrtr2w << 8) | (ac_t.xrtr2r << 0));
		clrsetbits32(&ch[chn].ao.shu[0].ac_time_05t,
			(0x1 << 25) | (0x0 << 24) | (0x1 << 16) | (0x0 << 15) |
			(0x1 << 13) | (0x1 << 12) | (0x1 << 10) | (0x1 << 9) |
			(0x1 << 8) | (0x1 << 7) | (0x1 << 6) | (0x1 << 5) |
			(0x1 << 4) | (0x1 << 2) | (0x1 << 1) | (0x1 << 0),
			(ac_t.twtr_05T << 25) | (trtw_05t << 24) |
			(ac_t.twtpd_05T << 16) | (ac_t.trtpd_05T << 15) |
			(ac_t.tfaw_05T << 13) | (ac_t.trrd_05T << 12) |
			(ac_t.twr_05T << 10) | (ac_t.tras_05T << 9) |
			(ac_t.trpab_05T << 8) | (ac_t.trp_05T << 7) |
			(ac_t.trcd_05T << 6) | (ac_t.trtp_05T << 5) |
			(ac_t.txp_05T << 4) | (ac_t.trfc_05T << 2) |
			(ac_t.trfcpb_05T << 1) | (ac_t.trc_05T << 0));
		clrsetbits32(&ch[chn].ao.catraining1, (0xff << 24) | (0xf << 20),
			(ac_t.r_dmcatrain_intv << 24) | (0x0 << 20));

		/* DQSINCTL related */
		clrsetbits32(&ch[chn].ao.shu[0].rk[0].dqsctl, 0xf << 0,
			ac_t.dqsinctl << 0);
		clrsetbits32(&ch[chn].ao.shu[0].rk[1].dqsctl, 0xf << 0,
			ac_t.dqsinctl << 0);
		clrsetbits32(&ch[chn].ao.shu[0].odtctrl, 0xf << 4,
			ac_t.dqsinctl << 4);

		/* DATLAT related, tREFBW */
		clrsetbits32(&ch[chn].ao.shu[0].conf[1],
			(0x1f << 0) | (0x1f << 8) | (0x1f << 26) | (0x3ff << 16),
			(ac_t.datlat << 0) | (new_datlat << 8) |
			(new_datlat << 26) | (0x0 << 16));
		clrsetbits32(&ch[chn].ao.shu[0].conf[2],
			(0xff << 8), ac_t.r_dmfspchg_prdcnt << 8);
		clrsetbits32(&ch[chn].ao.shu[0].scintv, (0x1f << 13) | (0x1f << 6),
			(ac_t.r_dmmrw_intv << 13) | (ac_t.zqlat2 << 6));

		/* CKEPRD - CKE pulse width */
		clrsetbits32(&ch[chn].ao.shu[0].ckectrl, 0x7 << 20, ac_t.ckeprd << 20);

		/* CKELCKCNT: Valid clock requirement after CKE input low */
		clrsetbits32(&ch[chn].ao.ckectrl, 0x7 << 24, ac_t.ckelckcnt << 24);

		temp = ((read32(&ch[chn].ao.shu[0].rankctl) & 0x00f00000) >> 20) & 0xf;
		clrsetbits32(&ch[chn].ao.shu[0].rankctl, 0xf << 0, temp << 0);

		clrsetbits32(&ch[chn].ao.shu[0].rankctl,
			(0xf << 16) | (0xf << 12) | (0xf << 8),
			(root << 16) | (tx_rank_inctl << 12) | (tx_dly << 8));
	}

	u8 dram_cbt_mode = 0;
	clrsetbits32(&ch[0].ao.arbctl, 0x7 << 10, 0x3 << 10);
	clrsetbits32(&ch[0].ao.rstmask, 0x3 << 13, dram_cbt_mode);
	clrsetbits32(&ch[0].ao.arbctl, 0x1 << 13, dram_cbt_mode);
}

void dramc_init(const struct sdram_params *params, u8 freq_group,
		struct dram_shared_data *shared)
{
	dramc_setting(params, freq_group, &shared->impedance);

	dramc_duty_calibration(params, freq_group);
	dvfs_settings(freq_group);

	dramc_mode_reg_init(freq_group, &shared->mr);
	ddr_update_ac_timing(freq_group);
}
