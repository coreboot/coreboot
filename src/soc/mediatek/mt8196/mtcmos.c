/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * This file is created based on MTCMOS_SW_NOTE.xlsx
 */

#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/mtcmos.h>
#include <soc/pll.h>
#include <soc/spm_mtcmos.h>
#include <timer.h>

#define MTK_SCPD_CAPS(_scpd, _x)	((_scpd)->caps & (_x))

#define POWERON_CONFIG_EN	((void *)(SPM_BASE + 0x0000))

#define SPM_PROJECT_CODE	0xB16

#define SET_BUS_PROTECT		1
#define RELEASE_BUS_PROTECT	0

#define WAIT_VOTE_TIMEOUT	20000

#define SPM_PBUS_BUS_PROTECT_EN_SET		((void *)(SPM_PBUS_BASE + 0x00DC))
#define SPM_PBUS_BUS_PROTECT_EN_CLR		((void *)(SPM_PBUS_BASE + 0x00E0))
#define SPM_PBUS_BUS_PROTECT_RDY_STA		((void *)(SPM_PBUS_BASE + 0x0208))
#define SPM_PBUS_BUS_MSB_PROTECT_EN_SET		((void *)(SPM_PBUS_BASE + 0x00E8))
#define SPM_PBUS_BUS_MSB_PROTECT_EN_CLR		((void *)(SPM_PBUS_BASE + 0x00EC))
#define SPM_PBUS_BUS_MSB_PROTECT_RDY_STA	((void *)(SPM_PBUS_BASE + 0x020C))

#define MM_BUCK_ISO_CON_CLR		((void *)(MMPC_BASE + 0x00C8))
#define DISP_AO_PWR_CON			((void *)(MMPC_BASE + 0x00E8))
#define MMPC_BUS_PROTECT_EN_1_CLR	((void *)(MMPC_BASE + 0x0188))

#define MMVOTE_MTCMOS_0_SET		((void *)(MMVOTE_BASE + 0x0218))
#define MMVOTE_MTCMOS_0_CLR		((void *)(MMVOTE_BASE + 0x021C))
#define MMVOTE_MTCMOS_0_DONE		((void *)(MMVOTE_BASE + 0x141C))
#define MMVOTE_MTCMOS_0_PM_ACK		((void *)(MMVOTE_BASE + 0x5514))
#define MMVOTE_MTCMOS_1_SET		((void *)(MMVOTE_BASE + 0x0220))
#define MMVOTE_MTCMOS_1_CLR		((void *)(MMVOTE_BASE + 0x0224))
#define MMVOTE_MTCMOS_1_DONE		((void *)(MMVOTE_BASE + 0x142C))
#define MMVOTE_MTCMOS_1_PM_ACK		((void *)(MMVOTE_BASE + 0x5518))
#define MMINFRA_MTCMOS_1_SET		((void *)(MMVOTE_BASE + 0x43220))
#define MMINFRA_MTCMOS_1_CLR		((void *)(MMVOTE_BASE + 0x43224))

#define DISP_VDISP_AO_CONFIG_CG		0x3E800108
#define DISP_DPC_DISP0_MTCMOS_CFG	0x3E8F0500
#define DISP_DPC_DISP1_MTCMOS_CFG	0x3E8F0580
#define DISP_DPC_OVL0_MTCMOS_CFG	0x3E8F0600
#define DISP_DPC_OVL1_MTCMOS_CFG	0x3E8F0680
#define DISP_DPC_MML0_MTCMOS_CFG	0x3E8F0B00
#define DISP_DPC_MML1_MTCMOS_CFG	0x3E8F0700
#define DISP_DPC_EDP_MTCMOS_CFG		0x3E8F0C00
#define DISP_DPC_EPTX_MTCMOS_CFG	0x3E8F0D00

#define MMINFRA0_VOTE_BIT	1
#define MMINFRA1_VOTE_BIT	2
#define MMINFRA_AO_VOTE_BIT	3
#define DSI_PHY0_VOTE_BIT	7
#define DSI_PHY1_VOTE_BIT	8
#define DSI_PHY2_VOTE_BIT	9
#define DISP_VCORE_VOTE_BIT	24
#define DISP0_VOTE_BIT		25
#define DISP1_VOTE_BIT		26
#define OVL0_VOTE_BIT		27
#define OVL1_VOTE_BIT		28
#define DISP_EDPTX_VOTE_BIT	29
#define DISP_DPTX_VOTE_BIT	30

