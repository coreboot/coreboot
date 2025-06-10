/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <soc/clkbuf_ctl.h>
#include <soc/srclken_rc.h>

#define XO_MD2_STABLE_TIME		0x80	/* ~= 1260us */

/* merge with spm */
#define SRCLKENO_0_CTRL_M		MERGE_OR_MODE

/* pmrc_en address */
/* default use this reg direct write */
#define PMRC_CON0			0x1A6
#define PMRC_CON0_SET			0x1A8
#define PMRC_CON0_CLR			0x1AA

/* SUBSYS_INTF_CFG */
#define SUB_BBLPM_SET		(1 << CHN_COANT)
#define SUB_FPM_SET		(1 << CHN_SUSPEND | 1 << CHN_MD0 | \
				 1 << CHN_MD1 | 1 << CHN_MD2 | \
				 1 << CHN_MDRF | 1 << CHN_MMWAVE | \
				 1 << CHN_GPS | 1 << CHN_BT | \
				 1 << CHN_WF | 1 << CHN_CONN_MCU | \
				 1 << CHN_NFC | 1 << CHN_SUSPEND2 | \
				 1 << CHN_UFS)

/* CFG6, mask subsys XO/DCXO request */
#define DCXO_MASK_SUBSYS	0
#define REQ_TO_SPMI_P_SUBSYS	0

/* First try to switch fpm. After polling ack done, switch to HW mode */
const struct rc_config rc_config[MAX_CHN_NUM] = {
	[CHN_SUSPEND] = { .hw_mode = true },
	[CHN_MD0] = { .hw_mode = true },
	[CHN_MD1] = { .hw_mode = true },
	[CHN_MD2] = { .hw_mode = true },
	[CHN_MDRF] = { .hw_mode = true },
	[CHN_MMWAVE] = { .hw_mode = true },
	[CHN_GPS] = { .lpm = true, .hw_mode = true },
	[CHN_BT] = { .lpm = true, .hw_mode = true },
	[CHN_WF] = { .lpm = true, .hw_mode = true },
	[CHN_CONN_MCU] = { .lpm = true, .hw_mode = true },
	[CHN_COANT] = { .lpm = true, .hw_mode = true },
	[CHN_NFC] = { .lpm = true, .hw_mode = true },
	[CHN_SUSPEND2] = { .hw_mode = true },
	[CHN_UFS] = { .lpm = true, .hw_mode = true },
};

const size_t rc_config_num = ARRAY_SIZE(rc_config);

static const struct subsys_rc_con rc_ctrl[MAX_CHN_NUM] = {
	SUB_CTRL_CON_EN(CHN_SUSPEND, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MD0, XO_MD0_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MD1, XO_MD1_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MD2, XO_MD2_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MDRF, XO_MDRF_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_MMWAVE, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_GPS, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_BT, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_WF, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_CONN_MCU, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_COANT, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_NFC, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_SUSPEND2, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
	SUB_CTRL_CON_EN(CHN_UFS, XO_DEFAULT_STABLE_TIME, REQ_ACK_IMD_EN),
};

static void rc_dump_reg_info(void)
{
	int chn_n;

	printk(BIOS_INFO, "RG_DEBUG_CFG[24]: %#lx\n",
	       (read32(&rc_regs->rc_debug_cfg) & BIT(24)) >> 24);
	printk(BIOS_INFO, "SRCLKEN_RC_CFG: 0x%x\n", read32(&rc_regs->srclken_rc_cfg));
	printk(BIOS_INFO, "RG_SRCLKEN_SW_CON_CFG: 0x%x\n",
	       read32(&rc_regs->srclken_sw_con_cfg));
	printk(BIOS_INFO, "RG_PMIC_RCEN_ADDR: 0x%x\n", read32(&rc_regs->rc_pmic_rcen_addr));
	printk(BIOS_INFO, "RG_PMIC_RCEN_SET_CLR_ADDR: 0x%x\n",
	       read32(&rc_regs->rc_pmic_rcen_set_clr_addr));
	printk(BIOS_INFO, "RG_CMD_ARB_CFG: 0x%x\n", read32(&rc_regs->rc_cmd_arb_cfg));
	printk(BIOS_INFO, "RG_CENTRAL_CFG1: 0x%x\n", read32(&rc_regs->rc_central_cfg1));
	printk(BIOS_INFO, "RG_CENTRAL_CFG2: 0x%x\n", read32(&rc_regs->rc_central_cfg2));
	printk(BIOS_INFO, "RG_CENTRAL_CFG3: 0x%x\n", read32(&rc_regs->rc_central_cfg3));
	printk(BIOS_INFO, "RG_CENTRAL_CFG4: 0x%x\n", read32(&rc_regs->rc_central_cfg4));
	printk(BIOS_INFO, "RG_CENTRAL_CFG5: 0x%x\n", read32(&rc_regs->rc_central_cfg5));
	printk(BIOS_INFO, "RG_CENTRAL_CFG6: 0x%x\n", read32(&rc_regs->rc_central_cfg6));
	printk(BIOS_INFO, "RG_DCXO_FPM_CFG: 0x%x\n", read32(&rc_regs->rc_dcxo_fpm_cfg));
	printk(BIOS_INFO, "SUBSYS_INTF_CFG: 0x%x\n", read32(&rc_regs->subsys_intf_cfg));
	printk(BIOS_INFO, "RC_SPI_STA_0: 0x%x\n", read32(&rc_regs->rc_spi_sta_0));
	printk(BIOS_INFO, "RC_PI_PO_STA: 0x%x\n", read32(&rc_regs->rc_pi_po_sta));
	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++)
		printk(BIOS_INFO, "M%d: 0x%x\n", chn_n,
		       read32(&rc_regs->rc_mxx_srclken_cfg[chn_n]));
}

