/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <delay.h>
#include <device/mmio.h>
#include <soc/spm.h>

void spm_reset_and_init_pcm(void)
{
	/* disable r0 and r7 to control power */
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* disable pcm timer after leaving FW */
	clrsetbits32(&mtk_spm->pcm_con1,
		     REG_PCM_TIMER_EN_LSB, SPM_REGWR_CFG_KEY);

	/* reset PCM */
	write32(&mtk_spm->pcm_con0,
		SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB | PCM_SW_RESET_LSB);
	write32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* init PCM_CON1 (disable PCM timer but keep PCM WDT setting) */
	clrsetbits32(&mtk_spm->pcm_con1, ~REG_PCM_WDT_WAKE_LSB,
		     SPM_REGWR_CFG_KEY | REG_SPM_APB_INTERNAL_EN_LSB |
		     REG_SSPM_APB_P2P_EN_LSB);
}

void spm_set_wakeup_event(const struct pwr_ctrl *pwrctrl)
{
	u32 val, mask;

	/* toggle event counter clear */
	write32(&mtk_spm->spm_event_counter_clear, REG_SPM_EVENT_COUNTER_CLR_LSB);
	/* toggle for reset SYS TIMER start point */
	setbits32(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);

	if (pwrctrl->timer_val_cust == 0)
		val = pwrctrl->timer_val ? pwrctrl->timer_val : PCM_TIMER_SUSPEND;
	else
		val = pwrctrl->timer_val_cust;

	write32(&mtk_spm->pcm_timer_val, val);
	setbits32(&mtk_spm->pcm_con1, SPM_REGWR_CFG_KEY | REG_PCM_TIMER_EN_LSB);

	/* unmask AP wakeup source */
	if (pwrctrl->wake_src_cust == 0)
		mask = pwrctrl->wake_src;
	else
		mask = pwrctrl->wake_src_cust;

	if (pwrctrl->reg_csyspwrup_ack_mask)
		mask &= ~R12_CSYSPWREQ_B;
	write32(&mtk_spm->spm_wakeup_event_mask, ~mask);

	/* unmask SPM ISR (keep TWAM setting) */
	setbits32(&mtk_spm->spm_irq_mask, ISRM_RET_IRQ_AUX);

	/* toggle event counter clear */
	write32(&mtk_spm->spm_event_counter_clear, 0);
	/* toggle for reset SYS TIMER start point */
	clrbits32(&mtk_spm->sys_timer_con, SYS_TIMER_START_EN_LSB);
}

void spm_init_pcm_register(void)
{
	write32(&mtk_spm->pcm_pwr_io_en, 0);
}

void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl)
{
	/* Waiting for loading SPMFW done*/
	while (read32(&mtk_spm->md32pcm_dma0_rlct) != 0x0)
		;

	/* In the new SOC design, this part has been simplified */
	spm_set_pcm_flags(pwrctrl);

	/* Kick PCM to run (only toggle PCM_KICK) */
	setbits32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Reset md32pcm */
	SET32_BITFIELDS(&mtk_spm->md32pcm_cfgreg_sw_rstn,
			MD32PCM_CFGREG_SW_RSTN_RESET, 1);

	/* Waiting for SPM init done */
	udelay(SPM_INIT_DONE_US);
}