#define MTK_SCPD_SRAM_ISO	BIT(0)
#define MTK_SCPD_SRAM_SLP	BIT(2)
#define MTK_SCPD_RTFF_DELAY	BIT(6)
#define MTK_SCPD_NO_SRAM	BIT(8)

#define PWR_RST_B		BIT(0)
#define PWR_ISO			BIT(1)
#define PWR_ON			BIT(2)
#define PWR_ON_2ND		BIT(3)
#define PWR_CLK_DIS		BIT(4)
#define SRAM_CKISO		BIT(5)
#define SRAM_ISOINT_B		BIT(6)
#define SRAM_PDN		BIT(8)
#define SRAM_SLP		BIT(9)
#define SRAM_PDN_ACK		BIT(12)
#define SRAM_SLP_ACK		BIT(13)
#define PWR_RTFF_SAVE		BIT(24)
#define PWR_RTFF_SAVE_FLAG	BIT(27)
#define PWR_ACK			BIT(30)
#define PWR_ACK_2ND		BIT(31)

#define ACK_DELAY_US		10
#define ACK_DELAY_TIMES		10000

#define ADSP_AO_PROT_STEP1_SHIFT		23
#define ADSP_INFRA_PROT_STEP1_SHIFT		22
#define ADSP_TOP_PROT_STEP1_SHIFT		21
#define AUDIO_PROT_STEP1_SHIFT			19
#define MM_PROC_PROT_STEP1_SHIFT		24
#define PEXTP_MAC0_PROT_STEP1_SHIFT		13
#define PEXTP_MAC1_PROT_STEP1_SHIFT		14
#define PEXTP_MAC2_PROT_STEP1_SHIFT		15
#define PEXTP_PHY0_PROT_STEP1_SHIFT		16
#define PEXTP_PHY1_PROT_STEP1_SHIFT		17
#define PEXTP_PHY2_PROT_STEP1_SHIFT		18
#define SSRSYS_PROT_STEP1_SHIFT			10
#define SSUSB_DP_PHY_P0_PROT_STEP1_SHIFT	6
#define SSUSB_P0_PROT_STEP1_SHIFT		7
#define SSUSB_P1_PROT_STEP1_SHIFT		8
#define SSUSB_P23_PROT_STEP1_SHIFT		9
#define SSUSB_PHY_P2_PROT_STEP1_SHIFT		10
#define UFS0_PHY_PROT_STEP1_SHIFT		12
#define UFS0_PROT_STEP1_SHIFT			11

struct mtcmos_bus_prot {
	u32 *bp_en_set;
	u32 *bp_en_clr;
	u32 *bp_rdy;
	u8 bit;
};

struct mtcmos_data {
	const struct mtcmos_bus_prot *bp_table;
	u32 *ctl_addr;
	u32 caps;
	u32 bp_num;
	const struct mtcmos_cb *cb;
};

struct mtcmos_vote_data {
	u32 *set_addr;
	u32 *clr_addr;
	u32 *done_addr;
	u32 *ack_addr;
	u8 vote_bit;
	const struct mtcmos_cb *cb;
};

static const struct mtcmos_bus_prot ssusb_dp_phy_p0_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, SSUSB_DP_PHY_P0_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ssusb_p0_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, SSUSB_P0_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ssusb_p1_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, SSUSB_P1_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ssusb_p23_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, SSUSB_P23_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ssusb_phy_p2_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, SSUSB_PHY_P2_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ufs0_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, UFS0_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ufs0_phy_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, UFS0_PHY_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_mac0_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_MAC0_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_mac1_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_MAC1_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_mac2_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_MAC2_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_phy0_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_PHY0_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_phy1_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_PHY1_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot pextp_phy2_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, PEXTP_PHY2_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot audio_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, AUDIO_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot adsp_top_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, ADSP_TOP_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot adsp_infra_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, ADSP_INFRA_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot adsp_ao_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, ADSP_AO_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot mm_proc_bp[] = {
	{ SPM_PBUS_BUS_PROTECT_EN_SET, SPM_PBUS_BUS_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_PROTECT_RDY_STA, MM_PROC_PROT_STEP1_SHIFT },
};

