/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/pll.h>
#include <soc/pmif_spi.h>
#include <soc/pmif_sw.h>
#include <timer.h>

/* PMIF, SPI_MODE_CTRL */
DEFINE_BIT(SPI_MODE_CTRL_VLD_SRCLK_EN_CTRL, 5)
DEFINE_BIT(SPI_MODE_CTRL_PMIF_RDY, 9)
DEFINE_BIT(SPI_MODE_CTRL_SRCLK_EN, 10)
DEFINE_BIT(SPI_MODE_CTRL_SRVOL_EN, 11)

/* PMIF, SLEEP_PROTECTION_CTRL */
DEFINE_BITFIELD(SPM_SLEEP_REQ_SEL, 1, 0)
DEFINE_BITFIELD(SCP_SLEEP_REQ_SEL, 10, 9)

/* PMIF, OTHER_INF_EN */
DEFINE_BITFIELD(INTGPSADCINF_EN, 5, 4)

/* PMIF, STAUPD_CTRL */
DEFINE_BITFIELD(STAUPD_CTRL_PRD, 3, 0)
DEFINE_BIT(STAUPD_CTRL_PMIC0_SIG_STA, 4)
DEFINE_BIT(STAUPD_CTRL_PMIC0_EINT_STA, 6)

/* SPIMST, Manual_Mode_Access */
DEFINE_BITFIELD(MAN_ACC_SPI_OP, 12, 8)
DEFINE_BIT(MAN_ACC_SPI_RW, 13)

static void pmif_spi_config(struct pmif *arb)
{
	/* Set srclk_en always valid regardless of ulposc_sel_for_scp */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl, SPI_MODE_CTRL_VLD_SRCLK_EN_CTRL, 0);

	/* Set SPI mode controlled by srclk_en and srvol_en instead of pmif_rdy */
	SET32_BITFIELDS(&arb->mtk_pmif->spi_mode_ctrl,
			SPI_MODE_CTRL_SRCLK_EN, 1,
			SPI_MODE_CTRL_SRVOL_EN, 1,
			SPI_MODE_CTRL_PMIF_RDY, 0);

	SET32_BITFIELDS(&arb->mtk_pmif->sleep_protection_ctrl, SPM_SLEEP_REQ_SEL, 0,
			SCP_SLEEP_REQ_SEL, 0);

	/* Enable SWINF for AP */
	write32(&arb->mtk_pmif->inf_en, PMIF_SPI_AP);

	/* Enable arbitration for SWINF for AP */
	write32(&arb->mtk_pmif->arb_en, PMIF_SPI_AP);

	/* Enable PMIF_SPI Command Issue */
	write32(&arb->mtk_pmif->cmdissue_en, 1);
}

static int check_idle(void *addr, u32 expected)
{
	u32 reg_rdata;
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, PMIF_WAIT_IDLE_US);
	do {
		reg_rdata = read32(addr);
		if (stopwatch_expired(&sw))
			return E_TIMEOUT;
	} while ((reg_rdata & expected) != 0);

	return 0;
}

static int reset_spislv(void)
{
	u32 pmicspi_mst_dio_en_backup;

	write32(&mtk_pmicspi_mst->wrap_en, 0);
	write32(&mtk_pmicspi_mst->mux_sel, 1);
	write32(&mtk_pmicspi_mst->man_en, 1);
	pmicspi_mst_dio_en_backup = read32(&mtk_pmicspi_mst->dio_en);
	write32(&mtk_pmicspi_mst->dio_en, 0);

	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_CSL);
	/* Reset counter */
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_OUTS);
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_CSH);
	/*
	 * In order to pull CSN signal to PMIC,
	 * PMIC will count it then reset spi slave
	 */
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_OUTS);
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_OUTS);
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_OUTS);
	SET32_BITFIELDS(&mtk_pmicspi_mst->man_acc, MAN_ACC_SPI_RW, OP_WR,
			MAN_ACC_SPI_OP, OP_OUTS);

	/* Wait for PMIC SPI Master to be idle */
	if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
		printk(BIOS_ERR, "[%s] spi master busy, timeout\n", __func__);
		return E_TIMEOUT;
	}

	write32(&mtk_pmicspi_mst->man_en, 0);
	write32(&mtk_pmicspi_mst->mux_sel, 0);
	write32(&mtk_pmicspi_mst->wrap_en, 1);
	write32(&mtk_pmicspi_mst->dio_en, pmicspi_mst_dio_en_backup);

	return 0;
}

