/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <console/console.h>
#include <delay.h>
#include <soc/infracfg.h>
#include <soc/pmic_wrap.h>

static s32 pwrap_init_dio(u32 dio_en)
{
	u16 rdata = 0;
	u32 return_value = 0;

	pwrap_write_nochk(DEW_DIO_EN, dio_en);

	/* Check IDLE in advance */
	return_value =
	wait_for_state_ready(wait_for_idle_and_sync,
			     TIMEOUT_WAIT_IDLE_US,
			     &mtk_pwrap->wacs2_rdata,
			     0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%#x\n", __func__, return_value);
		return return_value;
	}
	write32(&mtk_pwrap->dio_en, dio_en);
	/* Read Test */
	pwrap_read_nochk(DEW_READ_TEST, &rdata);
	if (rdata != DEFAULT_VALUE_READ_TEST) {
		pwrap_err("fail,dio_en = %#x, READ_TEST rdata=%#x\n", dio_en,
			  rdata);
		return E_PWR_READ_TEST_FAIL;
	}

	return 0;
}

/*
 * pwrap_init_sidly - configure serial input delay
 *
 * This configures the serial input delay. We can configure 0, 2, 4 or 6ns
 * delay. Do a read test with all possible values and chose the best delay.
 */
static s32 pwrap_init_sidly(void)
{
	u16 rdata;
	u32 i;
	u32 pass = 0;
	u32 sidly = 0;

	for (i = 0; i < 4; i++) {
		write32(&mtk_pwrap->sidly, i);
		pwrap_wacs2(0, DEW_READ_TEST, 0, &rdata, 0);
		if (rdata == DEFAULT_VALUE_READ_TEST)
			pass |= 1 << i;
	}

	/*
	 * Config SIDLY according to results
	 * Pass range should be continuously or return failed
	 */
	switch (pass) {
	/* only 1 pass, choose it */
	case 1 << 0:
		sidly = 0;
		break;
	case 1 << 1:
		sidly = 1;
		break;
	case 1 << 2:
		sidly = 2;
		break;
	case 1 << 3:
		sidly = 3;
		break;
	/* two pass, choose the one on SIDLY boundary */
	case (1 << 0) | (1 << 1):
		sidly = 0;
		break;
	case (1 << 1) | (1 << 2): /* no boundary, choose smaller one */
		sidly = 1;
		break;
	case (1 << 2) | (1 << 3):
		sidly = 3;
		break;
	/* three pass, choose the middle one */
	case (1 << 0) | (1 << 1) | (1 << 2):
		sidly = 1;
		break;
	case (1 << 1) | (1 << 2) | (1 << 3):
		sidly = 2;
		break;
	/* four pass, choose the smaller middle one */
	case (1 << 0) | (1 << 1) | (1 << 2) | (1 << 3):
		sidly = 1;
		break;
	/* pass range not continuous, should not happen */
	default:
		die("sidly pass range not continuous\n");
	}

	write32(&mtk_pwrap->sidly, sidly);

	return 0;
}

static s32 pwrap_init_reg_clock(enum pmic_regck regck_sel)
{
	u16 wdata = 0;
	u16 rdata = 0;

	/* Set reg clk freq */
	pwrap_read_nochk(PMIC_TOP_CKCON2, &rdata);

	if (regck_sel == REG_CLOCK_18MHZ)
		wdata = (rdata & (~(0x3 << 10))) | (0x1 << 10);
	else
		wdata = rdata & (~(0x3 << 10));

	pwrap_write_nochk(PMIC_TOP_CKCON2, wdata);
	pwrap_read_nochk(PMIC_TOP_CKCON2, &rdata);
	if (rdata != wdata) {
		pwrap_err("%s,rdata=%#x\n", __func__, rdata);
		return E_PWR_INIT_REG_CLOCK;
	}
	/* Config SPI Waveform according to reg clk */
	switch (regck_sel) {
	case REG_CLOCK_18MHZ:
		write32(&mtk_pwrap->rddmy, 0xc);
		write32(&mtk_pwrap->cshext_write, 0x0);
		write32(&mtk_pwrap->cshext_read, 0x4);
		write32(&mtk_pwrap->cslext_start, 0x0);
		write32(&mtk_pwrap->cslext_end, 0x4);
		break;
	case REG_CLOCK_26MHZ:
		write32(&mtk_pwrap->rddmy, 0xc);
		write32(&mtk_pwrap->cshext_write, 0x0);
		write32(&mtk_pwrap->cshext_read, 0x4);
		write32(&mtk_pwrap->cslext_start, 0x2);
		write32(&mtk_pwrap->cslext_end, 0x2);
		break;
	default:
		write32(&mtk_pwrap->rddmy, 0xf);
		write32(&mtk_pwrap->cshext_write, 0xf);
		write32(&mtk_pwrap->cshext_read, 0xf);
		write32(&mtk_pwrap->cslext_start, 0xf);
		write32(&mtk_pwrap->cslext_end, 0xf);
		break;
	}

	return 0;
}