static const struct mtcmos_bus_prot ssrsys_bp[] = {
	{ SPM_PBUS_BUS_MSB_PROTECT_EN_SET, SPM_PBUS_BUS_MSB_PROTECT_EN_CLR,
	  SPM_PBUS_BUS_MSB_PROTECT_RDY_STA, SSRSYS_PROT_STEP1_SHIFT },
};

static struct mtcmos_data mds[MTCMOS_ID_NUM] = {
	[MTCMOS_ID_SSUSB_DP_PHY_P0] = {
		ssusb_dp_phy_p0_bp, &mtk_spm_mtcmos->ssusb_dp_phy_p0_pwr_con,
		MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_SSUSB_P0] = {
		ssusb_p0_bp, &mtk_spm_mtcmos->ssusb_p0_pwr_con, 0, 1
	},
	[MTCMOS_ID_SSUSB_P1] = {
		ssusb_p1_bp, &mtk_spm_mtcmos->ssusb_p1_pwr_con, 0, 1
	},
	[MTCMOS_ID_SSUSB_P23] = {
		ssusb_p23_bp, &mtk_spm_mtcmos->ssusb_p23_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_SSUSB_PHY_P2] = {
		ssusb_phy_p2_bp, &mtk_spm_mtcmos->ssusb_phy_p2_pwr_con, 0, 1
	},
	[MTCMOS_ID_UFS0_SHUTDOWN] = {
		ufs0_bp, &mtk_spm_mtcmos->ufs0_pwr_con, 0, 1
	},
	[MTCMOS_ID_UFS0_PHY] = {
		ufs0_phy_bp, &mtk_spm_mtcmos->ufs0_phy_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_PEXTP_MAC0] = {
		pextp_mac0_bp, &mtk_spm_mtcmos->pextp_mac0_pwr_con, 0, 1
	},
	[MTCMOS_ID_PEXTP_MAC1] = {
		pextp_mac1_bp, &mtk_spm_mtcmos->pextp_mac1_pwr_con, 0, 1
	},
	[MTCMOS_ID_PEXTP_MAC2] = {
		pextp_mac2_bp, &mtk_spm_mtcmos->pextp_mac2_pwr_con, 0, 1
	},
	[MTCMOS_ID_PEXTP_PHY0] = {
		pextp_phy0_bp, &mtk_spm_mtcmos->pextp_phy0_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_PEXTP_PHY1] = {
		pextp_phy1_bp, &mtk_spm_mtcmos->pextp_phy1_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_PEXTP_PHY2] = {
		pextp_phy2_bp, &mtk_spm_mtcmos->pextp_phy2_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_AUDIO] = {
		audio_bp, &mtk_spm_mtcmos->audio_pwr_con, 0, 1
	},
	[MTCMOS_ID_ADSP_TOP_SHUTDOWN] = {
		adsp_top_bp, &mtk_spm_mtcmos->adsp_top_pwr_con, 0, 1
	},
	[MTCMOS_ID_ADSP_INFRA] = {
		adsp_infra_bp, &mtk_spm_mtcmos->adsp_infra_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_ADSP_AO] = {
		adsp_ao_bp, &mtk_spm_mtcmos->adsp_ao_pwr_con, MTK_SCPD_NO_SRAM, 1
	},
	[MTCMOS_ID_MM_PROC_SHUTDOWN] = {
		mm_proc_bp, &mtk_spm_mtcmos->mm_proc_pwr_con, 0, 1
	},
	[MTCMOS_ID_SSRSYS] = {
		ssrsys_bp, &mtk_spm_mtcmos->ssrsys_pwr_con, 0, 1
	},
};

