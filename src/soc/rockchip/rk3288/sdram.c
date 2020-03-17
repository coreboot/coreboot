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
#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/clock.h>
#include <soc/sdram.h>
#include <soc/grf.h>
#include <soc/soc.h>
#include <soc/pmu.h>
#include <types.h>

struct rk3288_ddr_pctl_regs {
	u32 scfg;
	u32 sctl;
	u32 stat;
	u32 intrstat;
	u32 reserved0[12];
	u32 mcmd;
	u32 powctl;
	u32 powstat;
	u32 cmdtstat;
	u32 tstaten;
	u32 reserved1[3];
	u32 mrrcfg0;
	u32 mrrstat0;
	u32 mrrstat1;
	u32 reserved2[4];
	u32 mcfg1;
	u32 mcfg;
	u32 ppcfg;
	u32 mstat;
	u32 lpddr2zqcfg;
	u32 reserved3;
	u32 dtupdes;
	u32 dtuna;
	u32 dtune;
	u32 dtuprd0;
	u32 dtuprd1;
	u32 dtuprd2;
	u32 dtuprd3;
	u32 dtuawdt;
	u32 reserved4[3];
	u32 togcnt1u;
	u32 tinit;
	u32 trsth;
	u32 togcnt100n;
	u32 trefi;
	u32 tmrd;
	u32 trfc;
	u32 trp;
	u32 trtw;
	u32 tal;
	u32 tcl;
	u32 tcwl;
	u32 tras;
	u32 trc;
	u32 trcd;
	u32 trrd;
	u32 trtp;
	u32 twr;
	u32 twtr;
	u32 texsr;
	u32 txp;
	u32 txpdll;
	u32 tzqcs;
	u32 tzqcsi;
	u32 tdqs;
	u32 tcksre;
	u32 tcksrx;
	u32 tcke;
	u32 tmod;
	u32 trstl;
	u32 tzqcl;
	u32 tmrr;
	u32 tckesr;
	u32 tdpd;
	u32 reserved5[14];
	u32 ecccfg;
	u32 ecctst;
	u32 eccclr;
	u32 ecclog;
	u32 reserved6[28];
	u32 dtuwactl;
	u32 dturactl;
	u32 dtucfg;
	u32 dtuectl;
	u32 dtuwd0;
	u32 dtuwd1;
	u32 dtuwd2;
	u32 dtuwd3;
	u32 dtuwdm;
	u32 dturd0;
	u32 dturd1;
	u32 dturd2;
	u32 dturd3;
	u32 dtulfsrwd;
	u32 dtulfsrrd;
	u32 dtueaf;
	u32 dfitctrldelay;
	u32 dfiodtcfg;
	u32 dfiodtcfg1;
	u32 dfiodtrankmap;
	u32 dfitphywrdata;
	u32 dfitphywrlat;
	u32 reserved7[2];
	u32 dfitrddataen;
	u32 dfitphyrdlat;
	u32 reserved8[2];
	u32 dfitphyupdtype0;
	u32 dfitphyupdtype1;
	u32 dfitphyupdtype2;
	u32 dfitphyupdtype3;
	u32 dfitctrlupdmin;
	u32 dfitctrlupdmax;
	u32 dfitctrlupddly;
	u32 reserved9;
	u32 dfiupdcfg;
	u32 dfitrefmski;
	u32 dfitctrlupdi;
	u32 reserved10[4];
	u32 dfitrcfg0;
	u32 dfitrstat0;
	u32 dfitrwrlvlen;
	u32 dfitrrdlvlen;
	u32 dfitrrdlvlgateen;
	u32 dfiststat0;
	u32 dfistcfg0;
	u32 dfistcfg1;
	u32 reserved11;
	u32 dfitdramclken;
	u32 dfitdramclkdis;
	u32 dfistcfg2;
	u32 dfistparclr;
	u32 dfistparlog;
	u32 reserved12[3];
	u32 dfilpcfg0;
	u32 reserved13[3];
	u32 dfitrwrlvlresp0;
	u32 dfitrwrlvlresp1;
	u32 dfitrwrlvlresp2;
	u32 dfitrrdlvlresp0;
	u32 dfitrrdlvlresp1;
	u32 dfitrrdlvlresp2;
	u32 dfitrwrlvldelay0;
	u32 dfitrwrlvldelay1;
	u32 dfitrwrlvldelay2;
	u32 dfitrrdlvldelay0;
	u32 dfitrrdlvldelay1;
	u32 dfitrrdlvldelay2;
	u32 dfitrrdlvlgatedelay0;
	u32 dfitrrdlvlgatedelay1;
	u32 dfitrrdlvlgatedelay2;
	u32 dfitrcmd;
	u32 reserved14[46];
	u32 ipvr;
	u32 iptr;
};
check_member(rk3288_ddr_pctl_regs, iptr, 0x03fc);

struct rk3288_ddr_publ_datx {
	u32 dxgcr;
	u32 dxgsr[2];
	u32 dxdllcr;
	u32 dxdqtr;
	u32 dxdqstr;
	u32 reserved[10];
};

struct rk3288_ddr_publ_regs {
	u32 ridr;
	u32 pir;
	u32 pgcr;
	u32 pgsr;
	u32 dllgcr;
	u32 acdllcr;
	u32 ptr[3];
	u32 aciocr;
	u32 dxccr;
	u32 dsgcr;
	u32 dcr;
	u32 dtpr[3];
	u32 mr[4];
	u32 odtcr;
	u32 dtar;
	u32 dtdr[2];
	u32 reserved1[24];
	u32 dcuar;
	u32 dcudr;
	u32 dcurr;
	u32 dculr;
	u32 dcugcr;
	u32 dcutpr;
	u32 dcusr[2];
	u32 reserved2[8];
	u32 bist[17];
	u32 reserved3[15];
	u32 zq0cr[2];
	u32 zq0sr[2];
	u32 zq1cr[2];
	u32 zq1sr[2];
	u32 zq2cr[2];
	u32 zq2sr[2];
	u32 zq3cr[2];
	u32 zq3sr[2];
	struct rk3288_ddr_publ_datx datx8[4];
};
check_member(rk3288_ddr_publ_regs, datx8[3].dxdqstr, 0x0294);

