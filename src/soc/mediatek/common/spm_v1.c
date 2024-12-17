/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/spm.h>
#include <delay.h>

void spm_init_pcm_register(void)
{
	/* Init r0 with POWER_ON_VAL0 */
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val0));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R0);
	write32(&mtk_spm->pcm_pwr_io_en, 0);

	/* Init r7 with POWER_ON_VAL1 */
	write32(&mtk_spm->pcm_reg_data_ini,
		read32(&mtk_spm->spm_power_on_val1));
	write32(&mtk_spm->pcm_pwr_io_en, PCM_RF_SYNC_R7);
	write32(&mtk_spm->pcm_pwr_io_en, 0);
}

void spm_kick_pcm_to_run(const struct pwr_ctrl *pwrctrl)
{
	/* Waiting for loading SPMFW done*/
	while (read32(&mtk_spm->md32pcm_dma0_rlct) != 0x0)
		;

	/* Init register to match PCM expectation */
	write32(&mtk_spm->spm_bus_protect_mask_b, SPM_BUS_PROTECT_MASK_B_DEF);
	write32(&mtk_spm->spm_bus_protect2_mask_b,
		SPM_BUS_PROTECT2_MASK_B_DEF);
	write32(&mtk_spm->pcm_reg_data_ini, 0);

	spm_set_pcm_flags(pwrctrl);

	/* Kick PCM to run (only toggle PCM_KICK) */
	setbits32(&mtk_spm->pcm_con0, SPM_REGWR_CFG_KEY | PCM_CK_EN_LSB);

	/* Reset md32pcm */
	SET32_BITFIELDS(&mtk_spm->md32pcm_cfgreg_sw_rstn,
			MD32PCM_CFGREG_SW_RSTN_RESET, 1);

	/* Waiting for SPM init done */
	udelay(SPM_INIT_DONE_US);
}
