/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/pmif.h>
#include <soc/pmif_spi.h>
#include <soc/pmif_spmi.h>
#include <soc/pmif_sw.h>
#include <soc/spmi.h>
#include <timer.h>

static int pmif_check_swinf(struct pmif *arb, long timeout_us, u32 expected_status)
{
	u32 reg_rdata;
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, timeout_us);
	do {
		reg_rdata = read32(&arb->ch->ch_sta);
		if (stopwatch_expired(&sw))
			return E_TIMEOUT;
	} while (GET_SWINF_0_FSM(reg_rdata) != expected_status);

	return 0;
}

static void pmif_send_cmd(struct pmif *arb, int write, u32 opc, u32 slvid,
			  u32 addr, u32 *rdata, u32 wdata, u32 len)
{
	int ret;
	u32 data, bc = len - 1;

	/* Wait for Software Interface FSM state to be IDLE. */
	ret = pmif_check_swinf(arb, PMIF_WAIT_IDLE_US, SWINF_FSM_IDLE);
	if (ret) {
		printk(BIOS_ERR, "[%s] idle timeout\n", __func__);
		return;
	}

	/* Set the write data */
	if (write)
		write32(&arb->ch->wdata, wdata);

	/* Send the command. */
	write32(&arb->ch->ch_send,
		(opc << 30) | (write << 29) | (slvid << 24) | (bc << 16) | addr);

	if (!write) {
		/*
		 * Wait for Software Interface FSM state to be WFVLDCLR,
		 * read the data and clear the valid flag.
		 */
		ret = pmif_check_swinf(arb, PMIF_READ_US, SWINF_FSM_WFVLDCLR);
		if (ret) {
			printk(BIOS_ERR, "[%s] read timeout\n", __func__);
			return;
		}

		data = read32(&arb->ch->rdata);
		*rdata = data;
		write32(&arb->ch->ch_rdy, 0x1);
	}
}

static void pmif_spmi_read(struct pmif *arb, u32 slvid, u32 reg, u32 *data)
{
	*data = 0;
	pmif_send_cmd(arb, 0, PMIF_CMD_EXT_REG_LONG, slvid, reg, data, 0, 1);
}

static void pmif_spmi_write(struct pmif *arb, u32 slvid, u32 reg, u32 data)
{
	pmif_send_cmd(arb, 1, PMIF_CMD_EXT_REG_LONG, slvid, reg, NULL, data, 1);
}

static u32 pmif_spmi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift)
{
	u32 data;

	pmif_spmi_read(arb, slvid, reg, &data);
	data &= (mask << shift);
	data >>= shift;

	return data;
}

static void pmif_spmi_write_field(struct pmif *arb, u32 slvid, u32 reg,
				  u32 val, u32 mask, u32 shift)
{
	u32 old, new;

	pmif_spmi_read(arb, slvid, reg, &old);
	new = old & ~(mask << shift);
	new |= (val << shift);
	pmif_spmi_write(arb, slvid, reg, new);
}

static void pmif_spi_read(struct pmif *arb, u32 slvid, u32 reg, u32 *data)
{
	*data = 0;
	pmif_send_cmd(arb, 0, PMIF_CMD_REG_0, slvid, reg, data, 0, 1);
}

static void pmif_spi_write(struct pmif *arb, u32 slvid, u32 reg, u32 data)
{
	pmif_send_cmd(arb, 1, PMIF_CMD_REG_0, slvid, reg, NULL, data, 1);
}

static u32 pmif_spi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift)
{
	u32 data;

	pmif_spi_read(arb, slvid, reg, &data);
	data &= (mask << shift);
	data >>= shift;

	return data;
}

static void pmif_spi_write_field(struct pmif *arb, u32 slvid, u32 reg,
				 u32 val, u32 mask, u32 shift)
{
	u32 old, new;

	pmif_spi_read(arb, slvid, reg, &old);
	new = old & ~(mask << shift);
	new |= (val << shift);
	pmif_spi_write(arb, slvid, reg, new);
}

static int is_pmif_init_done(struct pmif *arb)
{
	if (read32(&arb->mtk_pmif->init_done) & 0x1)
		return 0;

	return -E_NODEV;
}