struct rk3288_msch_regs {
	u32 coreid;
	u32 revisionid;
	u32 ddrconf;
	u32 ddrtiming;
	u32 ddrmode;
	u32 readlatency;
	u32 reserved1[8];
	u32 activate;
	u32 devtodev;
};
check_member(rk3288_msch_regs, devtodev, 0x003c);

static struct rk3288_ddr_pctl_regs * const rk3288_ddr_pctl[2] = {
	(void *)DDR_PCTL0_BASE, (void *)DDR_PCTL1_BASE};
static struct rk3288_ddr_publ_regs * const rk3288_ddr_publ[2] = {
	(void *)DDR_PUBL0_BASE, (void *)DDR_PUBL1_BASE};
static struct rk3288_msch_regs * const rk3288_msch[2] = {
	(void *)SERVICE_BUS_BASE, (void *)SERVICE_BUS_BASE + 0x80};

/* PCT_DFISTCFG0 */
#define DFI_INIT_START			(1 << 0)

/* PCT_DFISTCFG1 */
#define DFI_DRAM_CLK_SR_EN		(1 << 0)
#define DFI_DRAM_CLK_DPD_EN		(1 << 1)

/* PCT_DFISTCFG2 */
#define DFI_PARITY_INTR_EN		(1 << 0)
#define DFI_PARITY_EN			(1 << 1)

/* PCT_DFILPCFG0 */
#define TLP_RESP_TIME(n)		(n << 16)
#define LP_SR_EN			(1 << 8)
#define LP_PD_EN			(1 << 0)

/* PCT_DFITCTRLDELAY */
#define TCTRL_DELAY_TIME(n)		(n << 0)

/* PCT_DFITPHYWRDATA */
#define TPHY_WRDATA_TIME(n)		(n << 0)

/* PCT_DFITPHYRDLAT */
#define TPHY_RDLAT_TIME(n)		(n << 0)

/* PCT_DFITDRAMCLKDIS */
#define TDRAM_CLK_DIS_TIME(n)		(n << 0)

/* PCT_DFITDRAMCLKEN */
#define TDRAM_CLK_EN_TIME(n)		(n << 0)

/* PCTL_DFIODTCFG */
#define RANK0_ODT_WRITE_SEL		(1 << 3)
#define RANK1_ODT_WRITE_SEL		(1 << 11)

/* PCTL_DFIODTCFG1 */
#define ODT_LEN_BL8_W(n)		(n<<16)

/* PUBL_ACDLLCR */
#define ACDLLCR_DLLDIS			(1 << 31)
#define ACDLLCR_DLLSRST			(1 << 30)

/* PUBL_DXDLLCR */
#define DXDLLCR_DLLDIS			(1 << 31)
#define DXDLLCR_DLLSRST			(1 << 30)

/* PUBL_DLLGCR */
#define DLLGCR_SBIAS			(1 << 30)

/* PUBL_DXGCR */
#define DQSRTT				(1 << 9)
#define DQRTT				(1 << 10)

/* PIR */
#define PIR_INIT			(1 << 0)
#define PIR_DLLSRST			(1 << 1)
#define PIR_DLLLOCK			(1 << 2)
#define PIR_ZCAL			(1 << 3)
#define PIR_ITMSRST			(1 << 4)
#define PIR_DRAMRST			(1 << 5)
#define PIR_DRAMINIT			(1 << 6)
#define PIR_QSTRN			(1 << 7)
#define PIR_RVTRN			(1 << 8)
#define PIR_ICPC			(1 << 16)
#define PIR_DLLBYP			(1 << 17)
#define PIR_CTLDINIT			(1 << 18)
#define PIR_CLRSR			(1 << 28)
#define PIR_LOCKBYP			(1 << 29)
#define PIR_ZCALBYP			(1 << 30)
#define PIR_INITBYP			(1u << 31)

/* PGCR */
#define PGCR_DFTLMT(n)			((n) << 3)
#define PGCR_DFTCMP(n)			((n) << 2)
#define PGCR_DQSCFG(n)			((n) << 1)
#define PGCR_ITMDMD(n)			((n) << 0)

/* PGSR */
#define PGSR_IDONE			(1 << 0)
#define PGSR_DLDONE			(1 << 1)
#define PGSR_ZCDONE			(1 << 2)
#define PGSR_DIDONE			(1 << 3)
#define PGSR_DTDONE			(1 << 4)
#define PGSR_DTERR			(1 << 5)
#define PGSR_DTIERR			(1 << 6)
#define PGSR_DFTERR			(1 << 7)
#define PGSR_RVERR			(1 << 8)
#define PGSR_RVEIRR			(1 << 9)

/* PTR0 */
#define PRT_ITMSRST(n)			((n) << 18)
#define PRT_DLLLOCK(n)			((n) << 6)
#define PRT_DLLSRST(n)			((n) << 0)

/* PTR1 */
#define PRT_DINIT0(n)			((n) << 0)
#define PRT_DINIT1(n)			((n) << 19)

/* PTR2 */
#define PRT_DINIT2(n)			((n) << 0)
#define PRT_DINIT3(n)			((n) << 17)

