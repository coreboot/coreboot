/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/clkbuf_ctl.h>
#include <soc/pmif.h>
#include <soc/pmif_common.h>
#include <soc/spmi.h>
#include <soc/srclken_rc.h>
#include <timer.h>

/* merge with spm */
#define SRCLKENO_0_CTRL_M		BYPASS_MODE

/* pmrc_en address */
/* default use this reg direct write */
#define PMRC_CON0			0x190
#define PMRC_CON0_SET			0x198
#define PMRC_CON0_CLR			0x19A

/* XO/DCXO settle time=0, bypass cmd */
#define SUB_CTRL_CON_DIS(_id) \
	SUB_CTRL_CON_INIT(_id, \
			  DCXO_STABLE_TIME, 0, REQ_ACK_IMD_EN, \
			  0x1, DXCO_SETTLE_BLK_DIS)

/* Normal init, SW LPM mode */
#define SUB_CTRL_CON_UNUSE(_id) \
	SUB_CTRL_CON_NO_INIT(_id, \
			     DCXO_STABLE_TIME, XO_DEFAULT_STABLE_TIME, \
			     REQ_ACK_IMD_EN, 0, DXCO_SETTLE_BLK_EN)

/* Porting starting from here */
#define XO_MD2_STABLE_TIME		0x33	/* ~= 500us */
#define VCORE_STABLE_TIME		0x15	/* ~= 200us */

/* SUBSYS_INTF_CFG_FPM */
#define SUB_FPM_SET	(BIT(CHN_SUSPEND) | BIT(CHN_MD1) | \
			 BIT(CHN_MD2) | BIT(CHN_MD3) | \
			 BIT(CHN_MDRF) | BIT(CHN_MMWAVE) | \
			 BIT(CHN_GPS) | BIT(CHN_PCIE_CONN1) | \
			 BIT(CHN_VCORE) | BIT(CHN_CONN_MCU) | \
			 BIT(CHN_COANT) | BIT(CHN_NFC_CONN2) | \
			 BIT(CHN_SUSPEND2) | BIT(CHN_UFS_VREQ))

/* CFG6_1, all 1 means RC M & P used */
#define REQ_TO_SPMI_P_SUBSYS		0xFFFFFFFF
/* subsys 8-vcore on/off & subsys 11-pmic power not join DCXO vote*/
#define SUBSYS_EN_L			0x110E
#define SUBSYS_EN_H			0x0000

/* First try to switch fpm. After polling ack done, switch to HW mode */
const struct rc_config rc_config[MAX_CHN_NUM] = {
	[CHN_SUSPEND] = { .hw_mode = true },
	[CHN_MD1] = { .hw_mode = true },
	[CHN_MD2] = { .hw_mode = true },
	[CHN_MD3] = { .hw_mode = true },
	[CHN_MDRF] = { .hw_mode = true },
	[CHN_MMWAVE] = { .hw_mode = true },
	[CHN_GPS] = { .lpm = true, .hw_mode = true },
	[CHN_PCIE_CONN1] = { .hw_mode = true },
	[CHN_VCORE] = { .lpm = true },
	[CHN_CONN_MCU] = { .lpm = true, .hw_mode = true },
	[CHN_COANT] = { .lpm = true, .hw_mode = true },
	[CHN_NFC_CONN2] = { .lpm = true, .hw_mode = true },
	[CHN_SUSPEND2] = { .hw_mode = true },
	[CHN_UFS_VREQ] = { .hw_mode = true },
	[CHN_DCXO_L] = { .disabled = true },
	[CHN_DCXO_H] = { .disabled = true },
	[CHN_UFS_2] = {},
	[CHN_PCIE_2] = {},
	[CHN_NFC_2] = {},
	[CHN_EXT1] = {},
	[CHN_EXT2] = {},
	[CHN_EXT3] = {},
	[CHN_T900] = {},
	[CHN_RSV1] = {},
	[CHN_RSV2] = {},
	[CHN_RSV3] = {},
	[CHN_RSV4] = {},
	[CHN_RSV5] = {},
	[CHN_RSV6] = {},
	[CHN_RSV7] = {},
	[CHN_RSV8] = {},
	[CHN_RSV9] = {},
};

