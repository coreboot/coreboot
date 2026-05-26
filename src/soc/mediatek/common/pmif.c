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
#include <stdbool.h>
#include <stdint.h>
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

static void send_cmd(struct pmif *arb, bool write, u32 opc, u32 slvid,
		     u32 bc, u32 addr, u32 *rdata, u32 wdata)
{
	int ret;
	u32 cmd = (opc << 30) | ((u32)write << 29) | (slvid << 24) | (bc << 16) | addr;

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
	write32(&arb->ch->ch_send, cmd);

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

		*rdata = read32(&arb->ch->rdata);
		write32(&arb->ch->ch_rdy, 0x1);
	}
}

static void pmif_send_cmd(struct pmif *arb, bool write, u32 opc, u32 slvid,
			  u32 addr, u32 *rdata, u32 wdata, u32 len)
{
	assert(len >= 1);
	send_cmd(arb, write, opc, slvid, len - 1, addr, rdata, wdata);
}

/* Data type is always u16 for PWRAP interface. */
static void pwrap_send_cmd(struct pmif *arb, bool write, u32 addr,
			   u16 *rdata, u16 wdata)
{
	u32 rdata32 = 0;

	/* opc, slvid, bc are not used for PWRAP interface. */
	send_cmd(arb, write, 0, 0, 0, addr, &rdata32, wdata);

	if (!write) {
		*rdata = (u16)rdata32;
		assert(*rdata == rdata32);
	}
}

void pmif_spmi_read8(struct pmif *arb, u32 slvid, u32 reg, u8 *data)
{
	u32 rdata = 0;
	pmif_send_cmd(arb, false, PMIF_CMD_EXT_REG_LONG, slvid, reg, &rdata, 0, 1);
	assert(rdata <= UINT8_MAX);
	*data = (u8)rdata;
}

void pmif_spmi_write8(struct pmif *arb, u32 slvid, u32 reg, u8 data)
{
	pmif_send_cmd(arb, true, PMIF_CMD_EXT_REG_LONG, slvid, reg, NULL, data, 1);
}

void pmif_spmi_read16(struct pmif *arb, u32 slvid, u32 reg, u16 *data)
{
	u32 rdata = 0;
	pmif_send_cmd(arb, false, PMIF_CMD_EXT_REG_LONG, slvid, reg, &rdata, 0, 2);
	assert(rdata <= UINT16_MAX);
	*data = (u16)rdata;
}

void pmif_spmi_write16(struct pmif *arb, u32 slvid, u32 reg, u16 data)
{
	pmif_send_cmd(arb, true, PMIF_CMD_EXT_REG_LONG, slvid, reg, NULL, data, 2);
}

u32 pmif_spmi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift)
{
	u8 data;

	pmif_spmi_read8(arb, slvid, reg, &data);
	return ((u32)data >> shift) & mask;
}

void pmif_spmi_write_field(struct pmif *arb, u32 slvid, u32 reg,
			   u32 val, u32 mask, u32 shift)
{
	u8 old, new;

	pmif_spmi_read8(arb, slvid, reg, &old);
	new = old & ~(mask << shift);
	new |= (u8)((val & mask) << shift);
	pmif_spmi_write8(arb, slvid, reg, new);
}

void pmif_spi_read16(struct pmif *arb, u32 slvid, u32 reg, u16 *data)
{
	pwrap_send_cmd(arb, false, reg, data, 0);
}

void pmif_spi_write16(struct pmif *arb, u32 slvid, u32 reg, u16 data)
{
	pwrap_send_cmd(arb, true, reg, NULL, data);
}

u32 pmif_spi_read_field(struct pmif *arb, u32 slvid, u32 reg, u32 mask, u32 shift)
{
	u16 data;

	pmif_spi_read16(arb, slvid, reg, &data);
	return ((u32)data >> shift) & mask;
}

void pmif_spi_write_field(struct pmif *arb, u32 slvid, u32 reg,
			  u32 val, u32 mask, u32 shift)
{
	u16 old, new;

	assert(((val & mask) << shift) <= UINT16_MAX);

	pmif_spi_read16(arb, slvid, reg, &old);
	new = old & ~(mask << shift);
	new |= (u16)((val & mask) << shift);
	pmif_spi_write16(arb, slvid, reg, new);
}

int pmif_check_init_done(struct pmif *arb)
{
	if (read32(&arb->mtk_pmif->init_done) & 0x1)
		return 0;

	return -E_NODEV;
}

struct pmif *get_pmif_controller(int inf, int mstid)
{
	if (inf == PMIF_SPMI && mstid < pmif_spmi_arb_count)
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