static struct mtcmos_vote_data vote_mds[MTCMOS_ID_NUM] = {
	[MTCMOS_ID_DISP_VCORE] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, DISP_VCORE_VOTE_BIT
	},
	[MTCMOS_ID_DIS0_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, DISP0_VOTE_BIT
	},
	[MTCMOS_ID_DIS1_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, DISP1_VOTE_BIT
	},
	[MTCMOS_ID_OVL0_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, OVL0_VOTE_BIT
	},
	[MTCMOS_ID_OVL1_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, OVL1_VOTE_BIT
	},
	[MTCMOS_ID_DISP_EDPTX_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, DISP_EDPTX_VOTE_BIT
	},
	[MTCMOS_ID_DISP_DPTX_SHUTDOWN] = {
		MMVOTE_MTCMOS_0_SET, MMVOTE_MTCMOS_0_CLR, MMVOTE_MTCMOS_0_DONE,
		MMVOTE_MTCMOS_0_PM_ACK, DISP_DPTX_VOTE_BIT
	},
	[MTCMOS_ID_MM_INFRA0] = {
		MMINFRA_MTCMOS_1_SET, MMINFRA_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, MMINFRA0_VOTE_BIT
	},
	[MTCMOS_ID_MM_INFRA1] = {
		MMINFRA_MTCMOS_1_SET, MMINFRA_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, MMINFRA1_VOTE_BIT
	},
	[MTCMOS_ID_MM_INFRA_AO] = {
		MMINFRA_MTCMOS_1_SET, MMINFRA_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, MMINFRA_AO_VOTE_BIT
	},
	[MTCMOS_ID_DSI_PHY0] = {
		MMVOTE_MTCMOS_1_SET, MMVOTE_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, DSI_PHY0_VOTE_BIT
	},
	[MTCMOS_ID_DSI_PHY1] = {
		MMVOTE_MTCMOS_1_SET, MMVOTE_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, DSI_PHY1_VOTE_BIT
	},
	[MTCMOS_ID_DSI_PHY2] = {
		MMVOTE_MTCMOS_1_SET, MMVOTE_MTCMOS_1_CLR, MMVOTE_MTCMOS_1_DONE,
		MMVOTE_MTCMOS_1_PM_ACK, DSI_PHY2_VOTE_BIT
	},
};

int mtcmos_cb_register(enum mtcmos_id id, const struct mtcmos_cb *cb)
{
	if (id >= MTCMOS_ID_NUM) {
		printk(BIOS_ERR, "id(%d) is invalid\n", id);
		return -1;
	}

	if (id <= MTCMOS_ID_SSRSYS)
		mds[id].cb = cb;
	else
		vote_mds[id].cb = cb;

	return 0;
}

static int mtcmos_wait_for_state(u32 *reg, u32 mask, bool onoff)
{
	u32 expect = onoff ? mask : 0;

	if (!retry(ACK_DELAY_TIMES, (read32(reg) & mask) == expect, udelay(ACK_DELAY_US))) {
		printk(BIOS_ERR, "%s(%p, %#x, %d) reg_val=%#x timeout\n",
		       __func__, reg, mask, onoff, read32(reg));
		return -1;
	}

	return 0;
}

static int mtcmos_bus_prot_ctrl(const struct mtcmos_bus_prot *bp_table, bool set)
{
	u32 *ctrl_addr;
	u32 mask = BIT(bp_table->bit);

	if (set)
		ctrl_addr = bp_table->bp_en_set;
	else
		ctrl_addr = bp_table->bp_en_clr;

	write32(ctrl_addr, mask);

	if (set)
		return mtcmos_wait_for_state(bp_table->bp_rdy, mask, set);

	return 0;
}

static int mtcmos_setclr_bus_prot(const struct mtcmos_bus_prot *bp_table, u32 bp_num, bool set)
{
	int step_idx;
	int ret;
	int i;

	for (i = 0; i < bp_num; i++) {
		step_idx = set ? i : bp_num - 1 - i;
		ret = mtcmos_bus_prot_ctrl(&bp_table[step_idx], set);
		if (ret) {
			printk(BIOS_ERR, "mtcmos_bus_prot_ctrl set fail: %d\n", ret);
			mtcmos_bus_prot_ctrl(&bp_table[step_idx], !set);
			return ret;
		}
	}

	return 0;
}

