/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <assert.h>
#include <console/console.h>
#include <delay.h>
#include <soc/infracfg.h>
#include <soc/pmic_wrap.h>
#include <timer.h>

#define PWRAPTAG                "[PWRAP] "
#define pwrap_log(fmt, arg ...)   printk(BIOS_INFO, PWRAPTAG fmt, ## arg)
#define pwrap_err(fmt, arg ...)   printk(BIOS_ERR, PWRAPTAG "ERROR,line=%d" fmt, \
					__LINE__, ## arg)

/* define macro and inline function (for do while loop) */

typedef u32 (*loop_condition_fp)(u32);

static inline u32 wait_for_fsm_vldclr(u32 x)
{
	return ((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_WFVLDCLR;
}

static inline u32 wait_for_sync(u32 x)
{
	return ((x >> RDATA_SYNC_IDLE_SHIFT) & RDATA_SYNC_IDLE_MASK) !=
		WACS_SYNC_IDLE;
}

static inline u32 wait_for_idle_and_sync(u32 x)
{
	return ((((x >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		WACS_FSM_IDLE) || (((x >> RDATA_SYNC_IDLE_SHIFT) &
		RDATA_SYNC_IDLE_MASK)!= WACS_SYNC_IDLE));
}

static inline u32 wait_for_cipher_ready(u32 x)
{
	return x != 3;
}

static inline u32 wait_for_state_idle(u32 timeout_us, void *wacs_register,
				      void *wacs_vldclr_register,
				      u32 *read_reg)
{
	u32 reg_rdata;

	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, timeout_us);
	do {
		reg_rdata = read32((wacs_register));
		/* if last read command timeout,clear vldclr bit
		   read command state machine:FSM_REQ-->wfdle-->WFVLDCLR;
		   write:FSM_REQ-->idle */
		switch (((reg_rdata >> RDATA_WACS_FSM_SHIFT) &
			RDATA_WACS_FSM_MASK)) {
		case WACS_FSM_WFVLDCLR:
			write32(wacs_vldclr_register, 1);
			pwrap_err("WACS_FSM = PMIC_WRAP_WACS_VLDCLR\n");
			break;
		case WACS_FSM_WFDLE:
			pwrap_err("WACS_FSM = WACS_FSM_WFDLE\n");
			break;
		case WACS_FSM_REQ:
			pwrap_err("WACS_FSM = WACS_FSM_REQ\n");
			break;
		default:
			break;
		}

		if (stopwatch_expired(&sw))
			return E_PWR_WAIT_IDLE_TIMEOUT;

	} while (((reg_rdata >> RDATA_WACS_FSM_SHIFT) & RDATA_WACS_FSM_MASK) !=
		 WACS_FSM_IDLE);  /* IDLE State */
	if (read_reg)
		*read_reg = reg_rdata;
	return 0;
}

static inline u32 wait_for_state_ready(loop_condition_fp fp, u32 timeout_us,
				       void *wacs_register, u32 *read_reg)
{
	u32 reg_rdata;
	struct stopwatch sw;

	stopwatch_init_usecs_expire(&sw, timeout_us);
	do {
		reg_rdata = read32((wacs_register));

		if (stopwatch_expired(&sw)) {
			pwrap_err("timeout when waiting for idle\n");
			return E_PWR_WAIT_IDLE_TIMEOUT;
		}
	} while (fp(reg_rdata));  /* IDLE State */
	if (read_reg)
		*read_reg = reg_rdata;
	return 0;
}

s32 pwrap_wacs2(u32 write, u16 adr, u16 wdata, u16 *rdata, u32 init_check)
{
	u32 reg_rdata = 0;
	u32 wacs_write = 0;
	u32 wacs_adr = 0;
	u32 wacs_cmd = 0;
	u32 return_value = 0;

	if (init_check) {
		reg_rdata = read32(&mt8173_pwrap->wacs2_rdata);
		/* Prevent someone to used pwrap before pwrap init */
		if (((reg_rdata >> RDATA_INIT_DONE_SHIFT) &
		    RDATA_INIT_DONE_MASK) != WACS_INIT_DONE) {
			pwrap_err("initialization isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
	}
	reg_rdata = 0;
	/* Check IDLE in advance */
	return_value = wait_for_state_idle(TIMEOUT_WAIT_IDLE_US,
					   &mt8173_pwrap->wacs2_rdata,
					   &mt8173_pwrap->wacs2_vldclr,
					   0);
	if (return_value != 0) {
		pwrap_err("wait_for_fsm_idle fail,return_value=%d\n",
			  return_value);
		return E_PWR_WAIT_IDLE_TIMEOUT;
	}
	wacs_write = write << 31;
	wacs_adr = (adr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_adr | wdata;

	write32(&mt8173_pwrap->wacs2_cmd, wacs_cmd);
	if (write == 0) {
		if (NULL == rdata) {
			pwrap_err("rdata is a NULL pointer\n");
			return E_PWR_INVALID_ARG;
		}
		return_value = wait_for_state_ready(wait_for_fsm_vldclr,
						    TIMEOUT_READ_US,
						    &mt8173_pwrap->wacs2_rdata,
						    &reg_rdata);
		if (return_value != 0) {
			pwrap_err("wait_for_fsm_vldclr fail,return_value=%d\n",
				  return_value);
			return E_PWR_WAIT_IDLE_TIMEOUT_READ;
		}
		*rdata = ((reg_rdata >> RDATA_WACS_RDATA_SHIFT)
			  & RDATA_WACS_RDATA_MASK);
		write32(&mt8173_pwrap->wacs2_vldclr, 1);
	}

	return 0;
}

/* external API for pmic_wrap user */

s32 pwrap_read(u16 adr, u16 *rdata)
{
	return pwrap_wacs2(0, adr, 0, rdata, 1);
}

s32 pwrap_write(u16 adr, u16 wdata)
{
	return pwrap_wacs2(1, adr, wdata, 0, 1);
}

static s32 pwrap_read_nochk(u16 adr, u16 *rdata)
{
	return pwrap_wacs2(0, adr, 0, rdata, 0);
}

static s32 pwrap_write_nochk(u16 adr, u16 wdata)
{
	return pwrap_wacs2(1, adr, wdata, 0, 0);
}

/* call it in pwrap_init,mustn't check init done */
static s32 pwrap_init_dio(u32 dio_en)
{
	u16 rdata = 0;
	u32 return_value = 0;

	pwrap_write_nochk(DEW_DIO_EN, dio_en);

	/* Check IDLE in advance */
	return_value =
	wait_for_state_ready(wait_for_idle_and_sync,
			     TIMEOUT_WAIT_IDLE_US,
			     &mt8173_pwrap->wacs2_rdata,
			     0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%#x\n", __func__, return_value);
		return return_value;
	}
	write32(&mt8173_pwrap->dio_en, dio_en);
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
		write32(&mt8173_pwrap->sidly, i);
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

	write32(&mt8173_pwrap->sidly, sidly);

	return 0;
}

static s32 pwrap_reset_spislv(void)
{
	u32 ret = 0;
	u32 return_value = 0;

	write32(&mt8173_pwrap->hiprio_arb_en, 0);
	write32(&mt8173_pwrap->wrap_en, 0);
	write32(&mt8173_pwrap->mux_sel, 1);
	write32(&mt8173_pwrap->man_en, 1);
	write32(&mt8173_pwrap->dio_en, 0);

	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_CSL << 8));
	/* to reset counter */
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_CSH << 8));
	/*
	 * In order to pull CSN signal to PMIC,
	 * PMIC will count it then reset spi slave
	*/
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mt8173_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));

	return_value = wait_for_state_ready(wait_for_sync,
					    TIMEOUT_WAIT_IDLE_US,
					    &mt8173_pwrap->wacs2_rdata, 0);
	if (return_value != 0) {
		pwrap_err("%s fail,return_value=%#x\n", __func__, return_value);
		ret = E_PWR_TIMEOUT;
	}

	write32(&mt8173_pwrap->man_en, 0);
	write32(&mt8173_pwrap->mux_sel, 0);

	return ret;
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
		pwrap_err("pwrap_init_reg_clock,rdata=%#x\n", rdata);
		return E_PWR_INIT_REG_CLOCK;
	}
	/* Config SPI Waveform according to reg clk */
	switch (regck_sel) {
	case REG_CLOCK_18MHZ:
		write32(&mt8173_pwrap->rddmy, 0xc);
		write32(&mt8173_pwrap->cshext_write, 0x0);
		write32(&mt8173_pwrap->cshext_read, 0x4);
		write32(&mt8173_pwrap->cslext_start, 0x0);
		write32(&mt8173_pwrap->cslext_end, 0x4);
		break;
	case REG_CLOCK_26MHZ:
		write32(&mt8173_pwrap->rddmy, 0xc);
		write32(&mt8173_pwrap->cshext_write, 0x0);
		write32(&mt8173_pwrap->cshext_read, 0x4);
		write32(&mt8173_pwrap->cslext_start, 0x2);
		write32(&mt8173_pwrap->cslext_end, 0x2);
		break;
	default:
		write32(&mt8173_pwrap->rddmy, 0xf);
		write32(&mt8173_pwrap->cshext_write, 0xf);
		write32(&mt8173_pwrap->cshext_read, 0xf);
		write32(&mt8173_pwrap->cslext_start, 0xf);
		write32(&mt8173_pwrap->cslext_end, 0xf);
		break;
	}

	return 0;
}

