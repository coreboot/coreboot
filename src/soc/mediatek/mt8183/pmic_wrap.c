/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/infracfg.h>
#include <soc/pll.h>
#include <soc/pmic_wrap.h>
#include <timer.h>

#define PRIORITY_FIELD(x)		((x % 4) * 8)
#define PRIORITY_IN(id, priority)	(id << PRIORITY_FIELD(priority))
#define PRIORITY_OUT(id, priority)	(priority << PRIORITY_FIELD(id))

enum {
	MD_ADCINF0 =  8,
	MD_ADCINF1 =  9,
	STAUPD     = 10,
	GPSINF0    = 11,

	PRIORITY_IN_SEL_2 = PRIORITY_IN(MD_ADCINF0,  9) |
			    PRIORITY_IN(MD_ADCINF1, 10) |
			    PRIORITY_IN(STAUPD,      8) |
			    PRIORITY_IN(GPSINF0,    11),

	PRIORITY_OUT_SEL_2 = PRIORITY_OUT(MD_ADCINF0,  9) |
			     PRIORITY_OUT(MD_ADCINF1, 10) |
			     PRIORITY_OUT(STAUPD,      8) |
			     PRIORITY_OUT(GPSINF0,    11),
};

#define PENDING_US(x) x
enum {
	STARVE_ENABLE = 0x1 << 10,
	COUNTER0_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x2),
	COUNTER1_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x3),
	COUNTER2_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x3),
	COUNTER3_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x3),
	COUNTER4_PENDING_THRES = STARVE_ENABLE | PENDING_US(0xf),
	COUNTER5_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x20),
	COUNTER6_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x28),
	COUNTER7_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x28),
	COUNTER8_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x13),
	COUNTER9_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x17),
	COUNTER10_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x17),
	COUNTER11_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x7c),
	COUNTER12_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x7c),
	COUNTER13_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x340),
	COUNTER16_PENDING_THRES = STARVE_ENABLE | PENDING_US(0x340),
};

static void pwrap_soft_reset(void)
{
	write32(&mt8183_infracfg->infra_globalcon_rst2_set, 0x1);
	write32(&mt8183_infracfg->infra_globalcon_rst2_clr, 0x1);
}

static void pwrap_spi_clk_set(void)
{
	write32(&mtk_topckgen->clk_cfg_5_clr,
		ULPOSC_OFF | ULPOSC_INV | ULPOSC_SEL_1 | ULPOSC_SEL_2);
	write32(&mtk_topckgen->clk_cfg_5_set, ULPOSC_SEL_1);
	write32(&mtk_topckgen->clk_cfg_update, ULPOSC_CLK);

	write32(&mt8183_infracfg->module_sw_cg_0_set,
		TIMER_CG | AP_CG | MD_CG | CONN_CG);
	write32(&mt8183_infracfg->module_sw_cg_2_set, MODEM_TEMP_SHARE_CG);

	pwrap_soft_reset();

	write32(&mt8183_infracfg->module_sw_cg_0_clr,
		TIMER_CG | AP_CG | MD_CG | CONN_CG);
	write32(&mt8183_infracfg->module_sw_cg_2_clr, MODEM_TEMP_SHARE_CG);
}

static s32 pwrap_init_dio(u16 dio_en)
{
	pwrap_write_nochk(PMIC_DEW_DIO_EN, dio_en);

	if (!wait_us(100,
		     !wait_for_idle_and_sync(read32(&mtk_pwrap->wacs2_rdata))))
		return -1;

	write32(&mtk_pwrap->dio_en, dio_en);
	return 0;
}

static void pwrap_lock_spislvreg(void)
{
	pwrap_write_nochk(PMIC_SPISLV_KEY, 0x0);
}

