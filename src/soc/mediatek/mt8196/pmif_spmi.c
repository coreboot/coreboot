/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <console/console.h>
#include <device/mmio.h>
#include <gpio.h>
#include <soc/addressmap.h>
#include <soc/pll.h>
#include <soc/pmif.h>
#include <soc/pmif_spmi.h>
#include <soc/spmi.h>

#define SPMI_RCS_CTRL_BASE 0x140084

/* TOPCKGEN, CLK_CFG_17 */
DEFINE_BITFIELD(CLK_SPMI_MST_SEL, 10, 8)
DEFINE_BIT(CLK_SPMI_MST_INT, 12)
DEFINE_BIT(PDN_SPMI_MST, 15)

/* TOPCKGEN, CLK_CFG_UPDATE2 */
DEFINE_BIT(SPMI_MST_CK_UPDATE, 5)

/* SPMI_MST, SPMI_SAMPL_CTRL */
DEFINE_BIT(SAMPL_CK_POL, 0)
DEFINE_BITFIELD(SAMPL_CK_DLY, 3, 1)
DEFINE_BITFIELD(SAMPL_CK_DLY_ARB, 6, 4)

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(VLD_SRCLK_EN_CTRL, 5)
DEFINE_BIT(SPI_MODE_CTRL_PMIF_RDY, 9)
DEFINE_BIT(SPI_MODE_CTRL_SRCLK_EN, 10)
DEFINE_BIT(SPI_MODE_CTRL_SRVOL_EN, 11)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BIT(SPM_SLEEP_REQ_SEL, 0)
DEFINE_BIT(SCP_SLEEP_REQ_SEL, 9)

const struct spmi_device spmi_dev[] = {
	{
		.slvid = SPMI_SLAVE_4,	/* MT6363 */
		.mstid = SPMI_MASTER_1,
		.type = MAIN_PMIC,
		.type_id = MAIN_PMIC_ID,
		.hwcid_addr = 0x09,
		.hwcid_val = 0x63,
		.hwcid_mask = 0xF0,
	},
	{
		.slvid = SPMI_SLAVE_9,	/* MT6685 */
		.mstid = SPMI_MASTER_1,
		.type = CLOCK_PMIC,
		.type_id = CLOCK_PMIC_ID,
		.hwcid_addr = 0x09,
		.hwcid_val = 0x85,
		.hwcid_mask = 0xF0,
	},
	{
		.slvid = SPMI_SLAVE_5, /* MT6373 */
		.mstid = SPMI_MASTER_1,
		.type = SECOND_PMIC,
		.type_id = SECOND_PMIC_ID,
		.hwcid_addr = 0x09,
		.hwcid_val = 0x73,
		.hwcid_mask = 0xF0,
	},
	{
		.slvid = SPMI_SLAVE_6,	/* MT6316 */
		.mstid = SPMI_MASTER_0,
		.type = SUB_PMIC,
		.type_id = SUB_PMIC_ID,
		.hwcid_addr = 0x209,
		.hwcid_val = 0x16,
		.hwcid_mask = 0xf0,
	},
	{
		.slvid = SPMI_SLAVE_7,  /* MT6316 */
		.mstid = SPMI_MASTER_0,
		.type = SUB_PMIC,
		.type_id = SUB_PMIC_ID,
		.hwcid_addr = 0x209,
		.hwcid_val = 0x16,
		.hwcid_mask = 0xf0,
	},
	{
		.slvid = SPMI_SLAVE_8,  /* MT6316 */
		.mstid = SPMI_MASTER_0,
		.type = SUB_PMIC,
		.type_id = SUB_PMIC_ID,
		.hwcid_addr = 0x209,
		.hwcid_val = 0x16,
		.hwcid_mask = 0xf0,
	},
	{
		.slvid = SPMI_SLAVE_15,  /* MT6316 */
		.mstid = SPMI_MASTER_0,
		.type = SUB_PMIC,
		.type_id = SUB_PMIC_ID,
		.hwcid_addr = 0x209,
		.hwcid_val = 0x16,
		.hwcid_mask = 0xf0,
	},
};