static int mtcmos_sram_on(enum mtcmos_id id, const struct mtcmos_data *md)
{
	u32 *ctl_addr = md->ctl_addr;
	int ret = 0;

	if (MTK_SCPD_CAPS(md, MTK_SCPD_SRAM_SLP)) {
		setbits32(ctl_addr, SRAM_SLP);
		ret = mtcmos_wait_for_state(ctl_addr, SRAM_SLP_ACK, true);
	} else {
		clrbits32(ctl_addr, SRAM_PDN);
		ret = mtcmos_wait_for_state(ctl_addr, SRAM_PDN_ACK, false);
	}
	if (ret)
		return ret;

	if (MTK_SCPD_CAPS(md, MTK_SCPD_SRAM_ISO)) {
		setbits32(ctl_addr, SRAM_ISOINT_B);
		udelay(1);
		clrbits32(ctl_addr, SRAM_CKISO);
	}

	return 0;
}

static int mtcmos_sram_off(enum mtcmos_id id, const struct mtcmos_data *md)
{
	u32 *ctl_addr = md->ctl_addr;

	if (MTK_SCPD_CAPS(md, MTK_SCPD_SRAM_ISO)) {
		setbits32(ctl_addr, SRAM_CKISO);
		clrbits32(ctl_addr, SRAM_ISOINT_B);
		udelay(1);
	}

	if (MTK_SCPD_CAPS(md, MTK_SCPD_SRAM_SLP)) {
		clrbits32(ctl_addr, SRAM_SLP);
		return mtcmos_wait_for_state(ctl_addr, SRAM_SLP_ACK, false);
	} else {
		setbits32(ctl_addr, SRAM_PDN);
		return mtcmos_wait_for_state(ctl_addr, SRAM_PDN_ACK, true);
	}
}

static int mtcmos_onoff(enum mtcmos_id id, enum mtcmos_state state)
{
	const struct mtcmos_data *md = &mds[id];
	u32 *ctl_addr = md->ctl_addr;
	int ret = 0;

	if (state == MTCMOS_POWER_DOWN) {
		if (md->cb && md->cb->pre_off) {
			ret = md->cb->pre_off();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call pre_off fail(%d)\n", id, ret);
				return ret;
			}
		}

		ret = mtcmos_setclr_bus_prot(md->bp_table, md->bp_num, SET_BUS_PROTECT);
		if (ret)
			return ret;

		if (!MTK_SCPD_CAPS(md, MTK_SCPD_NO_SRAM)) {
			ret = mtcmos_sram_off(id, md);
			if (ret)
				return ret;
		}

		setbits32(ctl_addr, PWR_ISO);
		setbits32(ctl_addr, PWR_CLK_DIS);
		clrbits32(ctl_addr, PWR_RST_B);
		clrbits32(ctl_addr, PWR_ON);
		ret = mtcmos_wait_for_state(ctl_addr, PWR_ACK, state);
		if (ret)
			return ret;

		clrbits32(ctl_addr, PWR_ON_2ND);
		ret = mtcmos_wait_for_state(ctl_addr, PWR_ACK_2ND, state);
		if (ret)
			return ret;

		if (md->cb && md->cb->post_off) {
			ret = md->cb->post_off();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call post_off fail(%d)\n", id, ret);
				return ret;
			}
		}
	} else {
		if (md->cb && md->cb->pre_on) {
			ret = md->cb->pre_on();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call pre_on fail(%d)\n", id, ret);
				return ret;
			}
		}

		setbits32(ctl_addr, PWR_ON);
		ret = mtcmos_wait_for_state(ctl_addr, PWR_ACK, state);
		if (ret)
			return ret;
		udelay(50);

		setbits32(ctl_addr, PWR_ON_2ND);
		ret = mtcmos_wait_for_state(ctl_addr, PWR_ACK_2ND, state);
		if (ret)
			return ret;

		clrbits32(ctl_addr, PWR_CLK_DIS);
		clrbits32(ctl_addr, PWR_ISO);

		if (MTK_SCPD_CAPS(md, MTK_SCPD_RTFF_DELAY))
			udelay(10);

		setbits32(ctl_addr, PWR_RST_B);
		if (!MTK_SCPD_CAPS(md, MTK_SCPD_NO_SRAM)) {
			ret = mtcmos_sram_on(id, md);
			if (ret)
				return ret;
		}

		ret = mtcmos_setclr_bus_prot(md->bp_table, md->bp_num, RELEASE_BUS_PROTECT);
		if (ret)
			return ret;

		if (md->cb && md->cb->post_on) {
			ret = md->cb->post_on();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call post_on fail(%d)\n", id, ret);
				return ret;
			}
		}
	}

	return 0;
}