static const struct pmif pmif_spmi_arb[] = {
	{
		.mtk_pmif = (struct mtk_pmif_regs *)PMIF_SPMI_BASE,
		.ch = (struct chan_regs *)PMIF_SPMI_AP_CHAN,
		.mstid = SPMI_MASTER_0,
		.pmifid = PMIF_SPMI,
		.write = pmif_spmi_write,
		.read = pmif_spmi_read,
		.write_field = pmif_spmi_write_field,
		.read_field = pmif_spmi_read_field,
		.is_pmif_init_done = is_pmif_init_done,
	},
};

static const struct pmif pmif_spi_arb[] = {
	{
		.mtk_pmif = (struct mtk_pmif_regs *)PMIF_SPI_BASE,
		.ch = (struct chan_regs *)PMIF_SPI_AP_CHAN,
		.pmifid = PMIF_SPI,
		.write = pmif_spi_write,
		.read = pmif_spi_read,
		.write_field = pmif_spi_write_field,
		.read_field = pmif_spi_read_field,
		.is_pmif_init_done = is_pmif_init_done,
	},
};

struct pmif *get_pmif_controller(int inf, int mstid)
{
	if (inf == PMIF_SPMI && mstid < ARRAY_SIZE(pmif_spmi_arb))
		return (struct pmif *)&pmif_spmi_arb[mstid];
	else if (inf == PMIF_SPI)
		return (struct pmif *)&pmif_spi_arb[0];

	die("[%s] Failed to get pmif controller: inf = %d, mstid = %d\n", __func__, inf, mstid);
	return NULL;
}

