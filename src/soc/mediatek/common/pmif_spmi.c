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

__weak void pmif_spmi_config(struct pmif *arb)
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

static int spmi_cali_rd_clock_polarity(struct pmif *pmif_arb)
{
	int i, j;
	const struct cali cali_data[] = {
		{SPMI_CK_DLY_1T, SPMI_CK_POL_NEG},
		{SPMI_CK_DLY_1T, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_NEG},
	};

	/* Indicate sampling clock polarity, 1: Positive 0: Negative */
	for (i = 0; i < ARRAY_SIZE(cali_data); i++) {
		bool success = true;
		SET32_BITFIELDS(&mtk_spmi_mst->mst_sampl, SAMPL_CK_DLY,
				cali_data[i].dly, SAMPL_CK_POL, cali_data[i].pol);
		for (j = 0; j < spmi_dev_cnt(); j++) {
			if (spmi_read_check(pmif_arb, spmi_dev[j].slvid) != 0) {
				success = false;
				break;
			}
		}
		if (success) {
			printk(BIOS_INFO, "calibration success for spmi clk: "
			       "cali_data[%d] dly = %u, pol = %u\n",
			       i, cali_data[i].dly, cali_data[i].pol);
			return 0;
		}
	}

	return -E_NODEV;
}

static int spmi_mst_init(struct pmif *pmif_arb)
{
	if (!pmif_arb) {
		printk(BIOS_ERR, "%s: null pointer for pmif dev.\n", __func__);
		return -E_INVAL;
	}

	if (!CONFIG(PMIF_SPMI_IOCFG_DEFAULT_SETTING))
		pmif_spmi_iocfg();
	spmi_config_master();

	if (spmi_cali_rd_clock_polarity(pmif_arb) != 0)
		die("ERROR - calibration fail for spmi clk");

	return 0;
}

static void pmif_spmi_enable_swinf(struct pmif *arb)
{
	write32(&arb->mtk_pmif->inf_en, PMIF_SPMI_SW_CHAN);
	write32(&arb->mtk_pmif->arb_en, PMIF_SPMI_SW_CHAN);
}

static void pmif_spmi_enable_cmdIssue(struct pmif *arb, bool en)
{
	/* Enable cmdIssue */
	write32(&arb->mtk_pmif->cmdissue_en, en);
}

static void pmif_spmi_enable(struct pmif *arb)
{
	pmif_spmi_config(arb);

	/*
	 * Set max bytecnt. For each 32-bit reg, 4 bits indicate one channel.
	 * 0x0 means max bytecnt = 1.
	 * 0x1 means max bytecnt = 2.
	 * Here we set max bytecnt to 2 for all channels.
	 */

	write32(&arb->mtk_pmif->inf_max_bytecnt_per_0, 0x11111111);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_1, 0x11111111);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_2, 0x11111111);
	write32(&arb->mtk_pmif->inf_max_bytecnt_per_3, 0x11111111);

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
	if (arb->check_init_done(arb) != 0) {
		pmif_spmi_force_normal_mode(arb);
		pmif_spmi_enable_swinf(arb);
		pmif_spmi_enable_cmdIssue(arb, true);
		pmif_spmi_enable(arb);
		if (arb->check_init_done(arb))
			return -E_NODEV;
	}

	if (spmi_mst_init(arb)) {
		printk(BIOS_ERR, "[%s] failed to init spmi master\n", __func__);
		return -E_NODEV;
	}

	return 0;
}