static void pwrap_initstaupd(void)
{
	write32(&mtk_pwrap->staupd_grpen,
		SIG_PMIC_0 | INT_STA_PMIC_0 | MD_ADC_DATA0 |
		MD_ADC_DATA1 | GPS_ADC_DATA0 | GPS_ADC_DATA1);

	/* CRC */
	pwrap_write_nochk(PMIC_DEW_CRC_EN, 0x1);
	write32(&mtk_pwrap->crc_en, 0x1);
	write32(&mtk_pwrap->sig_adr, PMIC_DEW_CRC_VAL);

	write32(&mtk_pwrap->eint_sta0_adr, PMIC_CPU_INT_STA);

	/* MD ADC Interface */
	write32(&mtk_pwrap->md_auxadc_rdata_latest_addr,
		(PMIC_AUXADC_ADC35 << 16) + PMIC_AUXADC_ADC31);
	write32(&mtk_pwrap->md_auxadc_rdata_wp_addr,
		(PMIC_AUXADC_ADC35 << 16) + PMIC_AUXADC_ADC31);
	for (size_t i = 0; i < 32; i++)
		write32(&mtk_pwrap->md_auxadc_rdata[i],
			(PMIC_AUXADC_ADC35 << 16) + PMIC_AUXADC_ADC31);

	write32(&mtk_pwrap->int_gps_auxadc_cmd_addr,
		(PMIC_AUXADC_RQST1 << 16) + PMIC_AUXADC_RQST0);
	write32(&mtk_pwrap->int_gps_auxadc_cmd, (GPS_MAIN << 16) + GPS_SUBSYS);
	write32(&mtk_pwrap->int_gps_auxadc_rdata_addr,
		(PMIC_AUXADC_ADC32 << 16) + PMIC_AUXADC_ADC17);

	write32(&mtk_pwrap->ext_gps_auxadc_rdata_addr, PMIC_AUXADC_ADC31);
}

static void pwrap_starve_set(void)
{
	write32(&mtk_pwrap->harb_hprio, ARB_PRIORITY);
	write32(&mtk_pwrap->starv_counter_0, COUNTER0_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_1, COUNTER1_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_2, COUNTER2_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_3, COUNTER3_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_4, COUNTER4_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_5, COUNTER5_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_6, COUNTER6_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_7, COUNTER7_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_8, COUNTER8_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_9, COUNTER9_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_10, COUNTER10_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_11, COUNTER11_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_12, COUNTER12_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_13, COUNTER13_PENDING_THRES);
	write32(&mtk_pwrap->starv_counter_16, COUNTER16_PENDING_THRES);
}

static void pwrap_enable(void)
{
	write32(&mtk_pwrap->hiprio_arb_en, ARB_USER_EN);
	write32(&mtk_pwrap->wacs0_en, 0x1);
	write32(&mtk_pwrap->wacs2_en, 0x1);
	write32(&mtk_pwrap->wacs_p2p_en, 0x1);
	write32(&mtk_pwrap->wacs_md32_en, 0x1);
	write32(&mtk_pwrap->staupd_ctrl, STA_PD_98_5_US);
	write32(&mtk_pwrap->wdt_unit, WATCHDOG_TIMER_7_5_MS);
	write32(&mtk_pwrap->wdt_src_en_0, WDT_MONITOR_ALL);
	write32(&mtk_pwrap->wdt_src_en_1, WDT_MONITOR_ALL);
	write32(&mtk_pwrap->timer_en, 0x1);
	write32(&mtk_pwrap->int0_en, INT0_MONITOR);
	write32(&mtk_pwrap->int1_en, INT1_MONITOR);
}

