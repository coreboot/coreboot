/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <soc/addressmap.h>
#include <soc/pmif.h>
#include <soc/srclken_rc.h>

#define RCTAG			"[SRCLKEN_RC]"
#define rc_info(fmt, arg ...)	printk(BIOS_INFO, RCTAG "%s,%d: " fmt, \
					__func__, __LINE__, ## arg)

#define SRCLKEN_DBG		1

/* RC settle time setting */
enum {
	FULL_SET_HW_MODE = 0,	/* dcxo mode use pmrc_en */
	VCORE_SETTLE_TIME = 1,	/* ~= 30us */
	ULPOSC_SETTLE_TIME = 4,	/* ~= 150us */
	XO_SETTLE_TIME = 0x1,	/* 2 ^ (step_sz + 5) * 0x33 * 30.77ns ~= 400us */
	DCXO_SETTLE_TIME = 0x1,	/* 2 ^ (step_sz + 5) * 0x87 * 30.77ns ~= 1063us */
	CENTROL_CNT_STEP = 0x3,	/* fix in 3, central align with Mxx channel */
	DCXO_STABLE_TIME = 0x70,
	XO_STABLE_TIME = 0x70,
	KEEP_RC_SPI_ACTIVE_VAL = 1,
	SRCLKEN_RC_EN_SEL_VAL = 0,
};

enum {
	INIT_SUBSYS_FPM_TO_LPM = 1 << CHN_RF | 1 << CHN_DEEPIDLE | 1 << CHN_MD
				| 1 << CHN_GPS | 1 << CHN_BT | 1 << CHN_WIFI
				| 1 << CHN_MCU | 1 << CHN_COANT | 1 << CHN_NFC
				| 1 << CHN_UFS | 1 << CHN_SCP | 1 << CHN_RESERVE,
	INIT_SUBSYS_FPM_TO_BBLPM = 1 << CHN_DEEPIDLE,
	INIT_SUBSYS_TO_HW = 1 << CHN_SUSPEND | 1 << CHN_DEEPIDLE | 1 << CHN_MCU,
};

/* RC central setting */
enum {
	RC_CENTRAL_ENABLE = 1,
	RC_CENTRAL_DISABLE = 0,
	SPI_TRIG_MODE = SRCLKENAO_MODE, /* use srlckenao to set vcore */
	IS_SPI_DONE_RELEASE = 0, /* release vcore when spi request done */
	IS_SPI2PMIC_SET_CLR = 0, /* register direct write */
	SRCLKENO_0_CTRL_M = MERGE_OR_MODE, /* merge with spm */
	VREQ_CTRL_M = BYPASS_MODE, /* merge with vreq */
	ULPOSC_CTRL_M_VAL = BYPASS_MODE, /* merge with ulposc */
	PWRAP_CTRL_M = MERGE_OR_MODE, /* merge with pwrap_scp */
	SPI_CLK_SRC = RC_32K, /* pmic spec under 200us */
};

/* Other setting */
enum {
	DCXO_FPM_CTRL_MODE = MERGE_OR_MODE | ASYNC_MODE, /* merge with spm */
	PWRAP_TMOUT_VAL = 0xA, /* 31us * 0xa ~= 310us */
	FPM_MSK_B = FULL_SET_HW_MODE,
	MD0_SRCLKENO_0_MASK_B = 0, /* md0 control by pmrc */
};

enum {
	SUB_BBLPM_SET = 1 << CHN_COANT | 1 << CHN_DEEPIDLE,
	SUB_FPM_SET = 1 << CHN_SUSPEND | 1 << CHN_RF | 1 << CHN_MD
			| 1 << CHN_GPS | 1 << CHN_BT | 1 << CHN_WIFI
			| 1 << CHN_MCU | 1 << CHN_NFC | 1 << CHN_UFS
			| 1 << CHN_SCP | 1 << CHN_RESERVE,
};