static const uint32_t chan_perm[2][4] = {
	[SPMI_MASTER_0] = { 0x8888888A, 0x88888888, 0x88888888, 0x88888888 },
	[SPMI_MASTER_1] = { 0x888888AA, 0x88888888, 0x88888888, 0x88888888 },
};

static const uint32_t lat_limit[2][23] = {
	[SPMI_MASTER_0] = { 0x3e6, 0x3e6, 0x9, 0x9, 0x3e6, 0x3e6, 0x3e6, 0x3e6, 0x9, 0x3e6,
			    0x0, 0x3e6, 0x9, 0x0, 0x0, 0x3e6, 0x3e6, 0x3e6, 0x3e6, 0x0,
			    0x3e6, 0x0, 0x3e6 },
	[SPMI_MASTER_1] = { 0x3e6, 0x0, 0x3e6, 0x3e6, 0x26, 0x37, 0x3e6, 0x10, 0x9, 0x23,
			    0x0, 0x39, 0x9, 0x0, 0x0, 0x12, 0x3e6, 0x3e6, 0x3e6, 0x0,
			    0x1e, 0x0, 0x3e6 },
};

const size_t spmi_dev_cnt = ARRAY_SIZE(spmi_dev);

static struct mtk_spmi_mst_reg *get_mst_reg(struct pmif *arb)
{
	if (arb->mstid == SPMI_MASTER_1)
		return mtk_spmi_mst;
	else if (arb->mstid == SPMI_MASTER_0 || arb->mstid == SPMI_MASTER_2)
		return mtk_spmi_mst_p;
	else
		return NULL;
}

static int pmif_spmi_config_master(struct pmif *arb)
{
	struct mtk_spmi_mst_reg *reg = get_mst_reg(arb);
	if (!reg) {
		printk(BIOS_ERR, "%s: get_mst_reg get null pointer\n", __func__);
		return -1;
	}

	write32(&reg->mst_req_en, 1);
	write32(&reg->grp_id_en, 0xC00);
	/* enable multi-master and rcs support */
	write32(&reg->rcs_ctrl, SPMI_RCS_CTRL_BASE + arb->mstid);
	printk(BIOS_INFO, "%s: mstid=%d done\n", __func__, arb->mstid);
	return 0;
}

void pmif_spmi_config(struct pmif *arb)
{
	_Static_assert(ARRAY_SIZE(chan_perm[0]) == ARRAY_SIZE(arb->mtk_pmif->inf_cmd_per),
		       "Inconsistent array size for chan_perm and inf_cmd_per");
	const uint32_t *perm = chan_perm[arb->mstid % 2];
	/*
	 * all cmd permission for per channel
	 * [0]: Command 0 Permitted
	 * [1]: Command 1 Permitted
	 * [2]: Command 2 Permitted
	 * [3]: Command 3 Permitted
	 */
	/* PER_0[8:0]: CH0 or CH1 for MD DVFS HW, backward compatible for swrgo */
	for (int i = 0; i < ARRAY_SIZE(arb->mtk_pmif->inf_cmd_per); i++)
		write32(&arb->mtk_pmif->inf_cmd_per[i], perm[i]);
}

void pmif_spmi_iocfg(void)
{
	/* SPMI_M 10mA */
	gpio_set_driving(GPIO(SPMI_M_SCL), GPIO_DRV_10_MA);
	gpio_set_driving(GPIO(SPMI_M_SDA), GPIO_DRV_10_MA);
	/* SPMI_P 16mA */
	gpio_set_driving(GPIO(SPMI_P_SCL), GPIO_DRV_16_MA);
	gpio_set_driving(GPIO(SPMI_P_SDA), GPIO_DRV_16_MA);
	/* SPMI-P set Pull-Down mode */
	gpio_set_pull(GPIO(SPMI_P_SCL), GPIO_PULL_ENABLE, GPIO_PULL_DOWN);
	gpio_set_pull(GPIO(SPMI_P_SDA), GPIO_PULL_ENABLE, GPIO_PULL_DOWN);
	printk(BIOS_INFO, "%s done\n", __func__);
}