/* DCR */
#define DDRMD_LPDDR			0
#define DDRMD_DDR			1
#define DDRMD_DDR2			2
#define DDRMD_DDR3			3
#define DDRMD_LPDDR2_LPDDR3		4
#define DDRMD_MSK			(7 << 0)
#define DDRMD_CFG(n)			((n) << 0)
#define PDQ_MSK				(7 << 4)
#define PDQ_CFG(n)			((n) << 4)

/* DXCCR */
#define DQSNRES_MSK			(0x0f << 8)
#define DQSNRES_CFG(n)			((n) << 8)
#define DQSRES_MSK			(0x0f << 4)
#define DQSRES_CFG(n)			((n) << 4)

/* DTPR */
#define TDQSCKMAX_VAL(n)		(((n) >> 27) & 7)
#define TDQSCK_VAL(n)			(((n) >> 24) & 7)

/* DSGCR */
#define DQSGX_MSK			(0x07 << 5)
#define DQSGX_CFG(n)			((n) << 5)
#define DQSGE_MSK			(0x07 << 8)
#define DQSGE_CFG(n)			((n) << 8)

/* SCTL */
#define INIT_STATE			(0)
#define CFG_STATE			(1)
#define GO_STATE			(2)
#define SLEEP_STATE			(3)
#define WAKEUP_STATE			(4)

/* STAT */
#define LP_TRIG_VAL(n)			(((n) >> 4) & 7)
#define PCTL_STAT_MSK			(7)
#define INIT_MEM			(0)
#define CONF				(1)
#define CONF_REQ			(2)
#define ACCESS				(3)
#define ACCESS_REQ			(4)
#define LOW_POWER			(5)
#define LOW_POWER_ENTRY_REQ		(6)
#define LOW_POWER_EXIT_REQ		(7)

/* ZQCR*/
#define PD_OUTPUT(n)			((n) << 0)
#define PU_OUTPUT(n)			((n) << 5)
#define PD_ONDIE(n)			((n) << 10)
#define PU_ONDIE(n)			((n) << 15)
#define ZDEN(n)				((n) << 28)

/* DDLGCR */
#define SBIAS_BYPASS			(1 << 23)

/* MCFG */
#define MDDR_LPDDR2_CLK_STOP_IDLE(n)	((n) << 24)
#define PD_IDLE(n)			((n) << 8)
#define MDDR_EN				(2 << 22)
#define LPDDR2_EN			(3 << 22)
#define DDR2_EN				(0 << 5)
#define DDR3_EN				(1 << 5)
#define LPDDR2_S2			(0 << 6)
#define LPDDR2_S4			(1 << 6)
#define MDDR_LPDDR2_BL_2		(0 << 20)
#define MDDR_LPDDR2_BL_4		(1 << 20)
#define MDDR_LPDDR2_BL_8		(2 << 20)
#define MDDR_LPDDR2_BL_16		(3 << 20)
#define DDR2_DDR3_BL_4			(0)
#define DDR2_DDR3_BL_8			(1)
#define TFAW_CFG(n)			(((n)-4) << 18)
#define PD_EXIT_SLOW			(0 << 17)
#define PD_EXIT_FAST			(1 << 17)
#define PD_TYPE(n)			((n) << 16)
#define BURSTLENGTH_CFG(n)		(((n) >> 1) << 20)

/* POWCTL */
#define POWER_UP_START			(1 << 0)

/* POWSTAT */
#define POWER_UP_DONE			(1 << 0)

/* MCMD */
#define DESELECT_CMD			(0)
#define PREA_CMD			(1)
#define REF_CMD				(2)
#define MRS_CMD				(3)
#define ZQCS_CMD			(4)
#define ZQCL_CMD			(5)
#define RSTL_CMD			(6)
#define MRR_CMD				(8)
#define DPDE_CMD			(9)

#define LPDDR2_MA(n)			(((n) & 0xff) << 4)
#define LPDDR2_OP(n)			(((n) & 0xff) << 12)

#define START_CMD			(1u << 31)

/* DEVTODEV */
#define BUSWRTORD(n)			((n) << 4)
#define BUSRDTOWR(n)			((n) << 2)
#define BUSRDTORD(n)			((n) << 0)

/* GRF_SOC_CON0 */
#define MSCH_MAINDDR3(ch, n)		(((n) << (3 + (ch))) \
					| ((1 << (3 + (ch))) << 16))

/* GRF_SOC_CON2 */
#define PCTL_LPDDR3_ODT_EN(ch, n) RK_CLRSETBITS(1 << (10 + (3 * (ch))), \
	(n) << (10 + (3 * (ch))))
#define PCTL_BST_DISABLE(ch, n) RK_CLRSETBITS(1 << (9 + (3 * (ch))), \
	(n) << (9 + (3 * (ch))))
#define PUBL_LPDDR3_EN(ch, n) RK_CLRSETBITS(1 << (8 + (3 * (ch))), \
	(n) << (8 + (3 * (ch))))

/* mr1 for ddr3 */
#define DDR3_DLL_ENABLE		(0)
#define DDR3_DLL_DISABLE	(1)