const struct subsys_rc_con rc_ctrl[MAX_CHN_NUM] = {
	SUB_CTRL_CON_EN(CHN_SUSPEND, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	/* CFG[2] = 0x1 for bypass waiting DCXO settle time */
	/* Usually also need to set CFG6_2 not involve FPM vote */
	SUB_CTRL_CON_INIT(CHN_MD1, DCXO_STABLE_TIME, XO_MD0_STABLE_TIME, REQ_ACK_IMD_EN,
			  0x1, DXCO_SETTLE_BLK_EN),
	SUB_CTRL_CON_INIT(CHN_MD2, DCXO_STABLE_TIME, XO_MD1_STABLE_TIME, REQ_ACK_IMD_EN,
			  0x1, DXCO_SETTLE_BLK_EN),
	SUB_CTRL_CON_INIT(CHN_MD3, DCXO_STABLE_TIME, XO_MD2_STABLE_TIME, REQ_ACK_IMD_EN,
			  0x1, DXCO_SETTLE_BLK_EN),
	SUB_CTRL_CON_EN(CHN_MDRF, XO_MDRF_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MMWAVE, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_GPS, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_PCIE_CONN1, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_INIT(CHN_VCORE, DCXO_STABLE_TIME, VCORE_STABLE_TIME, REQ_ACK_IMD_EN,
			  0x1, DXCO_SETTLE_BLK_EN),
	SUB_CTRL_CON_EN(CHN_CONN_MCU, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_DIS(CHN_COANT),
	SUB_CTRL_CON_EN(CHN_NFC_CONN2, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_INIT(CHN_SUSPEND2, DCXO_STABLE_TIME, VCORE_STABLE_TIME, REQ_ACK_IMD_EN,
			  0x1, DXCO_SETTLE_BLK_EN),
	SUB_CTRL_CON_EN(CHN_UFS_VREQ, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_DIS(CHN_DCXO_L),
	SUB_CTRL_CON_DIS(CHN_DCXO_H),
	SUB_CTRL_CON_UNUSE(CHN_UFS_2),
	SUB_CTRL_CON_UNUSE(CHN_PCIE_2),
	SUB_CTRL_CON_UNUSE(CHN_NFC_2),
	SUB_CTRL_CON_UNUSE(CHN_EXT1),
	SUB_CTRL_CON_UNUSE(CHN_EXT2),
	SUB_CTRL_CON_UNUSE(CHN_EXT3),
	SUB_CTRL_CON_UNUSE(CHN_T900),
	SUB_CTRL_CON_UNUSE(CHN_RSV1),
	SUB_CTRL_CON_UNUSE(CHN_RSV2),
	SUB_CTRL_CON_UNUSE(CHN_RSV3),
	SUB_CTRL_CON_UNUSE(CHN_RSV4),
	SUB_CTRL_CON_UNUSE(CHN_RSV5),
	SUB_CTRL_CON_UNUSE(CHN_RSV6),
	SUB_CTRL_CON_UNUSE(CHN_RSV7),
	SUB_CTRL_CON_UNUSE(CHN_RSV8),
	SUB_CTRL_CON_UNUSE(CHN_RSV9)
};

static void rc_dump_reg_info(void)
{
	u32 ch;

	printk(BIOS_INFO, "RG_DEBUG_CFG[24] : %#lx\n",
	       (read32(&rc_regs->rc_debug_cfg) & BIT(24)) >> 24);
	printk(BIOS_INFO, "RG_SRCLKEN_RC_CFG : %#x\n", read32(&rc_regs->srclken_rc_cfg));

	for (ch = 0; ch < MAX_CHN_NUM; ch++) {
		if (rc_config[ch].disabled)
			continue;
		printk(BIOS_INFO, "RG_M0%d_SRCLKEN_CFG : %#x\n",
		       ch, read32(&rc_regs->rc_mxx_srclken_cfg[ch]));
	}

	printk(BIOS_INFO, "RG_SRCLKEN_SW_CON_CFG : %#x\n",
	       read32(&rc_regs->srclken_sw_con_cfg));
	printk(BIOS_INFO, "RG_PMIC_RCEN_ADDR : %#x\n", read32(&rc_regs->rc_pmic_rcen_addr));
	printk(BIOS_INFO, "RG_PMIC_RCEN_SET_CLR_ADDR : %#x\n",
	       read32(&rc_regs->rc_pmic_rcen_set_clr_addr));
	printk(BIOS_INFO, "RG_CMD_ARB_CFG : %#x\n", read32(&rc_regs->rc_cmd_arb_cfg));
	printk(BIOS_INFO, "RG_CENTRAL_CFG1 : %#x\n", read32(&rc_regs->rc_central_cfg1));
	printk(BIOS_INFO, "RG_CENTRAL_CFG2 : %#x\n", read32(&rc_regs->rc_central_cfg2));
	printk(BIOS_INFO, "RG_CENTRAL_CFG3 : %#x\n", read32(&rc_regs->rc_central_cfg3));
	printk(BIOS_INFO, "RG_CENTRAL_CFG5 : %#x\n", read32(&rc_regs->rc_central_cfg5));
	printk(BIOS_INFO, "RG_CENTRAL_CFG6_1 : %#x\n", read32(&rc_regs->central_cfg6_1));
	printk(BIOS_INFO, "RG_CENTRAL_CFG6_2 : %#x\n", read32(&rc_regs->central_cfg6_2));
	printk(BIOS_INFO, "RG_DCXO_FPM_CFG : %#x\n", read32(&rc_regs->rc_dcxo_fpm_cfg));
	printk(BIOS_INFO, "SUBSYS_INTF_CFG_FPM : %#x\n",
	       read32(&rc_regs->subsys_intf_cfg_fpm));
	printk(BIOS_INFO, "RG_CENTRAL_CFG4 : %#x\n", read32(&rc_regs->rc_central_cfg4));
	printk(BIOS_INFO, "RG_CENTRAL_CFG4_1 : %#x\n", read32(&rc_regs->central_cfg4_1));
	printk(BIOS_INFO, "RG_SRC_PMIC_MULTI_MASTER_CFG : %#x\n",
	       read32(&rc_regs->src_pmrc_multi_master_cfg));
	printk(BIOS_INFO, "RG_SRC_PMRC_STARTER_DISABLE_CFG : %#x\n",
	       read32(&rc_regs->src_pmrc_starter_disable_cfg));
	printk(BIOS_INFO, "RG_SRC_PMRC_STARTER_MASK_CFG : %#x\n",
	       read32(&rc_regs->src_pmrc_starter_mask_cfg));
	printk(BIOS_INFO, "RG_SRC_PMRC_STARTER_BYPASS_CFG : %#x\n",
	       read32(&rc_regs->src_pmrc_starter_bypass_cfg));
	printk(BIOS_INFO, "RG_SRC_PMRC_STARTER_TURN_ON_CNT_SIZE : %#x\n",
	       read32(&rc_regs->src_pmrc_starter_turn_on_cnt_size));
	printk(BIOS_INFO, "RG_SRC_PMRC_STARTER_TURN_OFF_CNT_SIZE : %#x\n",
	       read32(&rc_regs->src_pmrc_starter_turn_off_cnt_size));
	printk(BIOS_INFO, "RC_SPI_STA_0:%#x\n", read32(&rc_regs->rc_spi_sta_0));
	printk(BIOS_INFO, "RC_PI_PO_STA:%#x\n", read32(&rc_regs->rc_pi_po_sta));
}

static void rc_ctrl_mode_init(void)
{
	u32 ch;
	const struct subsys_rc_con *rc_ctrl_ch;

	for (ch = 0; ch < MAX_CHN_NUM; ch++) {
		if (rc_config[ch].disabled)
			continue;
		rc_ctrl_ch = &rc_ctrl[ch];
		SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[ch],
				DCXO_SETTLE_BLK_EN, rc_ctrl_ch->dcxo_settle_blk_en,
				BYPASS_CMD_EN, rc_ctrl_ch->bypass_cmd,
				SW_SRCLKEN_RC, rc_ctrl_ch->sw_rc,
				SW_SRCLKEN_FPM, rc_ctrl_ch->sw_fpm,
				SW_SRCLKEN_BBLPM, rc_ctrl_ch->sw_bblpm,
				XO_SOC_LINKAGE_EN, rc_ctrl_ch->xo_soc_link_en,
				REQ_ACK_LOW_IMD_EN, rc_ctrl_ch->req_ack_imd_en,
				SRCLKEN_TRACK_M_EN, rc_ctrl_ch->track_en,
				CNT_PRD_STEP, rc_ctrl_ch->cnt_step,
				XO_STABLE_PRD, rc_ctrl_ch->xo_prd,
				DCXO_STABLE_PRD, rc_ctrl_ch->dcxo_prd);
	}
}

static int polling_rc_chn_ack(enum chn_id id, u32 mask, u32 value)
{
	if (!retry(ACK_DELAY_TIMES,
		   (read32(&rc_regs->rc_mxx_req_sta_0[id]) & mask) == value,
		   udelay(ACK_DELAY_US))) {
		printk(BIOS_ERR, "%s: polling M%u status fail(R:%#x)(C:%#x)(PMRC:%#x)\n",
		       __func__, id, read32(&rc_regs->rc_mxx_req_sta_0[id]),
		       read32(&rc_regs->rc_mxx_srclken_cfg[id]), clk_buf_get_pmrc_en0());
		return -1;
	}

	return 0;
}

int srclken_rc_init(void)
{
	u32 ch;
	u32 fpm;
	u32 state;
	u32 mask;

	/* Enable srclken_rc debug trace */
	SET32_BITFIELDS(&rc_regs->rc_debug_cfg, TRACE_MODE_EN, 0x1);

	/* Set SW RESET 1 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, SW_RESET, 0x1);
	udelay(100);

	/* Set SW CG 1 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, CG_32K_EN, 0x7);
	udelay(100);

	/* Set Clock Mux*/
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, MUX_FCLK_FR, 0x1);

	/* Set req_filter m00~m13 as default SW_FPM */
	rc_ctrl_mode_init();

	/* Enable sw trace & pmrc_o0 sel */
	SET32_BITFIELDS(&rc_regs->srclken_sw_con_cfg, TACE_EN, 0x1);
	SET32_BITFIELDS(&rc_regs->srclken_sw_con_cfg, TIE_SYS_RDY, 0x1);

	/* Set PMIC addr for SPI CMD */
	write32(&rc_regs->rc_pmic_rcen_addr, PMRC_CON0);

	write32(&rc_regs->rc_pmic_rcen_set_clr_addr, PMRC_CON0);

	write32(&rc_regs->rc_cmd_arb_cfg, 0);

	/* CFG1 setting for spi cmd config */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg1,
			  DCXO_SETTLE_T, DCXO_SETTLE_TIME_VAL,
			  NON_DCXO_SETTLE_T, NON_DCXO_SETTLE_TIME_VAL,
			  ULPOSC_SETTLE_T, ULPOSC_SETTLE_TIME_VAL,
			  VCORE_SETTLE_T, VCORE_SETTLE_TIME_VAL,
			  SRCLKEN_RC_EN_SEL, SRCLKEN_RC_EN_SEL_VAL,
			  RC_SPI_ACTIVE, KEEP_RC_SPI_ACTIVE_VAL,
			  RCEN_ISSUE_M, IS_SPI2PMIC_SET_CLR_VAL,
			  SRCLKEN_RC_EN, RC_CENTRAL_DISABLE_VAL);

	/* CFG2 setting for signal mode of each control mux */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg2,
			  PWRAP_SLP_MUX_SEL, SPI_CLK_SRC,
			  PWRAP_SLP_CTRL_M, PWRAP_CTRL_M,
			  ULPOSC_CTRL_M, ULPOSC_CTRL_M_VAL,
			  SRCVOLTEN_VREQ_M, IS_SPI_DONE_RELEASE,
			  SRCVOLTEN_VREQ_SEL, SPI_TRIG_MODE,
			  VREQ_CTRL, VREQ_CTRL_M,
			  SRCVOLTEN_CTRL, SRCLKENO_0_CTRL_M);

	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg3,
			  TO_LPM_SETTLE_T, 0x4,
			  TO_BBLPM_SETTLE_EN, 0x1,
			  BLK_COANT_DXCO_MD_TARGET, 0x1,
			  BLK_SCP_DXCO_MD_TARGET, 0x1,
			  TO_LPM_SETTLE_EN, 0x1);

	/* Broadcast/multi-cmd setting */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg5,
			  SPMI_CMD_BYTE_CNT, RC_SPMI_BYTE_LEN,
			  SPMI_M_SLV_ID, PMIC_GROUP_ID,
			  SPMI_P_SLV_ID, PMIC_GROUP_ID,
			  SPMI_P_PMIF_ID, 0x1,
			  SPMI_M_CMD_TYPE, 0x3, /* SPMI Burst write */
			  SPMI_P_CMD_TYPE, 0x3, /* SPMI Burst write */
			  SPMI_M_WRITE_EN, 0x1,
			  SPMI_P_WRITE_EN, 0x1,
			  BROADCAST_MODE_EN, 0x1,
			  DCXO_ENCODE, 0x1, /* DCXO 3bits --> 2bits */
			  SPMI_M_FIRST, 0x1,
			  SPMI_BYTE_NUM, 0x1); /* Setting: [29:30] = 1 means send 2 bytes to pmif*/

	/* CFG6-1 = 0x11111111 */
	write32(&rc_regs->central_cfg6_1, REQ_TO_SPMI_P_SUBSYS);

	/* CFG6-2 = 0x00001100 */
	WRITE32_BITFIELDS(&rc_regs->central_cfg6_2,
			  BYPASS_DCXO_VOTE_L, SUBSYS_EN_L,
			  BYPASS_DCXO_VOTE_H, SUBSYS_EN_H);

	/* Set srclkeno_0/conn_bt as factor to allow dcxo change to FPM */
	WRITE32_BITFIELDS(&rc_regs->rc_dcxo_fpm_cfg,
			  SUB_SRCLKEN_FPM_MSK_B, FULL_SET_HW_MODE,
			  SRCVOLTEN_FPM_MSK_B, MD0_SRCLKENO_0_MASK_B,
			  DCXO_FPM_CTRL_M, DCXO_FPM_CTRL_MODE);

	/* Set fpm channel */
	WRITE32_BITFIELDS(&rc_regs->subsys_intf_cfg_fpm,
			  SRCLKEN_FPM_MASK_B, SUB_FPM_SET);

	/* Trigger srclken_rc enable */
	SET32_BITFIELDS(&rc_regs->rc_central_cfg1, SRCLKEN_RC_EN, 0x1);

	/* Use PMIF_M + PMIF_P */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg4,
			  SLEEP_VLD_MODE, 0x1,
			  PWRAP_VLD_FORCE, 0x1);

	/* CFG4_1: extension CFG4[0:15] */
	WRITE32_BITFIELDS(&rc_regs->central_cfg4_1,
			  KEEP_RC_SPI_ACTIVE, 0x800);
	udelay(100);

	/* Set SW RESET 0 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, SW_RESET, 0);
	udelay(100);

	/* Set SW CG 0 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, CG_32K_EN, 0);
	udelay(500);

	/* Set req_filter m00~m31 FPM to LPM, hack original rc_ctrl[]*/
	rc_init_subsys_lpm();

	/* Polling ACK of Initial Subsys Input */
	for (ch = 0; ch < MAX_CHN_NUM; ch++) {
		if (rc_config[ch].disabled)
			continue;

		fpm = rc_ctrl[ch].sw_fpm;
		if (rc_config[ch].lpm)
			fpm = SW_FPM_LOW;

		mask = SW_SRCLKEN_FPM_MSK << 1 | SW_SRCLKEN_BBLPM_MSK << 3;
		state = (fpm & SW_SRCLKEN_FPM_MSK) << 1 |
			(rc_ctrl[ch].sw_bblpm & SW_SRCLKEN_BBLPM_MSK) << 3;

		if (polling_rc_chn_ack(ch, mask, state))
			die("%s() failed to polling rc_chn %d\n", __func__, ch);
	}

	/* Set req_filter m00~m31 */
	rc_init_subsys_hw_mode();

	/* [1] = 1 means pmrc7,11,13 mux choose non multi master */
	write32(&rc_regs->src_pmrc_multi_master_cfg, 0x00000002);

	/* [0] = 1 means disable pmic starter */
	write32(&rc_regs->src_pmrc_starter_disable_cfg, 0x00000001);
	write32(&rc_regs->src_pmrc_starter_mask_cfg, 0x00000000);

	/* pmrc8 & pmrc12 : bypass trigger pmic starter */
	write32(&rc_regs->src_pmrc_starter_bypass_cfg, 0x00001100);

	/* 92E ---> 90us */
	write32(&rc_regs->src_pmrc_starter_turn_on_cnt_size, 0x0000092E);

	/* 92E ---> 90us */
	write32(&rc_regs->src_pmrc_starter_turn_off_cnt_size, 0x0000092E);

	/* Write 0 to release force pmic req signal */
	SET32_BITFIELDS(&rc_regs->rc_central_cfg4, PWRAP_VLD_FORCE, 0);

	rc_dump_reg_info();

	return 0;
}