static int mtcmos_vote_onoff(enum mtcmos_id id, enum mtcmos_state state)
{
	const struct mtcmos_vote_data *md = &vote_mds[id];
	u32 vote_bit = BIT(md->vote_bit);
	int ret = 0;

	if (state == MTCMOS_POWER_DOWN) {
		if (md->cb && md->cb->pre_off) {
			ret = md->cb->pre_off();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call pre_off fail(%d)\n", id, ret);
				return ret;
			}
		}

		write32(md->clr_addr, vote_bit);
		if (!wait_us(WAIT_VOTE_TIMEOUT, ((read32(md->done_addr) & vote_bit) != 0 &&
						 (read32(md->ack_addr) & vote_bit) == vote_bit))) {
			printk(BIOS_ERR, "mtcmos_vote disable %d timeout\n", id);
			return -1;
		}

		if (md->cb && md->cb->post_off) {
			ret = md->cb->post_off();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call post_off fail(%d)\n", id, ret);
				return ret;
			}
		}
	} else {
		if (md->cb && md->cb->pre_on) {
			ret = md->cb->pre_on();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call pre_on fail(%d)\n", id, ret);
				return ret;
			}
		}

		write32(md->set_addr, vote_bit);
		if (!wait_us(WAIT_VOTE_TIMEOUT, ((read32(md->done_addr) & vote_bit) != 0 &&
						 (read32(md->ack_addr) & vote_bit) == vote_bit))) {
			printk(BIOS_ERR, "mtcmos_vote enable %d timeout\n", id);
			return ret;
		}

		if (md->cb && md->cb->post_on) {
			ret = md->cb->post_on();
			if (ret) {
				printk(BIOS_ERR, "mtcmos(%d) call post_on fail(%d)\n", id, ret);
				return ret;
			}
		}
	}

	return 0;
}

static void mtcmos_spm_clr_rtff_flag(void)
{
	clrbits32(&mtk_spm_mtcmos->conn_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssusb_dp_phy_p0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssusb_p0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssusb_p1_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssusb_p23_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssusb_phy_p2_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ufs0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ufs0_phy_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_mac0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_mac1_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_mac2_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_phy0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_phy1_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->pextp_phy2_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->audio_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->adsp_infra_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->adsp_ao_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->mm_proc_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->ssrsys_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->spu_ise_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->spu_hwrot_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->hsgmii0_pwr_con, PWR_RTFF_SAVE_FLAG);
	clrbits32(&mtk_spm_mtcmos->hsgmii1_pwr_con, PWR_RTFF_SAVE_FLAG);
}

static int mtcmos_disp_vcore_pre_on(void)
{
	write32(MM_BUCK_ISO_CON_CLR, BIT(0));
	write32(MM_BUCK_ISO_CON_CLR, BIT(1));
	write32(MM_BUCK_ISO_CON_CLR, BIT(2));

	write32(DISP_AO_PWR_CON, 0x1);

	return 0;
}

static int mtcmos_disp_vcore_post_on(void)
{
	write32(MMPC_BUS_PROTECT_EN_1_CLR, BIT(6) | BIT(7));

	write32p(DISP_VDISP_AO_CONFIG_CG, 0xFFFFFFFF);

	setbits32p(DISP_DPC_DISP0_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_DISP1_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_OVL0_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_OVL1_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_MML0_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_MML1_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_EDP_MTCMOS_CFG, BIT(4));
	setbits32p(DISP_DPC_EPTX_MTCMOS_CFG, BIT(4));

	return 0;
}

static struct mtcmos_cb disp_vcore_cb = {
	.pre_on = mtcmos_disp_vcore_pre_on,
	.post_on = mtcmos_disp_vcore_post_on,
};

void mtcmos_protect_display_bus(void)
{
	/* empty implementation for user common flow */
}

void mtcmos_display_power_on(void)
{
	mtcmos_ctrl(MTCMOS_ID_DIS0_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DIS1_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_OVL0_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_OVL1_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DISP_EDPTX_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DISP_DPTX_SHUTDOWN, MTCMOS_POWER_ON);
}