static void pmif_select(enum pmic_interface mode)
{
	unsigned int spi_spm_sleep_req, spi_scp_sleep_req,
		     spmi_spm_sleep_req, spmi_scp_sleep_req,
		     spi_md_ctl_pmif_rdy, spi_md_ctl_srclk_en, spi_md_ctl_srvol_en,
		     spmi_md_ctl_pmif_rdy, spmi_md_ctl_srclk_en, spmi_md_ctl_srvol_en,
		     spi_inf_srclken_rc_en, spi_other_inf_dcxo0_en, spi_other_inf_dcxo1_en,
		     spi_arb_srclken_rc_en, spi_arb_dcxo_conn_en, spi_arb_dcxo_nfc_en;

	switch (mode) {
	case PMIF_VLD_RDY:
		/* spm and scp sleep request disable spi and spmi */
		spi_spm_sleep_req = 1;
		spi_scp_sleep_req = 1;
		spmi_spm_sleep_req = 1;
		spmi_scp_sleep_req = 1;

		/*
		 * pmic vld/rdy control spi mode enable
		 * srclken control spi mode disable
		 * vreq control spi mode disable
		 */
		spi_md_ctl_pmif_rdy = 1;
		spi_md_ctl_srclk_en = 0;
		spi_md_ctl_srvol_en = 0;
		spmi_md_ctl_pmif_rdy = 1;
		spmi_md_ctl_srclk_en = 0;
		spmi_md_ctl_srvol_en = 0;

		/* srclken rc interface enable */
		spi_inf_srclken_rc_en = 1;

		/* dcxo interface disable */
		spi_other_inf_dcxo0_en = 0;
		spi_other_inf_dcxo1_en = 0;

		/* srclken enable, dcxo0,1 disable */
		spi_arb_srclken_rc_en = 1;
		spi_arb_dcxo_conn_en = 0;
		spi_arb_dcxo_nfc_en = 0;
		break;

	case PMIF_SLP_REQ:
		/* spm and scp sleep request enable spi and spmi */
		spi_spm_sleep_req = 0;
		spi_scp_sleep_req = 0;
		spmi_spm_sleep_req = 0;
		spmi_scp_sleep_req = 0;

		/*
		 * pmic vld/rdy control spi mode disable
		 * srclken control spi mode enable
		 * vreq control spi mode enable
		 */
		spi_md_ctl_pmif_rdy = 0;
		spi_md_ctl_srclk_en = 1;
		spi_md_ctl_srvol_en = 1;
		spmi_md_ctl_pmif_rdy = 0;
		spmi_md_ctl_srclk_en = 1;
		spmi_md_ctl_srvol_en = 1;

		/* srclken rc interface disable */
		spi_inf_srclken_rc_en = 0;

		/* dcxo interface enable */
		spi_other_inf_dcxo0_en = 1;
		spi_other_inf_dcxo1_en = 1;

		/* srclken disable, dcxo0,1 enable */
		spi_arb_srclken_rc_en = 0;
		spi_arb_dcxo_conn_en = 1;
		spi_arb_dcxo_nfc_en = 1;
		break;

	default:
		die("Can't support pmif mode %d\n", mode);
	}

	SET32_BITFIELDS(&pmif_spi_arb[0].mtk_pmif->sleep_protection_ctrl,
			PMIFSPI_SPM_SLEEP_REQ_SEL, spi_spm_sleep_req,
			PMIFSPI_SCP_SLEEP_REQ_SEL, spi_scp_sleep_req);
	SET32_BITFIELDS(&pmif_spmi_arb[0].mtk_pmif->sleep_protection_ctrl,
			PMIFSPMI_SPM_SLEEP_REQ_SEL, spmi_spm_sleep_req,
			PMIFSPMI_SCP_SLEEP_REQ_SEL, spmi_scp_sleep_req);
	SET32_BITFIELDS(&pmif_spi_arb[0].mtk_pmif->spi_mode_ctrl,
			PMIFSPI_MD_CTL_PMIF_RDY, spi_md_ctl_pmif_rdy,
			PMIFSPI_MD_CTL_SRCLK_EN, spi_md_ctl_srclk_en,
			PMIFSPI_MD_CTL_SRVOL_EN, spi_md_ctl_srvol_en);
	SET32_BITFIELDS(&pmif_spmi_arb[0].mtk_pmif->spi_mode_ctrl,
			PMIFSPMI_MD_CTL_PMIF_RDY, spmi_md_ctl_pmif_rdy,
			PMIFSPMI_MD_CTL_SRCLK_EN, spmi_md_ctl_srclk_en,
			PMIFSPMI_MD_CTL_SRVOL_EN, spmi_md_ctl_srvol_en);
	SET32_BITFIELDS(&pmif_spi_arb[0].mtk_pmif->inf_en,
			PMIFSPI_INF_EN_SRCLKEN_RC_HW, spi_inf_srclken_rc_en);
	SET32_BITFIELDS(&pmif_spi_arb[0].mtk_pmif->other_inf_en,
			PMIFSPI_OTHER_INF_DXCO0_EN, spi_other_inf_dcxo0_en,
			PMIFSPI_OTHER_INF_DXCO1_EN, spi_other_inf_dcxo1_en);
	SET32_BITFIELDS(&pmif_spi_arb[0].mtk_pmif->arb_en,
			PMIFSPI_ARB_EN_SRCLKEN_RC_HW, spi_arb_srclken_rc_en,
			PMIFSPI_ARB_EN_DCXO_CONN, spi_arb_dcxo_conn_en,
			PMIFSPI_ARB_EN_DCXO_NFC, spi_arb_dcxo_nfc_en);
}

void pmwrap_interface_init(void)
{
	if (CONFIG(SRCLKEN_RC_SUPPORT)) {
		printk(BIOS_INFO, "%s: Select PMIF_VLD_RDY\n", __func__);
		pmif_select(PMIF_VLD_RDY);
	} else {
		printk(BIOS_INFO, "%s: Select PMIF_SLP_REQ\n", __func__);
		pmif_select(PMIF_SLP_REQ);
	}
}

int mtk_pmif_init(void)
{
	int ret;

	ret = pmif_clk_init();
	if (!ret)
		ret = pmif_spmi_init(get_pmif_controller(PMIF_SPMI, SPMI_MASTER_0));
	if (!ret && !CONFIG(PWRAP_WITH_PMIF_SPMI))
		ret = pmif_spi_init(get_pmif_controller(PMIF_SPI, 0));

	return ret;
}