static void init_reg_clock(struct pmif *arb)
{
	pmif_spi_iocfg();

	/* Configure SPI protocol */
	write32(&mtk_pmicspi_mst->ext_ck_write, 1);
	write32(&mtk_pmicspi_mst->ext_ck_read, 0);
	write32(&mtk_pmicspi_mst->cshext_write, 0);
	write32(&mtk_pmicspi_mst->cshext_read, 0);
	write32(&mtk_pmicspi_mst->cslext_write, 0);
	write32(&mtk_pmicspi_mst->cslext_read, 0x100);

	/* Set Read Dummy Cycle Number (Slave Clock is 18MHz) */
	arb->write(arb, DEFAULT_SLVID, PMIC_DEW_RDDMY_NO, DUMMY_READ_CYCLES);
	write32(&mtk_pmicspi_mst->rddmy, DUMMY_READ_CYCLES);

	/* Enable DIO mode */
	arb->write(arb, DEFAULT_SLVID, PMIC_DEW_DIO_EN, 0x1);

	/* Wait for completion of sending the commands */
	if (check_idle(&arb->mtk_pmif->inf_busy_sta, PMIF_SPI_AP)) {
		printk(BIOS_ERR, "[%s] pmif channel busy, timeout\n", __func__);
		return;
	}

	if (check_idle(&arb->mtk_pmif->other_busy_sta_0, PMIF_CMD_STA)) {
		printk(BIOS_ERR, "[%s] pmif cmd busy, timeout\n", __func__);
		return;
	}

	if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
		printk(BIOS_ERR, "[%s] spi master busy, timeout\n", __func__);
		return;
	}

	write32(&mtk_pmicspi_mst->dio_en, 1);
}

static void init_spislv(struct pmif *arb)
{
	/* Turn on SPI IO filter function */
	arb->write(arb, DEFAULT_SLVID, PMIC_FILTER_CON0, SPI_FILTER);
	/* Turn on SPI IO SMT function to improve noise immunity */
	arb->write(arb, DEFAULT_SLVID, PMIC_SMT_CON1, SPI_SMT);
	/* Turn off SPI IO pull function for power saving */
	arb->write(arb, DEFAULT_SLVID, PMIC_GPIO_PULLEN0_CLR, SPI_PULL_DISABLE);
	/* Enable SPI access in SODI-3.0 and Suspend modes */
	arb->write(arb, DEFAULT_SLVID, PMIC_RG_SPI_CON0, 0x2);
	/* Set SPI IO driving strength to 4 mA */
	arb->write(arb, DEFAULT_SLVID, PMIC_DRV_CON1, SPI_DRIVING);
}

static int init_sistrobe(struct pmif *arb)
{
	u32 rdata = 0;
	int si_sample_ctrl;
	/* Random data for testing */
	const u32 test_data[30] = {
		0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996,
		0x9669, 0x6996, 0x9669, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
		0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x1B27,
		0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27,
		0x1B27, 0x1B27
	};

	for (si_sample_ctrl = 0; si_sample_ctrl < 16; si_sample_ctrl++) {
		write32(&mtk_pmicspi_mst->si_sampling_ctrl, si_sample_ctrl << 5);

		arb->read(arb, DEFAULT_SLVID, PMIC_DEW_READ_TEST, &rdata);
		if (rdata == DEFAULT_VALUE_READ_TEST)
			break;
	}

	if (si_sample_ctrl == 16)
		return E_CLK_EDGE;

	if (si_sample_ctrl == 15)
		return E_CLK_LAST_SETTING;

	/*
	 * Add the delay time of SPI data from PMIC to align the start boundary
	 * to current sampling clock edge.
	 */
	for (int si_dly = 0; si_dly < 10; si_dly++) {
		arb->write(arb, DEFAULT_SLVID, PMIC_RG_SPI_CON2, si_dly);

		int start_boundary_found = 0;
		for (int i = 0; i < ARRAY_SIZE(test_data); i++) {
			arb->write(arb, DEFAULT_SLVID, PMIC_DEW_WRITE_TEST, test_data[i]);
			arb->read(arb, DEFAULT_SLVID, PMIC_DEW_WRITE_TEST, &rdata);
			if ((rdata & 0x7fff) != (test_data[i] & 0x7fff)) {
				start_boundary_found = 1;
				break;
			}
		}
		if (start_boundary_found == 1)
			break;
	}

	/*
	 * Change the sampling clock edge to the next one which is the middle
	 * of SPI data window.
	 */
	write32(&mtk_pmicspi_mst->si_sampling_ctrl, ++si_sample_ctrl << 5);

	/* Read Test */
	arb->read(arb, DEFAULT_SLVID, PMIC_DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		printk(BIOS_ERR, "[%s] Failed for read test, data = %#x.\n",
			__func__, rdata);
		return E_READ_TEST_FAIL;
	}

	return 0;
}