s32 pwrap_init(void)
{
	s32 sub_return = 0;
	s32 sub_return1 = 0;
	u16 rdata = 0x0;

	setbits_le32(&mt8173_infracfg->infra_rst0, INFRA_PMIC_WRAP_RST);
	/* add 1us delay for toggling SW reset */
	udelay(1);
	/* clear reset bit */
	clrbits_le32(&mt8173_infracfg->infra_rst0, INFRA_PMIC_WRAP_RST);

	/* Enable DCM */
	write32(&mt8173_pwrap->dcm_en, 3);
	write32(&mt8173_pwrap->dcm_dbc_prd, 0);

	/* Reset SPISLV */
	sub_return = pwrap_reset_spislv();
	if (sub_return != 0) {
		pwrap_err("error,pwrap_reset_spislv fail,sub_return=%#x\n",
			  sub_return);
		return E_PWR_INIT_RESET_SPI;
	}
	/* Enable WACS2 */
	write32(&mt8173_pwrap->wrap_en, 1);
	write32(&mt8173_pwrap->hiprio_arb_en, WACS2);
	write32(&mt8173_pwrap->wacs2_en, 1);

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
	write32(&mt8173_pwrap->crc_en, 0x1);
	write32(&mt8173_pwrap->sig_mode, 0x0);
	write32(&mt8173_pwrap->sig_adr, DEW_CRC_VAL);

	/* PMIC_WRAP enables */
	write32(&mt8173_pwrap->hiprio_arb_en, 0x1ff);
	write32(&mt8173_pwrap->wacs0_en, 0x1);
	write32(&mt8173_pwrap->wacs1_en, 0x1);

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
	write32(&mt8173_pwrap->init_done2, 0x1);
	write32(&mt8173_pwrap->init_done0, 0x1);
	write32(&mt8173_pwrap->init_done1, 0x1);

	return 0;
}