enum {
	SW_FPM_LOW = 0,
	SW_FPM_HIGH = 1,
};

enum {
	SW_BBLPM_LOW = 0,
	SW_BBLPM_HIGH = 1,
};

enum {
	DXCO_SETTLE_BLK_DIS = 0,
	DXCO_SETTLE_BLK_EN = 1,
};

#define SUB_CTRL_CON(_dcxo_prd, _xo_prd, _bypass_cmd, _dcxo_settle_blk_en) {	\
		.dcxo_prd = _dcxo_prd,		\
		.xo_prd = _xo_prd,		\
		.cnt_step = CENTROL_CNT_STEP,	\
		.track_en = 0x0,		\
		.req_ack_imd_en = 0x1,		\
		.xo_soc_link_en = 0x0,		\
		.sw_bblpm = SW_BBLPM_LOW,	\
		.sw_fpm = SW_FPM_HIGH,		\
		.sw_rc = SW_MODE,		\
		.bypass_cmd = _bypass_cmd,	\
		.dcxo_settle_blk_en = _dcxo_settle_blk_en,	\
	}

static struct mtk_rc_regs *rc_regs = (struct mtk_rc_regs *)RC_BASE;
static struct mtk_rc_status_regs *rc_sta_regs = (struct mtk_rc_status_regs *)RC_STATUS_BASE;

static struct subsys_rc_con rc_ctrl[MAX_CHN_NUM] = {
	[CHN_SUSPEND] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_RF] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_DEEPIDLE] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME,
				      0x0, DXCO_SETTLE_BLK_EN),
	[CHN_MD] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_GPS] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_BT] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_WIFI] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_MCU] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_COANT] = SUB_CTRL_CON(0x0, 0x0, 0x1, DXCO_SETTLE_BLK_DIS),
	[CHN_NFC] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_UFS] = SUB_CTRL_CON(DCXO_STABLE_TIME, XO_STABLE_TIME, 0x0, DXCO_SETTLE_BLK_EN),
	[CHN_SCP] = SUB_CTRL_CON(0x0, 0x0, 0x1, DXCO_SETTLE_BLK_DIS),
	[CHN_RESERVE] = SUB_CTRL_CON(0x0, 0x0, 0x1, DXCO_SETTLE_BLK_DIS),
};

static void pmic_read(u32 addr, u32 *rdata)
{
	static struct pmif *pmif_arb;

	if (!pmif_arb)
		pmif_arb = get_pmif_controller(PMIF_SPI, 0);

	pmif_arb->read(pmif_arb, 0, addr, rdata);
}

static void rc_dump_reg_info(void)
{
	unsigned int chn_n;

	if (SRCLKEN_DBG) {
		rc_info("SRCLKEN_RC_CFG:%#x\n", read32(&rc_regs->srclken_rc_cfg));
		rc_info("RC_CENTRAL_CFG1:%#x\n", read32(&rc_regs->rc_central_cfg1));
		rc_info("RC_CENTRAL_CFG2:%#x\n", read32(&rc_regs->rc_central_cfg2));
		rc_info("RC_CENTRAL_CFG3:%#x\n", read32(&rc_regs->rc_central_cfg3));
		rc_info("RC_CENTRAL_CFG4:%#x\n", read32(&rc_regs->rc_central_cfg4));
		rc_info("RC_DCXO_FPM_CFG:%#x\n", read32(&rc_regs->rc_dcxo_fpm_cfg));
		rc_info("SUBSYS_INTF_CFG:%#x\n", read32(&rc_regs->rc_subsys_intf_cfg));
		rc_info("RC_SPI_STA_0:%#x\n", read32(&rc_sta_regs->rc_spi_sta_0));
		rc_info("RC_PI_PO_STA:%#x\n", read32(&rc_sta_regs->rc_pi_po_sta_0));

		for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
			rc_info("M%02d: %#x\n", chn_n,
				read32(&rc_regs->rc_mxx_srclken_cfg[chn_n]));
		}
	}
}