/*
 * sys_reg bitfield struct
 * [31] row_3_4_ch1
 * [30] row_3_4_ch0
 * [29:28] chinfo
 * [27] rank_ch1
 * [26:25] col_ch1
 * [24] bk_ch1
 * [23:22] cs0_row_ch1
 * [21:20] cs1_row_ch1
 * [19:18] bw_ch1
 * [17:16] dbw_ch1;
 * [15:13] ddrtype
 * [12] channelnum
 * [11] rank_ch0
 * [10:9] col_ch0
 * [8] bk_ch0
 * [7:6] cs0_row_ch0
 * [5:4] cs1_row_ch0
 * [3:2] bw_ch0
 * [1:0] dbw_ch0
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

static void copy_to_reg(u32 *dest, const u32 *src, u32 n)
{
	int i;
	for (i = 0; i < n / sizeof(u32); i++) {
		write32(dest, *src);
		src++;
		dest++;
	}
}

static void phy_pctrl_reset(struct rk3288_ddr_publ_regs *ddr_publ_regs,
			    u32 channel)
{
	int i;
	rkclk_ddr_reset(channel, 1, 1);
	udelay(1);
	clrbits32(&ddr_publ_regs->acdllcr, ACDLLCR_DLLSRST);
	for (i = 0; i < 4; i++)
		clrbits32(&ddr_publ_regs->datx8[i].dxdllcr, DXDLLCR_DLLSRST);

	udelay(10);
	setbits32(&ddr_publ_regs->acdllcr, ACDLLCR_DLLSRST);
	for (i = 0; i < 4; i++)
		setbits32(&ddr_publ_regs->datx8[i].dxdllcr, DXDLLCR_DLLSRST);

	udelay(10);
	rkclk_ddr_reset(channel, 1, 0);
	udelay(10);
	rkclk_ddr_reset(channel, 0, 0);
	udelay(10);
}

static void phy_dll_bypass_set(struct rk3288_ddr_publ_regs *ddr_publ_regs,
	u32 freq)
{
	int i;
	if (freq <= 250*MHz) {
		if (freq <= 150*MHz)
			clrbits32(&ddr_publ_regs->dllgcr, SBIAS_BYPASS);
		else
			setbits32(&ddr_publ_regs->dllgcr, SBIAS_BYPASS);
		setbits32(&ddr_publ_regs->acdllcr, ACDLLCR_DLLDIS);
		for (i = 0; i < 4; i++)
			setbits32(&ddr_publ_regs->datx8[i].dxdllcr,
				DXDLLCR_DLLDIS);

		setbits32(&ddr_publ_regs->pir, PIR_DLLBYP);
	} else {
		clrbits32(&ddr_publ_regs->dllgcr, SBIAS_BYPASS);
		clrbits32(&ddr_publ_regs->acdllcr, ACDLLCR_DLLDIS);
		for (i = 0; i < 4; i++)
			clrbits32(&ddr_publ_regs->datx8[i].dxdllcr,
				DXDLLCR_DLLDIS);

		clrbits32(&ddr_publ_regs->pir, PIR_DLLBYP);
	}
}

static void dfi_cfg(struct rk3288_ddr_pctl_regs *ddr_pctl_regs, u32 dramtype)
{
	write32(&ddr_pctl_regs->dfistcfg0, DFI_INIT_START);
	write32(&ddr_pctl_regs->dfistcfg1,
		DFI_DRAM_CLK_SR_EN | DFI_DRAM_CLK_DPD_EN);
	write32(&ddr_pctl_regs->dfistcfg2, DFI_PARITY_INTR_EN | DFI_PARITY_EN);
	write32(&ddr_pctl_regs->dfilpcfg0,
		TLP_RESP_TIME(7) | LP_SR_EN | LP_PD_EN);

	write32(&ddr_pctl_regs->dfitctrldelay, TCTRL_DELAY_TIME(2));
	write32(&ddr_pctl_regs->dfitphywrdata, TPHY_WRDATA_TIME(1));
	write32(&ddr_pctl_regs->dfitphyrdlat, TPHY_RDLAT_TIME(0xf));
	write32(&ddr_pctl_regs->dfitdramclkdis, TDRAM_CLK_DIS_TIME(2));
	write32(&ddr_pctl_regs->dfitdramclken, TDRAM_CLK_EN_TIME(2));
	write32(&ddr_pctl_regs->dfitphyupdtype0, 0x1);

	/* cs0 and cs1 write odt enable */
	write32(&ddr_pctl_regs->dfiodtcfg,
		(RANK0_ODT_WRITE_SEL | RANK1_ODT_WRITE_SEL));
	/* odt write length */
	write32(&ddr_pctl_regs->dfiodtcfg1, ODT_LEN_BL8_W(7));
	/* phyupd and ctrlupd disabled */
	write32(&ddr_pctl_regs->dfiupdcfg, 0);
}