void mtcmos_ctrl(enum mtcmos_id id, enum mtcmos_state state)
{
	int ret = 0;

	if (id >= MTCMOS_ID_NUM) {
		printk(BIOS_ERR, "id(%d) is invalid\n", id);
		return;
	}

	if (id <= MTCMOS_ID_SSRSYS)
		ret = mtcmos_onoff(id, state);
	else
		ret = mtcmos_vote_onoff(id, state);

	if (ret)
		printk(BIOS_ERR, "%s(%u, %u) fail, ret=%d\n", __func__, id, state, ret);
}

void mtcmos_init(void)
{
	write32(POWERON_CONFIG_EN, (SPM_PROJECT_CODE << 16) | BIT(0));

	mtcmos_spm_clr_rtff_flag();

	mtcmos_ctrl(MTCMOS_ID_SSUSB_P0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_SSUSB_DP_PHY_P0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_SSUSB_P1, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_SSUSB_P23, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_SSUSB_PHY_P2, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_UFS0_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_UFS0_PHY, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_MAC0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_MAC1, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_MAC2, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_PHY0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_PHY1, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_PEXTP_PHY2, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_ADSP_TOP_SHUTDOWN, MTCMOS_POWER_DOWN);
	mtcmos_ctrl(MTCMOS_ID_AUDIO, MTCMOS_POWER_DOWN);
	mtcmos_ctrl(MTCMOS_ID_ADSP_INFRA, MTCMOS_POWER_DOWN);
	mtcmos_ctrl(MTCMOS_ID_ADSP_AO, MTCMOS_POWER_DOWN);
	mtcmos_ctrl(MTCMOS_ID_ADSP_AO, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_ADSP_INFRA, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_AUDIO, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_ADSP_TOP_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_MM_PROC_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_SSRSYS, MTCMOS_POWER_ON);
}

void mtcmos_post_init(void)
{
	if (mtcmos_cb_register(MTCMOS_ID_DISP_VCORE, &disp_vcore_cb))
		printk(BIOS_ERR, "register disp_vcore failed\n");

	mtcmos_ctrl(MTCMOS_ID_MM_INFRA_AO, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_MM_INFRA0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_MM_INFRA1, MTCMOS_POWER_ON);

	mtcmos_ctrl(MTCMOS_ID_DISP_VCORE, MTCMOS_POWER_ON);
	write32(&mtk_disp->clk_cfg[0].clr, 0x00000007);

	mtcmos_ctrl(MTCMOS_ID_DIS0_SHUTDOWN, MTCMOS_POWER_ON);
	write32(&mtk_mmsys->clk_cfg[0].clr, 0xFFFFFFFF);
	write32(&mtk_mmsys->clk_cfg[1].clr, 0x07FFFFFF);

	mtcmos_ctrl(MTCMOS_ID_DIS1_SHUTDOWN, MTCMOS_POWER_ON);
	write32(&mtk_mmsys1->clk_cfg[0].clr, 0xFFFFFFFF);
	write32(&mtk_mmsys1->clk_cfg[1].clr, 0x1FF3FFFF);

	mtcmos_ctrl(MTCMOS_ID_OVL0_SHUTDOWN, MTCMOS_POWER_ON);
	write32(&mtk_ovlsys->clk_cfg[0].clr, 0xFFFFFFFF);
	write32(&mtk_ovlsys->clk_cfg[1].clr, 0x7FFFFFFF);

	mtcmos_ctrl(MTCMOS_ID_OVL1_SHUTDOWN, MTCMOS_POWER_ON);
	write32(&mtk_ovlsys1->clk_cfg[0].clr, 0xFFFFFFFF);
	write32(&mtk_ovlsys1->clk_cfg[1].clr, 0x7FFFFFFF);

	mtcmos_ctrl(MTCMOS_ID_DISP_EDPTX_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DISP_DPTX_SHUTDOWN, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DSI_PHY0, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DSI_PHY1, MTCMOS_POWER_ON);
	mtcmos_ctrl(MTCMOS_ID_DSI_PHY2, MTCMOS_POWER_ON);

	/* vmm buck isolation off */
	write32(MM_BUCK_ISO_CON_CLR, BIT(4));
	write32(MM_BUCK_ISO_CON_CLR, BIT(5));
	write32(MM_BUCK_ISO_CON_CLR, BIT(6));
}