static int spmi_read_check(struct pmif *arb, const struct spmi_device *dev)
{
	u32 rdata = 0;

	arb->read(arb, dev->slvid, dev->hwcid_addr, &rdata);
	if ((rdata & dev->hwcid_mask) != (dev->hwcid_val & dev->hwcid_mask)) {
		printk(BIOS_WARNING, "%s next, slvid:%d rdata = 0x%x\n",
		       __func__, dev->slvid, rdata);
		return -E_NODEV;
	}

	printk(BIOS_INFO, "%s next, slvid:%d rdata = 0x%x.\n", __func__, dev->slvid, rdata);
	return 0;
}

static int spmi_cali_rd_clock_polarity(struct pmif *arb, const struct spmi_device *dev)
{
	size_t i;
	bool success = false;
	const struct cali cali_data[] = {
		{SPMI_CK_DLY_1T, SPMI_CK_POL_NEG},
		{SPMI_CK_DLY_1T, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_POS},
		{SPMI_CK_NO_DLY, SPMI_CK_POL_NEG},
		{SPMI_CK_DLY_2T, SPMI_CK_POL_POS},
		{SPMI_CK_DLY_2T, SPMI_CK_POL_NEG},
	};

	struct mtk_spmi_mst_reg *reg = get_mst_reg(arb);
	if (!reg) {
		printk(BIOS_ERR, "%s: get_mst_reg get null pointer\n", __func__);
		return -1;
	}

	/* Indicate sampling clock polarity, 1: Positive 0: Negative */
	for (i = 0; i < ARRAY_SIZE(cali_data); i++) {
		SET32_BITFIELDS(&reg->mst_sampl, SAMPL_CK_DLY, cali_data[i].dly,
				SAMPL_CK_POL, cali_data[i].pol);
		SET32_BITFIELDS(&reg->mst_sampl, SAMPL_CK_DLY_ARB, cali_data[i].dly + 1);
		printk(BIOS_DEBUG, "dly:%d, pol:%d, sampl:0x%x\n",
		       cali_data[i].dly, cali_data[i].pol, read32(&reg->mst_sampl));
		if (spmi_read_check(arb, dev) == 0) {
			success = true;
			break;
		}
	}

	if (!success)
		die("ERROR - calibration fail for SPMI clk");

	return 0;
}

static int spmi_config_slave(struct pmif *arb, const struct spmi_device *dev)
{
	u32 wdata;

	/* set RG_RCS_ADDR=slave id */
	wdata = dev->slvid;
	arb->write(arb, dev->slvid, 0x419, wdata); /* rcs_slvid_addr: 0x419 */

	/* set RG_RCS_ENABLE=1, RG_RCS_ID=Master ID */
	wdata = 0x5 | (dev->mstid << 4);
	arb->write(arb, dev->slvid, 0x418, wdata); /* rcs_slvid_en: 0x418 */

	return 0;
}

static int spmi_mst_init(struct pmif *arb)
{
	int i;

	if (!arb) {
		printk(BIOS_ERR, "%s: null pointer for pmif dev\n", __func__);
		return -E_INVAL;
	}

	if (!CONFIG(PMIF_SPMI_IOCFG_DEFAULT_SETTING))
		pmif_spmi_iocfg();

	if (pmif_spmi_config_master(arb))
		return -1;

	for (i = 0; i < spmi_dev_cnt; i++) {
		if ((arb->mstid % 2) == spmi_dev[i].mstid) {
			spmi_cali_rd_clock_polarity(arb, &spmi_dev[i]); /* spmi_cali */
			spmi_config_slave(arb, &spmi_dev[i]);
		}
	}

	return 0;
}