static void pctl_cfg(u32 channel,
		     const struct rk3288_sdram_params *sdram_params)
{
	unsigned int burstlen;
	struct rk3288_ddr_pctl_regs *ddr_pctl_regs = rk3288_ddr_pctl[channel];
	burstlen = (sdram_params->noc_timing >> 18) & 0x7;
	copy_to_reg(&ddr_pctl_regs->togcnt1u,
			&(sdram_params->pctl_timing.togcnt1u),
			sizeof(sdram_params->pctl_timing));
	switch (sdram_params->dramtype) {
	case LPDDR3:
		write32(&ddr_pctl_regs->dfitrddataen,
			sdram_params->pctl_timing.tcl - 1);
		write32(&ddr_pctl_regs->dfitphywrlat,
			sdram_params->pctl_timing.tcwl);
		write32(&ddr_pctl_regs->mcfg, LPDDR2_S4 |
			MDDR_LPDDR2_CLK_STOP_IDLE(0) | LPDDR2_EN |
			BURSTLENGTH_CFG(burstlen) | TFAW_CFG(6) |
			PD_EXIT_FAST | PD_TYPE(1) | PD_IDLE(0));
		write32(&rk3288_grf->soc_con0, MSCH_MAINDDR3(channel, 0));

		write32(&rk3288_grf->soc_con2, PUBL_LPDDR3_EN(channel, 1) |
			PCTL_BST_DISABLE(channel, 1) |
			PCTL_LPDDR3_ODT_EN(channel, sdram_params->odt));

		break;
	case DDR3:
		if (sdram_params->phy_timing.mr[1] & DDR3_DLL_DISABLE)
			write32(&ddr_pctl_regs->dfitrddataen,
				sdram_params->pctl_timing.tcl - 3);
		else
			write32(&ddr_pctl_regs->dfitrddataen,
				sdram_params->pctl_timing.tcl - 2);
		write32(&ddr_pctl_regs->dfitphywrlat,
			sdram_params->pctl_timing.tcwl - 1);
		write32(&ddr_pctl_regs->mcfg,
			MDDR_LPDDR2_CLK_STOP_IDLE(0) | DDR3_EN |
			DDR2_DDR3_BL_8 | TFAW_CFG(6) |
			PD_EXIT_SLOW | PD_TYPE(1) | PD_IDLE(0));
		write32(&rk3288_grf->soc_con0, MSCH_MAINDDR3(channel, 1));

		write32(&rk3288_grf->soc_con2, PUBL_LPDDR3_EN(channel, 0) |
			PCTL_BST_DISABLE(channel, 0) |
			PCTL_LPDDR3_ODT_EN(channel, 0));

		break;
	}

	setbits32(&ddr_pctl_regs->scfg, 1);
}

static void phy_cfg(u32 channel, const struct rk3288_sdram_params *sdram_params)
{
	u32 i;
	u32 dinit2 = DIV_ROUND_UP(sdram_params->ddr_freq/MHz * 200000, 1000);
	struct rk3288_ddr_publ_regs *ddr_publ_regs = rk3288_ddr_publ[channel];
	struct rk3288_msch_regs *msch_regs = rk3288_msch[channel];

	/* DDR PHY Timing */
	copy_to_reg(&ddr_publ_regs->dtpr[0],
			&(sdram_params->phy_timing.dtpr0),
			sizeof(sdram_params->phy_timing));
	write32(&msch_regs->ddrtiming, sdram_params->noc_timing);
	write32(&msch_regs->readlatency, 0x3f);
	write32(&msch_regs->activate, sdram_params->noc_activate);
	write32(&msch_regs->devtodev,
		BUSWRTORD(2) | BUSRDTOWR(2) | BUSRDTORD(1));
	write32(&ddr_publ_regs->ptr[0],
	   PRT_DLLLOCK(DIV_ROUND_UP(sdram_params->ddr_freq / MHz * 5120, 1000))
	 | PRT_DLLSRST(DIV_ROUND_UP(sdram_params->ddr_freq / MHz * 50, 1000))
	 | PRT_ITMSRST(8));
	write32(&ddr_publ_regs->ptr[1],
	   PRT_DINIT0(DIV_ROUND_UP(sdram_params->ddr_freq / MHz * 500000, 1000))
	 | PRT_DINIT1(DIV_ROUND_UP(sdram_params->ddr_freq / MHz * 400, 1000)));
	write32(&ddr_publ_regs->ptr[2], PRT_DINIT2(MIN(dinit2, 0x1ffff))
	 | PRT_DINIT3(DIV_ROUND_UP(sdram_params->ddr_freq / MHz * 1000, 1000)));

	switch (sdram_params->dramtype) {
	case LPDDR3:
		clrsetbits32(&ddr_publ_regs->pgcr, 0x1F, PGCR_DFTLMT(0)
		       | PGCR_DFTCMP(0) | PGCR_DQSCFG(1) | PGCR_ITMDMD(0));
		/* DDRMODE select LPDDR3 */
		clrsetbits32(&ddr_publ_regs->dcr, DDRMD_MSK,
			DDRMD_CFG(DDRMD_LPDDR2_LPDDR3));
		clrsetbits32(&ddr_publ_regs->dxccr, DQSNRES_MSK | DQSRES_MSK,
				DQSRES_CFG(4) | DQSNRES_CFG(0xc));
		i = TDQSCKMAX_VAL(read32(&ddr_publ_regs->dtpr[1]))
		    - TDQSCK_VAL(read32(&ddr_publ_regs->dtpr[1]));
		clrsetbits32(&ddr_publ_regs->dsgcr, DQSGE_MSK | DQSGX_MSK,
				DQSGE_CFG(i) | DQSGX_CFG(i));
		break;
	case DDR3:
		clrbits32(&ddr_publ_regs->pgcr, 0x1f);
		clrsetbits32(&ddr_publ_regs->dcr, DDRMD_MSK,
			DDRMD_CFG(DDRMD_DDR3));
		break;
	}
	if (sdram_params->odt) {
		/*dynamic RTT enable */
		for (i = 0; i < 4; i++)
			setbits32(&ddr_publ_regs->datx8[i].dxgcr,
				DQSRTT | DQRTT);
	} else {
		/*dynamic RTT disable */
		for (i = 0; i < 4; i++)
			clrbits32(&ddr_publ_regs->datx8[i].dxgcr,
				DQSRTT | DQRTT);

	}
}

static void phy_init(struct rk3288_ddr_publ_regs *ddr_publ_regs)
{
	setbits32(&ddr_publ_regs->pir, PIR_INIT | PIR_DLLSRST
		| PIR_DLLLOCK | PIR_ZCAL | PIR_ITMSRST | PIR_CLRSR);
	udelay(1);
	while ((read32(&ddr_publ_regs->pgsr) &
		(PGSR_IDONE | PGSR_DLDONE | PGSR_ZCDONE)) !=
		(PGSR_IDONE | PGSR_DLDONE | PGSR_ZCDONE))
		;
}