static void init_staupd(struct pmif *arb)
{
	/* Unlock SPI Slave registers */
	arb->write(arb, DEFAULT_SLVID, PMIC_SPISLV_KEY, 0xbade);

	/* Enable CRC of PMIC 0 */
	arb->write(arb, DEFAULT_SLVID, PMIC_DEW_CRC_EN, 0x1);

	/* Wait for completion of sending the commands */
	if (check_idle(&arb->mtk_pmif->inf_busy_sta, PMIF_SPI_AP)) {
		printk(BIOS_ERR, "[%s] pmif channel busy, timeout\n", __func__);
		return;
	}

	if (check_idle(&arb->mtk_pmif->other_busy_sta_0, PMIF_CMD_STA)) {
		printk(BIOS_ERR, "[%s] pmif cmd busy, timeout\n", __func__);
		return;
	}

	if (check_idle(&mtk_pmicspi_mst->other_busy_sta_0, SPIMST_STA)) {
		printk(BIOS_ERR, "[%s] spi master busy, timeout\n", __func__);
		return;
	}

	/* Configure CRC of PMIC Interface */
	write32(&arb->mtk_pmif->crc_ctrl, 0x1);
	write32(&arb->mtk_pmif->sig_mode, 0x0);

	/* Lock SPI Slave registers */
	arb->write(arb, DEFAULT_SLVID, PMIC_SPISLV_KEY, 0x0);

	/* Set up PMIC Siganature */
	write32(&arb->mtk_pmif->pmic_sig_addr, PMIC_DEW_CRC_VAL);

	/* Set up PMIC EINT */
	write32(&arb->mtk_pmif->pmic_eint_sta_addr, PMIC_INT_STA);

	SET32_BITFIELDS(&arb->mtk_pmif->staupd_ctrl,
			STAUPD_CTRL_PRD, 5,
			STAUPD_CTRL_PMIC0_SIG_STA, 1,
			STAUPD_CTRL_PMIC0_EINT_STA, 1);
}

int pmif_spi_init(struct pmif *arb)
{
	pmif_spi_config(arb);

	/* Reset spislv */
	if (reset_spislv())
		return E_SPI_INIT_RESET_SPI;

	/* Enable WRAP */
	write32(&mtk_pmicspi_mst->wrap_en, 0x1);

	/* SPI Waveform Configuration */
	init_reg_clock(arb);

	/* SPI Slave Configuration */
	init_spislv(arb);

	/* Input data calibration flow; */
	if (init_sistrobe(arb)) {
		printk(BIOS_ERR, "[%s] data calibration fail\n", __func__);
		return E_SPI_INIT_SIDLY;
	}

	/* Lock SPISLV Registers */
	arb->write(arb, DEFAULT_SLVID, PMIC_SPISLV_KEY, 0x0);

	/*
	 * Status update function initialization
	 * 1. Check signature using CRC (CRC 0 only)
	 * 2. Update EINT
	 * 3. Read back AUXADC thermal data for GPS
	 */
	init_staupd(arb);

	/* Configure PMIF Timer */
	write32(&arb->mtk_pmif->timer_ctrl, 0x3);

	/* Enable interfaces and arbitration */
	write32(&arb->mtk_pmif->inf_en, PMIF_SPI_HW_INF | PMIF_SPI_MD |
		PMIF_SPI_AP_SECURE | PMIF_SPI_AP);

	write32(&arb->mtk_pmif->arb_en, PMIF_SPI_HW_INF | PMIF_SPI_MD | PMIF_SPI_AP_SECURE |
		PMIF_SPI_AP | PMIF_SPI_STAUPD | PMIF_SPI_TSX_HW | PMIF_SPI_DCXO_HW);

	/* Enable GPS AUXADC HW 0 and 1 */
	SET32_BITFIELDS(&arb->mtk_pmif->other_inf_en, INTGPSADCINF_EN, 0x3);

	/* Set INIT_DONE */
	write32(&arb->mtk_pmif->init_done, 0x1);

	return 0;
}