static void pmif_spmi_force_normal_mode(struct pmif *arb)
{
	/* listen srclken_0 only for entering normal or sleep mode */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			VLD_SRCLK_EN_CTRL, 0,
			SPI_MODE_CTRL_PMIF_RDY, 1,
			SPI_MODE_CTRL_SRCLK_EN, 0,
			SPI_MODE_CTRL_SRVOL_EN, 0);

	/* disable spm/scp sleep request */
	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl, SPM_SLEEP_REQ_SEL, 1,
			SCP_SLEEP_REQ_SEL, 1);
	printk(BIOS_INFO, "%s done\n", __func__);
}

static void pmif_spmi_enable_swinf(struct pmif *arb)
{
	write32(&arb->mtk_pmif->inf_en, PMIF_SPMI_SW_CHAN);
	write32(&arb->mtk_pmif->arb_en, PMIF_SPMI_SW_CHAN);
	printk(BIOS_INFO, "%s done\n", __func__);
}

static void pmif_spmi_enable_cmd_issue(struct pmif *arb, bool en)
{
	/* Enable cmdIssue */
	write32(&arb->mtk_pmif->cmdissue_en, en);
	printk(BIOS_INFO, "%s done\n", __func__);
}

static void pmif_spmi_enable(struct pmif *arb)
{
	_Static_assert(ARRAY_SIZE(lat_limit[0]) == ARRAY_SIZE(arb->mtk_pmif->lat_limit),
		       "Inconsistent array size for lat_limit and arb->mtk_pmif->lat_limit");
	const uint32_t *lat_limit_value = lat_limit[arb->mstid % 2];
	u32 pmif_spmi_inf;
	int i;

	if (arb->mstid % 2)
		pmif_spmi_inf = PMIF_SPMI_INF;
	else
		pmif_spmi_inf = PMIF_SPMI_INF_P;

	pmif_spmi_config(arb);

	/* set bytecnt max limitation, 0:1 byte, 1:2 bytes */
	for (i = 0; i < ARRAY_SIZE(arb->mtk_pmif->inf_max_bytecnt_per); i++)
		write32(&arb->mtk_pmif->inf_max_bytecnt_per[i], 0x11111111);

	/* Add latency limitation for all channels, method 2 */
	/* Latency unit = (LAT_UNIT_SEL + 1)T of TMR_CK, LAT_UNIT_SEL=25;Lat unit=26T ~=1us */
	write32(&arb->mtk_pmif->lat_cnter_ctrl, 0x32);
	if (arb->mstid % 2)
		write32(&arb->mtk_pmif->lat_cnter_en, pmif_spmi_inf);
	for (i = 0; i < ARRAY_SIZE(arb->mtk_pmif->lat_limit); i++)
		write32(&arb->mtk_pmif->lat_limit[i], lat_limit_value[i]);
	write32(&arb->mtk_pmif->lat_limit_loading, pmif_spmi_inf);

	/* Disable STAUPD_ITEM_EN */
	write32(&arb->mtk_pmif->staupd_ctrl, 0x1030000);

	write32(&arb->mtk_pmif->inf_en, pmif_spmi_inf);
	write32(&arb->mtk_pmif->arb_en, pmif_spmi_inf);
	write32(&arb->mtk_pmif->timer_ctrl, 0x3);
	write32(&arb->mtk_pmif->init_done, 1);
	printk(BIOS_INFO, "%s init done: 0x%x\n", __func__, read32(&arb->mtk_pmif->init_done));
}

int pmif_spmi_init(struct pmif *arb)
{
	if (arb->check_init_done(arb) != 0) {
		pmif_spmi_force_normal_mode(arb);
		pmif_spmi_enable_swinf(arb);
		pmif_spmi_enable_cmd_issue(arb, true);
		pmif_spmi_enable(arb);
		if (arb->check_init_done(arb))
			return -E_NODEV;
	}

	if (spmi_mst_init(arb)) {
		printk(BIOS_ERR, "[%s] failed to init SPMI master\n", __func__);
		return -E_NODEV;
	}

	return 0;
}