static void send_command(struct rk3288_ddr_pctl_regs *ddr_pctl_regs, u32 rank,
			 u32 cmd, u32 arg)
{
	write32(&ddr_pctl_regs->mcmd, (START_CMD | (rank << 20) | arg | cmd));
	udelay(1);
	while (read32(&ddr_pctl_regs->mcmd) & START_CMD)
		;
}

static void memory_init(struct rk3288_ddr_publ_regs *ddr_publ_regs,
			u32 dramtype)
{
	setbits32(&ddr_publ_regs->pir,
		  (PIR_INIT | PIR_DRAMINIT | PIR_LOCKBYP
		   | PIR_ZCALBYP | PIR_CLRSR | PIR_ICPC
		   | (dramtype == DDR3 ? PIR_DRAMRST : 0)));
	udelay(1);
	while ((read32(&ddr_publ_regs->pgsr) & (PGSR_IDONE | PGSR_DLDONE))
		!= (PGSR_IDONE | PGSR_DLDONE))
		;
}

static void move_to_config_state(struct rk3288_ddr_publ_regs *ddr_publ_regs,
				 struct rk3288_ddr_pctl_regs *ddr_pctl_regs)
{
	unsigned int state;

	while (1) {
		state = read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK;

		switch (state) {
		case LOW_POWER:
			write32(&ddr_pctl_regs->sctl, WAKEUP_STATE);
			while ((read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK)
				!= ACCESS)
				;
			/* wait DLL lock */
			while ((read32(&ddr_publ_regs->pgsr) & PGSR_DLDONE)
				!= PGSR_DLDONE)
				;
			/* if at low power state, need wakeup first, then enter the config */
			/* fall through */
		case ACCESS:
		case INIT_MEM:
			write32(&ddr_pctl_regs->sctl, CFG_STATE);
			while ((read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK)
				!= CONF)
				;
			break;
		case CONF:
			return;
		default:
			break;
		}
	}
}

static void set_bandwidth_ratio(u32 channel, u32 n)
{
	struct rk3288_ddr_pctl_regs *ddr_pctl_regs = rk3288_ddr_pctl[channel];
	struct rk3288_ddr_publ_regs *ddr_publ_regs = rk3288_ddr_publ[channel];
	struct rk3288_msch_regs *msch_regs = rk3288_msch[channel];

	if (n == 1) {
		setbits32(&ddr_pctl_regs->ppcfg, 1);
		write32(&rk3288_grf->soc_con0, RK_SETBITS(1 << (8 + channel)));
		setbits32(&msch_regs->ddrtiming, 1 << 31);
		/* Data Byte disable*/
		clrbits32(&ddr_publ_regs->datx8[2].dxgcr, 1);
		clrbits32(&ddr_publ_regs->datx8[3].dxgcr, 1);
		/*disable DLL */
		setbits32(&ddr_publ_regs->datx8[2].dxdllcr,
			DXDLLCR_DLLDIS);
		setbits32(&ddr_publ_regs->datx8[3].dxdllcr,
			DXDLLCR_DLLDIS);
	} else {
		clrbits32(&ddr_pctl_regs->ppcfg, 1);
		write32(&rk3288_grf->soc_con0, RK_CLRBITS(1 << (8 + channel)));
		clrbits32(&msch_regs->ddrtiming, 1 << 31);
		/* Data Byte enable*/
		setbits32(&ddr_publ_regs->datx8[2].dxgcr, 1);
		setbits32(&ddr_publ_regs->datx8[3].dxgcr, 1);

		/*enable DLL */
		clrbits32(&ddr_publ_regs->datx8[2].dxdllcr,
			DXDLLCR_DLLDIS);
		clrbits32(&ddr_publ_regs->datx8[3].dxdllcr,
			DXDLLCR_DLLDIS);
		/* reset DLL */
		clrbits32(&ddr_publ_regs->datx8[2].dxdllcr,
			DXDLLCR_DLLSRST);
		clrbits32(&ddr_publ_regs->datx8[3].dxdllcr,
			DXDLLCR_DLLSRST);
		udelay(10);
		setbits32(&ddr_publ_regs->datx8[2].dxdllcr,
			DXDLLCR_DLLSRST);
		setbits32(&ddr_publ_regs->datx8[3].dxdllcr,
			DXDLLCR_DLLSRST);
	}
	setbits32(&ddr_pctl_regs->dfistcfg0, 1 << 2);

}