/* RC initial flow and relative setting */
static void __rc_ctrl_mode_switch(enum chn_id id, enum rc_ctrl_m mode)
{
	switch (mode) {
	case INIT_MODE:
		SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id],
				DCXO_SETTLE_BLK_EN, rc_ctrl[id].dcxo_settle_blk_en,
				BYPASS_CMD_EN, rc_ctrl[id].bypass_cmd,
				SW_SRCLKEN_RC, rc_ctrl[id].sw_rc,
				SW_SRCLKEN_FPM, rc_ctrl[id].sw_fpm,
				SW_SRCLKEN_BBLPM, rc_ctrl[id].sw_bblpm,
				XO_SOC_LINK_EN, rc_ctrl[id].xo_soc_link_en,
				REQ_ACK_LOW_IMD_EN, rc_ctrl[id].req_ack_imd_en,
				SRCLKEN_TRACK_M_EN, rc_ctrl[id].track_en,
				CNT_PRD_STEP, rc_ctrl[id].cnt_step,
				XO_STABLE_PRD, rc_ctrl[id].xo_prd,
				DCXO_STABLE_PRD, rc_ctrl[id].dcxo_prd);
		break;
	case SW_MODE:
		SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_RC, 1);
		break;
	case HW_MODE:
		SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_RC, 0);
		break;
	default:
		die("Can't support rc_mode %d\n", mode);
	}

	rc_info("M%02d: %#x\n", id, read32(&rc_regs->rc_mxx_srclken_cfg[id]));
}


/* RC subsys FPM control */
static void __rc_ctrl_fpm_switch(enum chn_id id, unsigned int mode)
{
	SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_FPM, !!mode);
	rc_ctrl[id].sw_fpm = mode;
	rc_info("M%02d FPM SWITCH: %#x\n", id, read32(&rc_regs->rc_mxx_srclken_cfg[id]));
}

static void __rc_ctrl_bblpm_switch(enum chn_id id, unsigned int mode)
{
	SET32_BITFIELDS(&rc_regs->rc_mxx_srclken_cfg[id], SW_SRCLKEN_BBLPM, !!mode);
	rc_ctrl[id].sw_bblpm = mode;
	rc_info("M%02d BBLPM SWITCH: %#x\n", id, read32(&rc_regs->rc_mxx_srclken_cfg[id]));
}

static void rc_init_subsys_hw_mode(void)
{
	unsigned int chn_n;

	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		if (INIT_SUBSYS_TO_HW & (1 << chn_n))
			__rc_ctrl_mode_switch(chn_n, HW_MODE);
	}
}

static void rc_init_subsys_lpm(void)
{
	unsigned int chn_n;

	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		if (INIT_SUBSYS_FPM_TO_LPM & (1 << chn_n))
			__rc_ctrl_fpm_switch(chn_n, SW_FPM_LOW);
	}
	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		if (INIT_SUBSYS_FPM_TO_BBLPM & (1 << chn_n))
			__rc_ctrl_bblpm_switch(chn_n, SW_BBLPM_HIGH);
	}
}

static void rc_ctrl_mode_switch_init(enum chn_id id)
{
	__rc_ctrl_mode_switch(id, INIT_MODE);
}

static enum rc_support srclken_rc_chk_init_cfg(void)
{
	pmwrap_interface_init();
	if (!CONFIG(SRCLKEN_RC_SUPPORT)) {
		rc_info("Bring-UP : skip srclken_rc init\n");
		return SRCLKEN_RC_DISABLE;
	}
	if (SRCLKEN_DBG) {
		/* Enable debug trace */
		write32(&rc_sta_regs->rc_debug_trace, 1);
		SET32_BITFIELDS(&rc_regs->rc_debug_cfg, TRACE_MODE_EN, 1);
	}
	return SRCLKEN_RC_ENABLE;
}