static s32 pwrap_init_sistrobe(void)
{
	u16 rdata;
	int si_sample_ctrl;
	int test_data[30] = {
		0x6996, 0x9669, 0x6996, 0x9669, 0x6996, 0x9669, 0x6996,
		0x9669, 0x6996, 0x9669, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A,
		0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x5AA5, 0xA55A, 0x1B27,
		0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27, 0x1B27,
		0x1B27, 0x1B27};

	for (si_sample_ctrl = 0; si_sample_ctrl < 16; si_sample_ctrl++) {
		write32(&mtk_pwrap->si_sample_ctrl, si_sample_ctrl << 5);

		pwrap_read_nochk(PMIC_DEW_READ_TEST, &rdata);
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
		pwrap_write_nochk(PMIC_RG_SPI_CON2, si_dly);

		int start_boundary_found = 0;
		for (size_t i = 0; i < 30; i++) {
			pwrap_write_nochk(PMIC_DEW_WRITE_TEST, test_data[i]);
			pwrap_read_nochk(PMIC_DEW_WRITE_TEST, &rdata);
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
	write32(&mtk_pwrap->si_sample_ctrl, ++si_sample_ctrl << 5);

	/* Read Test */
	pwrap_read_nochk(PMIC_DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		pwrap_err("rdata = %#x, exp = %#x\n", rdata,
			  DEFAULT_VALUE_READ_TEST);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

static void pwrap_init_spislv(void)
{
	/* Turn on IO filter function */
	pwrap_write_nochk(PMIC_FILTER_CON0, SPI_FILTER);
	/* Turn on IO SMT function to improve noise immunity */
	pwrap_write_nochk(PMIC_SMT_CON1, SPI_SMT);
	/* Turn off IO pull function for power saving */
	pwrap_write_nochk(PMIC_GPIO_PULLEN0_CLR, SPI_PULL_DISABLE);
	/* Enable SPI R/W in suspend mode */
	pwrap_write_nochk(PMIC_RG_SPI_CON0, 0x1);
	/* Set PMIC GPIO driving current to 4mA */
	pwrap_write_nochk(PMIC_DRV_CON1, SPI_DRIVING);
}

static void pwrap_init_reg_clock(void)
{
	write32(&mtk_pwrap->ext_ck_write, 0x1);

	pwrap_write_nochk(PMIC_DEW_RDDMY_NO, DUMMY_READ_CYCLES);
	write32(&mtk_pwrap->rddmy, DUMMY_READ_CYCLES);

	write32(&mtk_pwrap->cshext_write, 0);
	write32(&mtk_pwrap->cshext_read, 0);
	write32(&mtk_pwrap->cslext_write, 0);
	write32(&mtk_pwrap->cslext_read, 0);
}

s32 pwrap_init(void)
{
	s32 sub_return = 0, sub_return1 = 0;
	u16 rdata;

	pwrap_spi_clk_set();

	/* Reset spislv */
	sub_return = pwrap_reset_spislv();
	if (sub_return != 0) {
		pwrap_err("reset_spislv fail, ret=%d\n", sub_return);
		return E_PWR_INIT_RESET_SPI;
	}

	/* Enable WRAP */
	write32(&mtk_pwrap->wrap_en, 0x1);

	/* Enable WACS2 */
	write32(&mtk_pwrap->wacs2_en, 0x1);
	write32(&mtk_pwrap->hiprio_arb_en, WACS2); /* ONLY WACS2 */

	/* SPI Waveform Configuration */
	pwrap_init_reg_clock();

	/* SPI Slave Configuration */
	pwrap_init_spislv();

	/* Enable DIO mode */
	sub_return = pwrap_init_dio(1);
	if (sub_return != 0) {
		pwrap_err("dio test error, ret=%d\n", sub_return);
		return E_PWR_INIT_DIO;
	}

	/* Input data calibration flow; */
	sub_return = pwrap_init_sistrobe();
	if (sub_return != 0) {
		pwrap_err("InitSiStrobe fail,ret=%d\n", sub_return);
		return E_PWR_INIT_SIDLY;
	}

	/*
	 * Write test using WACS2,
	 * make sure the read/write function ready.
	 */
	sub_return = pwrap_write_nochk(PMIC_DEW_WRITE_TEST, WRITE_TEST_VALUE);
	sub_return1 = pwrap_read_nochk(PMIC_DEW_WRITE_TEST, &rdata);
	if (rdata != WRITE_TEST_VALUE || sub_return || sub_return1) {
		pwrap_err("write error, rdata=%#x, return=%d, return1=%d\n",
			  rdata, sub_return, sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}

	/*
	 * Status update function initialization
	 * 1. Signature Checking using CRC (CRC 0 only)
	 * 2. EINT update
	 * 3. Read back Auxadc thermal data for GPS
	 */
	pwrap_initstaupd();

	write32(&mtk_pwrap->priority_user_sel_2, PRIORITY_IN_SEL_2);
	write32(&mtk_pwrap->arbiter_out_sel_2, PRIORITY_OUT_SEL_2);

	pwrap_starve_set();

	pwrap_enable();

	/* Initialization Done */
	write32(&mtk_pwrap->init_done0, 0x1);
	write32(&mtk_pwrap->init_done2, 0x1);
	write32(&mtk_pwrap->init_done_p2p, 0x1);
	write32(&mtk_pwrap->init_done_md32, 0x1);

	/* Lock SPISLV Registers */
	pwrap_lock_spislvreg();

	return 0;
}