static int data_training(u32 channel,
	const struct rk3288_sdram_params *sdram_params)
{
	unsigned int j;
	int ret = 0;
	u32 rank;
	int i;
	u32 step[2] = { PIR_QSTRN, PIR_RVTRN };
	struct rk3288_ddr_publ_regs *ddr_publ_regs = rk3288_ddr_publ[channel];
	struct rk3288_ddr_pctl_regs *ddr_pctl_regs = rk3288_ddr_pctl[channel];

	/* disable auto refresh */
	write32(&ddr_pctl_regs->trefi, 0);

	if (sdram_params->dramtype != LPDDR3)
		setbits32(&ddr_publ_regs->pgcr, PGCR_DQSCFG(1));
	rank = sdram_params->ch[channel].rank | 1;
	for (j = 0; j < ARRAY_SIZE(step); j++) {
		/*
		 * trigger QSTRN and RVTRN
		 * clear DTDONE status
		 */
		setbits32(&ddr_publ_regs->pir, PIR_CLRSR);

		/* trigger DTT */
		setbits32(&ddr_publ_regs->pir,
			  PIR_INIT | step[j] | PIR_LOCKBYP | PIR_ZCALBYP |
			  PIR_CLRSR);
		udelay(1);
		/* wait echo byte DTDONE */
		while ((read32(&ddr_publ_regs->datx8[0].dxgsr[0]) & rank)
			!= rank)
			;
		while ((read32(&ddr_publ_regs->datx8[1].dxgsr[0]) & rank)
			!= rank)
			;
		if (!(read32(&ddr_pctl_regs->ppcfg) & 1)) {
			while ((read32(&ddr_publ_regs->datx8[2].dxgsr[0])
				& rank) != rank)
				;
			while ((read32(&ddr_publ_regs->datx8[3].dxgsr[0])
				& rank) != rank)
				;
		}
		if (read32(&ddr_publ_regs->pgsr) &
		    (PGSR_DTERR | PGSR_RVERR | PGSR_RVEIRR)) {
			ret = -1;
			break;
		}
	}
	/* send some auto refresh to complement the lost while DTT */
	for (i = 0; i < (rank > 1 ? 8 : 4); i++)
		send_command(ddr_pctl_regs, rank, REF_CMD, 0);

	if (sdram_params->dramtype != LPDDR3)
		clrbits32(&ddr_publ_regs->pgcr, PGCR_DQSCFG(1));

	/* resume auto refresh */
	write32(&ddr_pctl_regs->trefi, sdram_params->pctl_timing.trefi);

	return ret;
}

static void move_to_access_state(u32 chnum)
{
	struct rk3288_ddr_publ_regs *ddr_publ_regs = rk3288_ddr_publ[chnum];
	struct rk3288_ddr_pctl_regs *ddr_pctl_regs = rk3288_ddr_pctl[chnum];

	unsigned int state;

	while (1) {
		state = read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK;

		switch (state) {
		case LOW_POWER:
			if (LP_TRIG_VAL(read32(&ddr_pctl_regs->stat)) == 1)
				return;

			write32(&ddr_pctl_regs->sctl, WAKEUP_STATE);
			while ((read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK)
				!= ACCESS)
				;
			/* wait DLL lock */
			while ((read32(&ddr_publ_regs->pgsr) & PGSR_DLDONE)
				!= PGSR_DLDONE)
				;
			break;
		case INIT_MEM:
			write32(&ddr_pctl_regs->sctl, CFG_STATE);
			while ((read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK)
				!= CONF)
				;
			/* fall through - enter config next to get to access state */
		case CONF:
			write32(&ddr_pctl_regs->sctl, GO_STATE);
			while ((read32(&ddr_pctl_regs->stat) & PCTL_STAT_MSK)
				== CONF)
				;
			break;
		case ACCESS:
			return;
		default:
			break;
		}
	}
}

static void dram_cfg_rbc(u32 chnum,
	const struct rk3288_sdram_params *sdram_params)
{
	struct rk3288_ddr_publ_regs *ddr_publ_regs = rk3288_ddr_publ[chnum];
	struct rk3288_msch_regs *msch_regs = rk3288_msch[chnum];

	if (sdram_params->ch[chnum].bk == 3)
		clrsetbits32(&ddr_publ_regs->dcr, PDQ_MSK, PDQ_CFG(1));
	else
		clrbits32(&ddr_publ_regs->dcr, PDQ_MSK);

	write32(&msch_regs->ddrconf, sdram_params->ddrconfig);
}

static void dram_all_config(const struct rk3288_sdram_params *sdram_params)
{
	u32 sys_reg = 0;
	unsigned int channel;

	sys_reg |= SYS_REG_ENC_DDRTYPE(sdram_params->dramtype);
	sys_reg |= SYS_REG_ENC_NUM_CH(sdram_params->num_channels);
	for (channel = 0; channel < sdram_params->num_channels; channel++) {
		const struct rk3288_sdram_channel *info =
			&(sdram_params->ch[channel]);
		sys_reg |= SYS_REG_ENC_ROW_3_4(info->row_3_4, channel);
		sys_reg |= SYS_REG_ENC_CHINFO(channel);
		sys_reg |= SYS_REG_ENC_RANK(info->rank, channel);
		sys_reg |= SYS_REG_ENC_COL(info->col, channel);
		sys_reg |= SYS_REG_ENC_BK(info->bk, channel);
		sys_reg |= SYS_REG_ENC_CS0_ROW(info->cs0_row, channel);
		sys_reg |= SYS_REG_ENC_CS1_ROW(info->cs1_row, channel);
		sys_reg |= SYS_REG_ENC_BW(info->bw, channel);
		sys_reg |= SYS_REG_ENC_DBW(info->dbw, channel);

		dram_cfg_rbc(channel, sdram_params);
	}
	write32(&rk3288_pmu->sys_reg[2], sys_reg);
	write32(&rk3288_sgrf->soc_con2,
		RK_CLRSETBITS(0x1F, sdram_params->stride));
}

