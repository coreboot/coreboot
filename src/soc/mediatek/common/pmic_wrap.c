/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/pmic_wrap.h>
#include <timer.h>

u32 wait_for_state_idle(u32 timeout_us, void *wacs_register,
			void *wacs_vldclr_register, u32 *read_reg)
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

u32 wait_for_state_ready(loop_condition_fp fp, u32 timeout_us,
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

s32 pwrap_reset_spislv(void)
{
	u32 ret = 0;

	write32(&mtk_pwrap->hiprio_arb_en, 0);
	write32(&mtk_pwrap->wrap_en, 0);
	write32(&mtk_pwrap->mux_sel, 1);
	write32(&mtk_pwrap->man_en, 1);
	write32(&mtk_pwrap->dio_en, 0);

	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_CSL << 8));
	/* Reset counter */
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_CSH << 8));
	/*
	 * In order to pull CSN signal to PMIC,
	 * PMIC will count it then reset spi slave
	 */
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));
	write32(&mtk_pwrap->man_cmd, (OP_WR << 13) | (OP_OUTS << 8));

	if (wait_for_state_ready(wait_for_sync, TIMEOUT_WAIT_IDLE_US,
				 &mtk_pwrap->wacs2_rdata, 0))
		ret = E_PWR_TIMEOUT;

	write32(&mtk_pwrap->man_en, 0);
	write32(&mtk_pwrap->mux_sel, 0);

	return ret;
}

s32 pwrap_wacs2(u32 write, u16 addr, u16 wdata, u16 *rdata, u32 init_check)
{
	u32 reg_rdata = 0;
	u32 wacs_write = 0;
	u32 wacs_addr = 0;
	u32 wacs_cmd = 0;
	u32 wait_result = 0;
	u32 shift;

	if (init_check) {
		reg_rdata = read32(&mtk_pwrap->wacs2_rdata);
		/* Prevent someone to use pwrap before pwrap init */
		if (CONFIG(SOC_MEDIATEK_MT8186))
			shift = RDATA_INIT_DONE_V2_SHIFT;
		else
			shift = RDATA_INIT_DONE_V1_SHIFT;

		if (((reg_rdata >> shift) & RDATA_INIT_DONE_MASK) != WACS_INIT_DONE) {
			pwrap_err("Pwrap initialization isn't finished\n");
			return E_PWR_NOT_INIT_DONE;
		}
	}
	reg_rdata = 0;
	/* Check IDLE in advance */
	wait_result = wait_for_state_idle(TIMEOUT_WAIT_IDLE_US,
					  &mtk_pwrap->wacs2_rdata,
					  &mtk_pwrap->wacs2_vldclr,
					  0);
	if (wait_result != 0) {
		pwrap_err("wait_for_fsm_idle fail,wait_result=%d\n",
			  wait_result);
		return E_PWR_WAIT_IDLE_TIMEOUT;
	}
	wacs_write = write << 31;
	wacs_addr = (addr >> 1) << 16;
	wacs_cmd = wacs_write | wacs_addr | wdata;

	write32(&mtk_pwrap->wacs2_cmd, wacs_cmd);
	if (write == 0) {
		if (!rdata) {
			pwrap_err("rdata is a NULL pointer\n");
			return E_PWR_INVALID_ARG;
		}
		wait_result = wait_for_state_ready(wait_for_fsm_vldclr,
						   TIMEOUT_READ_US,
						   &mtk_pwrap->wacs2_rdata,
						   &reg_rdata);
		if (wait_result != 0) {
			pwrap_err("wait_for_fsm_vldclr fail,wait_result=%d\n",
				  wait_result);
			return E_PWR_WAIT_IDLE_TIMEOUT_READ;
		}
		*rdata = ((reg_rdata >> RDATA_WACS_RDATA_SHIFT)
			  & RDATA_WACS_RDATA_MASK);
		write32(&mtk_pwrap->wacs2_vldclr, 1);
	}

	return 0;
}