int srclken_rc_init(void)
{
	/* New co-clock architecture srclkenrc implement here */
	unsigned int chn_n;
	int ret = 0;

	/* Check platform config to proceed init flow */
	if (srclken_rc_chk_init_cfg() != SRCLKEN_RC_ENABLE)
		return ret;

	/* Set SW RESET 1 */
	SET32_BITFIELDS(&rc_regs->srclken_rc_cfg, SW_RESET, 1);

	/* Wait 100us */
	udelay(100);

	/* Set SW CG 1 */
	write32(&rc_regs->srclken_rc_cfg,
		_BF_VALUE(SW_RESET, 1) | _BF_VALUE(CG_32K_EN, 1) |
		_BF_VALUE(CG_FCLK_EN, 1) | _BF_VALUE(CG_FCLK_FR_EN, 1));

	/* Wait 100us */
	udelay(100);

	/* Set Clock Mux */
	write32(&rc_regs->srclken_rc_cfg,
		_BF_VALUE(SW_RESET, 1) | _BF_VALUE(CG_32K_EN, 1) |
		_BF_VALUE(CG_FCLK_EN, 1) | _BF_VALUE(CG_FCLK_FR_EN, 1) |
		_BF_VALUE(MUX_FCLK_FR, 1));

	/* Set req_filter m00~m12 as default SW_FPM */
	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++)
		rc_ctrl_mode_switch_init(chn_n);

	/* Set PMIC addr for SPI CMD */
	write32(&rc_regs->rc_pmic_rcen_addr, PMIC_PMRC_CON0);

	write32(&rc_regs->rc_pmic_rcen_set_clr_addr,
		(PMIC_PMRC_CON0_CLR << 16) | PMIC_PMRC_CON0_SET);

	write32(&rc_regs->rc_cmd_arb_cfg, 0);

	/* CFG1 setting for spi cmd config */
	write32(&rc_regs->rc_central_cfg1,
		_BF_VALUE(DCXO_SETTLE_T, DCXO_SETTLE_TIME) |
		_BF_VALUE(NON_DCXO_SETTLE_T, XO_SETTLE_TIME) |
		_BF_VALUE(ULPOSC_SETTLE_T, ULPOSC_SETTLE_TIME) |
		_BF_VALUE(VCORE_SETTLE_T, VCORE_SETTLE_TIME) |
		_BF_VALUE(SRCLKEN_RC_EN_SEL, SRCLKEN_RC_EN_SEL_VAL) |
		_BF_VALUE(RC_SPI_ACTIVE, KEEP_RC_SPI_ACTIVE_VAL) |
		_BF_VALUE(RCEN_ISSUE_M, IS_SPI2PMIC_SET_CLR) |
		_BF_VALUE(SRCLKEN_RC_EN, RC_CENTRAL_DISABLE));

	/* CFG2 setting for signal mode of each control mux */
	write32(&rc_regs->rc_central_cfg2,
		_BF_VALUE(PWRAP_SLP_MUX_SEL, SPI_CLK_SRC) |
		_BF_VALUE(PWRAP_SLP_CTRL_M, PWRAP_CTRL_M) |
		_BF_VALUE(ULPOSC_CTRL_M, ULPOSC_CTRL_M_VAL) |
		_BF_VALUE(SRCVOLTEN_VREQ_M, IS_SPI_DONE_RELEASE) |
		_BF_VALUE(SRCVOLTEN_VREQ_SEL, SPI_TRIG_MODE) |
		_BF_VALUE(VREQ_CTRL, VREQ_CTRL_M) |
		_BF_VALUE(SRCVOLTEN_CTRL, SRCLKENO_0_CTRL_M));

	write32(&rc_regs->rc_central_cfg3,
		_BF_VALUE(TO_LPM_SETTLE_T, 0x4) |
		_BF_VALUE(TO_BBLPM_SETTLE_EN, 1) |
		_BF_VALUE(BLK_COANT_DXCO_MD_TARGET, 1) |
		_BF_VALUE(BLK_SCP_DXCO_MD_TARGET, 1) |
		_BF_VALUE(TO_LPM_SETTLE_EN, 1));

	/* Set srclkeno_0/conn_bt as factor to allow dcxo change to FPM */
	write32(&rc_regs->rc_dcxo_fpm_cfg,
		_BF_VALUE(SUB_SRCLKEN_FPM_MSK_B, FPM_MSK_B) |
		_BF_VALUE(SRCVOLTEN_FPM_MSK_B, MD0_SRCLKENO_0_MASK_B) |
		_BF_VALUE(DCXO_FPM_CTRL_M, DCXO_FPM_CTRL_MODE));

	/* Set bblpm/fpm channel */
	write32(&rc_regs->rc_subsys_intf_cfg,
		_BF_VALUE(SRCLKEN_BBLPM_MASK_B, SUB_BBLPM_SET) |
		_BF_VALUE(SRCLKEN_FPM_MASK_B, SUB_FPM_SET));

	/* Trigger srclken_rc enable */
	SET32_BITFIELDS(&rc_regs->rc_central_cfg1,
			SRCLKEN_RC_EN, RC_CENTRAL_ENABLE);

	write32(&rc_regs->rc_central_cfg4,
		_BF_VALUE(SLEEP_VLD_MODE, 0x1) |
		_BF_VALUE(PWRAP_VLD_FORCE, 0x1) |
		_BF_VALUE(KEEP_RC_SPI_ACTIVE, 0x800));


	/* Wait 100us */
	udelay(100);

	/* Set SW RESET 0 */
	write32(&rc_regs->srclken_rc_cfg,
		_BF_VALUE(CG_32K_EN, 1) | _BF_VALUE(CG_FCLK_EN, 1) |
		_BF_VALUE(CG_FCLK_FR_EN, 1) | _BF_VALUE(MUX_FCLK_FR, 1));

	/* Wait 100us */
	udelay(100);

	/* Set SW CG 0 */
	write32(&rc_regs->srclken_rc_cfg, _BF_VALUE(MUX_FCLK_FR, 1));

	/* Wait 500us */
	udelay(500);

	/* Set req_filter m00~m12 FPM to LPM */
	rc_init_subsys_lpm();

	/* Polling ACK of Initial Subsys Input */
	for (chn_n = 0; chn_n < MAX_CHN_NUM; chn_n++) {
		unsigned int chk_sta, shift_chn_n = 0;
		int retry;
		u32 temp;

		/* Fix RC_MXX_REQ_STA_0 register shift */
		if (chn_n > 0)
			shift_chn_n = 1;

		chk_sta = (rc_ctrl[chn_n].sw_fpm & SW_SRCLKEN_FPM_MSK) << 1 |
			(rc_ctrl[chn_n].sw_bblpm & SW_SRCLKEN_BBLPM_MSK) << 3;
		retry = 200;
		while ((read32(&rc_sta_regs->rc_mxx_req_sta_0[chn_n + shift_chn_n]) & 0xa)
				!= chk_sta && retry-- > 0)
			udelay(10);
		if (retry < 0) {
			pmic_read(PMIC_PMRC_CON0, &temp);
			rc_info("polling M%02d failed.(R:%#x)(C:%#x)(PMRC:%#x)\n",
				chn_n,
				read32(&rc_sta_regs->rc_mxx_req_sta_0[chn_n + shift_chn_n]),
				read32(&rc_regs->rc_mxx_srclken_cfg[chn_n]), temp);
			ret = -1;
		}
	}

	/* Set req_filter m00~m12 */
	rc_init_subsys_hw_mode();

	/* Release force pmic req signal */
	write32(&rc_regs->rc_central_cfg4,
		_BF_VALUE(SLEEP_VLD_MODE, 0x1) |
		_BF_VALUE(KEEP_RC_SPI_ACTIVE, 0x800));

	rc_dump_reg_info();

	return ret;
}