void sdram_init(const struct rk3288_sdram_params *sdram_params)
{
	int channel;
	int zqcr;
	printk(BIOS_INFO, "Starting SDRAM initialization...\n");

	if ((sdram_params->dramtype == DDR3
		&& sdram_params->ddr_freq > 800*MHz)
		|| (sdram_params->dramtype == LPDDR3
		&& sdram_params->ddr_freq > 533*MHz))
		die("SDRAM frequency is to high!");

	rkclk_configure_ddr(sdram_params->ddr_freq);

	for (channel = 0; channel < 2; channel++) {
		struct rk3288_ddr_pctl_regs *ddr_pctl_regs =
		    rk3288_ddr_pctl[channel];
		struct rk3288_ddr_publ_regs *ddr_publ_regs =
		    rk3288_ddr_publ[channel];

		phy_pctrl_reset(ddr_publ_regs, channel);
		phy_dll_bypass_set(ddr_publ_regs, sdram_params->ddr_freq);

		if (channel >= sdram_params->num_channels)
			continue;

		dfi_cfg(ddr_pctl_regs, sdram_params->dramtype);

		pctl_cfg(channel, sdram_params);

		phy_cfg(channel, sdram_params);

		phy_init(ddr_publ_regs);

		write32(&ddr_pctl_regs->powctl, POWER_UP_START);
		while (!(read32(&ddr_pctl_regs->powstat) & POWER_UP_DONE))
			;

		memory_init(ddr_publ_regs, sdram_params->dramtype);
		move_to_config_state(ddr_publ_regs, ddr_pctl_regs);

		if (sdram_params->dramtype == LPDDR3) {
			send_command(ddr_pctl_regs, 3, DESELECT_CMD, 0);
			udelay(1);
			send_command(ddr_pctl_regs, 3, PREA_CMD, 0);
			udelay(1);
			send_command(ddr_pctl_regs, 3, MRS_CMD, LPDDR2_MA(63) |
				LPDDR2_OP(0xFC));
			udelay(1);
			send_command(ddr_pctl_regs, 3, MRS_CMD, LPDDR2_MA(1) |
				LPDDR2_OP(sdram_params->phy_timing.mr[1]));
			udelay(1);
			send_command(ddr_pctl_regs, 3, MRS_CMD, LPDDR2_MA(2) |
				LPDDR2_OP(sdram_params->phy_timing.mr[2]));
			udelay(1);
			send_command(ddr_pctl_regs, 3, MRS_CMD, LPDDR2_MA(3) |
				LPDDR2_OP(sdram_params->phy_timing.mr[3]));
			udelay(1);
		}

		set_bandwidth_ratio(channel, sdram_params->ch[channel].bw);
		/*
		 * set cs
		 * CS0, n=1
		 * CS1, n=2
		 * CS0 & CS1, n = 3
		 */
		clrsetbits32(&ddr_publ_regs->pgcr, 0xF << 18,
			     (sdram_params->ch[channel].rank | 1) << 18);
		/* DS=40ohm,ODT=155ohm */
		zqcr = ZDEN(1) | PU_ONDIE(0x2) | PD_ONDIE(0x2)
				| PU_OUTPUT(0x19) | PD_OUTPUT(0x19);
		write32(&ddr_publ_regs->zq1cr[0], zqcr);
		write32(&ddr_publ_regs->zq0cr[0], zqcr);

		if (sdram_params->dramtype == LPDDR3) {
			/* LPDDR2/LPDDR3 need to wait DAI complete, max 10us */
			udelay(10);
			send_command(ddr_pctl_regs,
				(sdram_params->ch[channel].rank | 1),
				MRS_CMD, LPDDR2_MA(11) | (sdram_params->odt ?
				LPDDR2_OP(0x3) : LPDDR2_OP(0x0)));
			if (channel == 0) {
				write32(&ddr_pctl_regs->mrrcfg0, 0);
				send_command(ddr_pctl_regs, 1, MRR_CMD,
					LPDDR2_MA(0x8));
				/* S8 */
				if ((read32(&ddr_pctl_regs->mrrstat0) & 0x3)
					!= 3)
					die("SDRAM initialization failed!");
			}
		}

		if (-1 == data_training(channel, sdram_params)) {
			if (sdram_params->dramtype == LPDDR3) {
				rkclk_ddr_phy_ctl_reset(channel, 1);
				udelay(10);
				rkclk_ddr_phy_ctl_reset(channel, 0);
				udelay(10);
			}
			die("SDRAM initialization failed!");
		}

		if (sdram_params->dramtype == LPDDR3) {
			u32 i;
			write32(&ddr_pctl_regs->mrrcfg0, 0);
			for (i = 0; i < 17; i++)
				send_command(ddr_pctl_regs, 1, MRR_CMD,
					LPDDR2_MA(i));
		}
		move_to_access_state(channel);
	}
	dram_all_config(sdram_params);
	printk(BIOS_INFO, "Finish SDRAM initialization...\n");
}

size_t sdram_size_mb(void)
{
	u32 rank, col, bk, cs0_row, cs1_row, bw, row_3_4;
	size_t chipsize_mb = 0;
	static size_t size_mb = 0;
	u32 ch;

	if (!size_mb) {

		u32 sys_reg = read32(&rk3288_pmu->sys_reg[2]);
		u32 ch_num = SYS_REG_DEC_NUM_CH(sys_reg);

		for (ch = 0; ch < ch_num; ch++) {
			rank = SYS_REG_DEC_RANK(sys_reg, ch);
			col = SYS_REG_DEC_COL(sys_reg, ch);
			bk = SYS_REG_DEC_BK(sys_reg, ch);
			cs0_row = SYS_REG_DEC_CS0_ROW(sys_reg, ch);
			cs1_row = SYS_REG_DEC_CS1_ROW(sys_reg, ch);
			bw = SYS_REG_DEC_BW(sys_reg, ch);
			row_3_4 = SYS_REG_DEC_ROW_3_4(sys_reg, ch);

			chipsize_mb = (1 << (cs0_row + col + bk + bw - 20));

			if (rank > 1)
				chipsize_mb += chipsize_mb >>
					(cs0_row - cs1_row);
			if (row_3_4)
				chipsize_mb = chipsize_mb * 3 / 4;
			size_mb += chipsize_mb;
		}

		/*
		 * we use the 0x00000000~0xfeffffff space
		 * since 0xff000000~0xffffffff is soc register space
		 * so we reserve it
		 */
		size_mb = MIN(size_mb, 0xff000000/MiB);
	}

	return size_mb;
}