s32 pwrap_init(void)
{
	s32 sub_return = 0;
	s32 sub_return1 = 0;
	u16 rdata = 0x0;

	setbits32(&mt8173_infracfg->infra_rst0, INFRA_PMIC_WRAP_RST);
	/* add 1us delay for toggling SW reset */
	udelay(1);
	/* clear reset bit */
	clrbits32(&mt8173_infracfg->infra_rst0, INFRA_PMIC_WRAP_RST);

	/* Enable DCM */
	write32(&mtk_pwrap->dcm_en, 3);
	write32(&mtk_pwrap->dcm_dbc_prd, 0);

	/* Reset SPISLV */
	sub_return = pwrap_reset_spislv();
	if (sub_return != 0) {
		pwrap_err("error,pwrap_reset_spislv fail,sub_return=%#x\n",
			  sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	/* Enable WACS2 */
	write32(&mtk_pwrap->wrap_en, 1);
	write32(&mtk_pwrap->hiprio_arb_en, WACS2);
	write32(&mtk_pwrap->wacs2_en, 1);

	/* SIDLY setting */
	sub_return = pwrap_init_sidly();
	if (sub_return != 0) {
		pwrap_err("error,pwrap_init_sidly fail,sub_return=%#x\n",
			  sub_return);
		return E_PWR_INIT_SIDLY;
	}
	/*
	 * SPI Waveform Configuration
	 * 18MHz/26MHz/safe mode/
	 */
	sub_return = pwrap_init_reg_clock(REG_CLOCK_26MHZ);
	if (sub_return != 0) {
		pwrap_err("error,pwrap_init_reg_clock fail,sub_return=%#x\n",
			  sub_return);
		return E_PWR_INIT_REG_CLOCK;
	}
	/*
	 * Enable PMIC
	 */
	pwrap_read_nochk(PMIC_WRP_CKPDN, &rdata);
	sub_return = pwrap_write_nochk(PMIC_WRP_CKPDN, rdata & 0x50);
	/* clear dewrap reset bit */
	sub_return1 = pwrap_write_nochk(PMIC_WRP_RST_CON, 0);
	if ((sub_return != 0) || (sub_return1 != 0)) {
		pwrap_err("Enable PMIC fail, sub_return=%#x sub_return1=%#x\n",
			  sub_return, sub_return1);
		return E_PWR_INIT_ENABLE_PMIC;
	}
	/* Enable DIO mode */
	sub_return = pwrap_init_dio(1);
	if (sub_return != 0) {
		pwrap_err("pwrap_init_dio error code=%#x, sub_return=%#x\n",
			 0x11, sub_return);
		return E_PWR_INIT_DIO;
	}

	/*
	 * Write test using WACS2,
	 * make sure the read/write function ready
	 */
	sub_return = pwrap_write_nochk(DEW_WRITE_TEST, WRITE_TEST_VALUE);
	sub_return1 = pwrap_read_nochk(DEW_WRITE_TEST, &rdata);
	if ((rdata != WRITE_TEST_VALUE) || (sub_return != 0)
	    || (sub_return1 != 0)) {
		pwrap_err("write error, rdata=%#x, return=%#x, return1=%#x\n",
			  rdata, sub_return, sub_return1);
		return E_PWR_INIT_WRITE_TEST;
	}

	/* Signature Checking - Using CRC
	 * should be the last to modify WRITE_TEST
	 */
	sub_return = pwrap_write_nochk(DEW_CRC_EN, 0x1);
	if (sub_return != 0) {
		pwrap_err("enable CRC fail,sub_return=%#x\n", sub_return);
		return E_PWR_INIT_ENABLE_CRC;
	}
	write32(&mtk_pwrap->crc_en, 0x1);
	write32(&mtk_pwrap->sig_mode, 0x0);
	write32(&mtk_pwrap->sig_adr, DEW_CRC_VAL);

	/* PMIC_WRAP enables */
	write32(&mtk_pwrap->hiprio_arb_en, 0x1ff);
	write32(&mtk_pwrap->wacs0_en, 0x1);
	write32(&mtk_pwrap->wacs1_en, 0x1);

	/*
	 * switch event pin from usbdl mode to normal mode for pmic interrupt,
	 * NEW@MT6397
	 */
	pwrap_read_nochk(PMIC_TOP_CKCON3, &rdata);
	sub_return = pwrap_write_nochk(PMIC_TOP_CKCON3, (rdata & 0x0007));
	if (sub_return != 0)
		pwrap_err("!!switch event pin fail,sub_return=%d\n",
			  sub_return);

	/* Initialization Done */
	write32(&mtk_pwrap->init_done2, 0x1);
	write32(&mtk_pwrap->init_done0, 0x1);
	write32(&mtk_pwrap->init_done1, 0x1);

	return 0;
}
