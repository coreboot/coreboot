/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>
#include <soc/pmif_sw.h>
#include <soc/spmi.h>

/* SPMI_MST, SPMI_SAMPL_CTRL */
DEFINE_BIT(SAMPL_CK_POL, 0)
DEFINE_BITFIELD(SAMPL_CK_DLY, 3, 1)

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(SPI_MODE_CTRL, 7)
DEFINE_BIT(SRVOL_EN, 11)
DEFINE_BIT(SPI_MODE_EXT_CMD, 12)
DEFINE_BIT(SPI_EINT_MODE_GATING_EN, 13)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BITFIELD(SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(SCP_SLEEP_REQ_SEL, 10, 9)

__weak void pmif_spmi_config(struct pmif *arb, int mstid)
{
	/* Do nothing. */
}

static int spmi_read_check(struct pmif *pmif_arb, int slvid)
{
	u32 rdata = 0;

	pmif_arb->read(pmif_arb, slvid, MT6315_READ_TEST, &rdata);
	if (rdata != MT6315_DEFAULT_VALUE_READ) {
		printk(BIOS_INFO, "%s next, slvid:%d rdata = 0x%x.\n",
			__func__, slvid, rdata);
		return -E_NODEV;
	}

	pmif_arb->read(pmif_arb, slvid, MT6315_READ_TEST_1, &rdata);
	if (rdata != MT6315_DEFAULT_VALUE_READ) {
		printk(BIOS_INFO, "%s next, slvid:%d rdata = 0x%x.\n",
			__func__, slvid, rdata);
		return -E_NODEV;
	}

	return 0;
}

static int spmi_cali_rd_clock_polarity(struct pmif *pmif_arb, const struct spmi_device *dev)
{
	int i;
	bool success = false;
	const struct cali cali_data[] = {
		{SPMI_CK_DLY_1T, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_NEG},
		{SPMI_CK_DLY_1T, SPMI_CK_POL_NEG},
	};

	/* Indicate sampling clock polarity, 1: Positive 0: Negative */
	for (i = 0; i < ARRAY_SIZE(cali_data); i++) {
		SET32_BITFIELDS(&mtk_spmi_mst->mst_sampl, SAMPL_CK_DLY, cali_data[i].dly,
				SAMPL_CK_POL, cali_data[i].pol);
		if (spmi_read_check(pmif_arb, dev->slvid) == 0) {
			success = true;
			break;
		}
	}

	if (!success)
		die("ERROR - calibration fail for spmi clk");

	return 0;
}

static int spmi_mst_init(struct pmif *pmif_arb)
{
	size_t i;

	if (!pmif_arb) {
		printk(BIOS_ERR, "%s: null pointer for pmif dev.\n", __func__);
		return -E_INVAL;
	}

	if (!CONFIG(PMIF_SPMI_IOCFG_DEFAULT_SETTING))
		pmif_spmi_iocfg();
	spmi_config_master();

	for (i = 0; i < spmi_dev_cnt; i++)
		spmi_cali_rd_clock_polarity(pmif_arb, &spmi_dev[i]);

	return 0;
}

static void pmif_spmi_force_normal_mode(int mstid)
{
	struct pmif *arb = get_pmif_controller(PMIF_SPMI, mstid);

	/* listen srclken_0 only for entering normal or sleep mode */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			SPI_MODE_CTRL, 0,
			SRVOL_EN, 0,
			SPI_MODE_EXT_CMD, 1,
			SPI_EINT_MODE_GATING_EN, 1);

	/* enable spm/scp sleep request */
	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl, SPM_SLEEP_REQ_SEL, 0,
			SCP_SLEEP_REQ_SEL, 0);
}

static void pmif_spmi_enable_swinf(int mstid)
{
	struct pmif *arb = get_pmif_controller(PMIF_SPMI, mstid);

	write32(&arb->mtk_pmif->inf_en, PMIF_SPMI_SW_CHAN);
	write32(&arb->mtk_pmif->arb_en, PMIF_SPMI_SW_CHAN);
}

static void pmif_spmi_enable_cmdIssue(int mstid, bool en)
{
	struct pmif *arb = get_pmif_controller(PMIF_SPMI, mstid);

	/* Enable cmdIssue */
	write32(&arb->mtk_pmif->cmdissue_en, en);
}

static void pmif_spmi_enable(int mstid)
{
	struct pmif *arb = get_pmif_controller(PMIF_SPMI, mstid);

	pmif_spmi_config(arb, mstid);

	/*
	 * set bytecnt max limitation.
	 * hw bytecnt indicate when we set 0, it can send 1 byte;
	 * set 1, it can send 2 byte.
	 */
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_0, 0);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_1, 0);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_2, 0);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_3, 0);

	/* Add latency limitation */
	write32(&arb->mtk_pmif->lat_cnter_en, PMIF_SPMI_INF);
	write32(&arb->mtk_pmif->lat_limit_0, 0);
	write32(&arb->mtk_pmif->lat_limit_1, 0x4);
	write32(&arb->mtk_pmif->lat_limit_2, 0x8);
	write32(&arb->mtk_pmif->lat_limit_4, 0x8);
	write32(&arb->mtk_pmif->lat_limit_6, 0x3FF);
	write32(&arb->mtk_pmif->lat_limit_9, 0x4);
	write32(&arb->mtk_pmif->lat_limit_loading, PMIF_SPMI_INF);

	write32(&arb->mtk_pmif->inf_en, PMIF_SPMI_INF);
	write32(&arb->mtk_pmif->arb_en, PMIF_SPMI_INF);
	write32(&arb->mtk_pmif->timer_ctrl, 0x3);
	write32(&arb->mtk_pmif->init_done, 1);
}

int pmif_spmi_init(struct pmif *arb)
{
	if (arb->is_pmif_init_done(arb) != 0) {
		pmif_spmi_force_normal_mode(arb->mstid);
		pmif_spmi_enable_swinf(arb->mstid);
		pmif_spmi_enable_cmdIssue(arb->mstid, true);
		pmif_spmi_enable(arb->mstid);
		if (arb->is_pmif_init_done(arb))
			return -E_NODEV;
	}

	if (spmi_mst_init(arb)) {
		printk(BIOS_ERR, "[%s] failed to init spmi master\n", __func__);
		return -E_NODEV;
	}

	return 0;
}