static void rc_ctrl_mode_switch_init(void)
{
	int ch;
	const struct subsys_rc_con *rc_ctrl_ch;
	for (ch = 0; ch < MAX_CHN_NUM; ch++) {
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

static int polling_rc_chn_ack(int chn, u32 chk_sta)
{
	u32 pmrc_en;

	if (!retry(ACK_DELAY_TIMES,
		   (read32(&rc_regs->rc_mxx_req_sta[chn]) & 0xA) == chk_sta,
		   udelay(ACK_DELAY_US))) {
		pmrc_en = clk_buf_get_pmrc_en();
		printk(BIOS_ERR, "%s: polling M%u"
		       "status fail(R:0x%x)(C:0x%x)(PMRC:0x%x)\n",
		       __func__, chn,
		       read32(&rc_regs->rc_mxx_req_sta[chn]),
		       read32(&rc_regs->rc_mxx_srclken_cfg[chn]),
		       pmrc_en);
		return -1;
	}

	return 0;
}

int srclken_rc_init(void)
{
	int chn_n, fpm;
	u32 chk_sta;

	/* Todo: Init in pll init flow */
	/* enable srclken_rc debug trace */
	SET32_BITFIELDS(&rc_regs->rc_debug_cfg, TRACE_MODE_EN, 0x1);

	/* Set SW RESET 1 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, SW_RESET, 0x1);

	/* Wait 100us */
	udelay(100);

	/* Set SW CG 1 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, CG_32K_EN, 0x7);

	/* Wait 100us */
	udelay(100);

	/* Set Clock Mux*/
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, MUX_FCLK_FR, 0x1);

	/* Set req_filter m00~m13 as default SW_FPM */
	rc_ctrl_mode_switch_init();

	/* enable sw trace & pmrc_o0 sel */
	SET32_BITFIELDS(&rc_regs->srclken_sw_con_cfg, TACE_EN, 0x1);
	SET32_BITFIELDS(&rc_regs->srclken_sw_con_cfg, TIE_SYS_RDY, 0x1);

	/* Set PMIC addr for SPI CMD */
	write32(&rc_regs->rc_pmic_rcen_addr, PMRC_CON0);
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

	/* broadcast/multi-cmd setting */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg5,
			  SPMI_CMD_BYTE_CNT, RC_SPMI_BYTE_LEN,
			  SPMI_M_SLV_ID, PMIC_GROUP_ID,
			  SPMI_P_SLV_ID, PMIC_GROUP_ID,
			  SPMI_P_PMIF_ID, 0x1,
			  SPMI_M_CMD_TYPE, 0x3,
			  SPMI_P_CMD_TYPE, 0x3,
			  SPMI_M_WRITE_EN, 0x1,
			  SPMI_P_WRITE_EN, 0x1,
			  BROADCAST_MODE_EN, 0x1,
			  DCXO_ENCODE, 0x1,
			  SPMI_M_FIRST, 0x1);

	/* CFG6 = 0x0 */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg6,
			  REQ_TO_DCXO_MASK, DCXO_MASK_SUBSYS,
			  REQ_TO_SPMI_P_MASK_B, REQ_TO_SPMI_P_SUBSYS);

	/* Set srclkeno_0/conn_bt as factor to allow dcxo change to FPM */
	WRITE32_BITFIELDS(&rc_regs->rc_dcxo_fpm_cfg,
			  SUB_SRCLKEN_FPM_MSK_B, FULL_SET_HW_MODE,
			  SRCVOLTEN_FPM_MSK_B, MD0_SRCLKENO_0_MASK_B,
			  DCXO_FPM_CTRL_M, DCXO_FPM_CTRL_MODE);

	/* Set bblpm/fpm channel */
	WRITE32_BITFIELDS(&rc_regs->subsys_intf_cfg,
			  SRCLKEN_BBLPM_MASK_B, SUB_BBLPM_SET,
			  SRCLKEN_FPM_MASK_B, SUB_FPM_SET);

	/* Trigger srclken_rc enable */
	SET32_BITFIELDS(&rc_regs->rc_central_cfg1, SRCLKEN_RC_EN, 0x1);

	/* MT8189 uses PMIF_M only */
	WRITE32_BITFIELDS(&rc_regs->rc_central_cfg4,
			  SLEEP_VLD_MODE, 0x1,
			  PWRAP_VLD_FORCE, 0x1,
			  KEEP_RC_SPI_ACTIVE, 0x800,
			  BYPASS_PMIF_P, 0x1);

	/* Wait 100us */
	udelay(100);

	/* Set SW RESET 0 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, SW_RESET, 0x0);

	/* Wait 100us */
	udelay(100);

	/* Set SW CG 0 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, CG_32K_EN, 0x0);

	/* Wait 500us */
	udelay(500);

	/* Set req_filter m00~m12 FPM to LPM*/
	rc_init_subsys_lpm();

	/* Polling ACK of Initial Subsys Input */
	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		fpm = rc_ctrl[chn_n].sw_fpm;
		if (rc_config[chn_n].lpm)
			fpm = SW_FPM_LOW;

		chk_sta = (fpm & SW_SRCLKEN_FPM_MSK) << 1 |
			  (rc_ctrl[chn_n].sw_bblpm & SW_SRCLKEN_BBLPM_MSK) << 3;
		if (polling_rc_chn_ack(chn_n, chk_sta))
			die("%s() failed to polling rc_chn %d\n", __func__, chn_n);
	}

	/* Set req_filter m00~m13 */
	rc_init_subsys_hw_mode();

	/* release force pmic req signal*/
	SET32_BITFIELDS(&rc_regs->rc_central_cfg4, PWRAP_VLD_FORCE, 0x0);

	rc_dump_reg_info();

	return 0;
}
