/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dpm_v1.h>
#include <soc/mcu_common.h>

_Static_assert(!CONFIG(DPM_BROADCAST),
	       "DPM_BROADCAST not implemented in dpm_v1");

static struct mtk_mcu dpm_mcu[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE,
		.priv = mtk_dpm,
		.reset = dpm_reset,
	},
	{},
};

void dpm_reset(struct mtk_mcu *mcu)
{
	struct dpm_regs *dpm = mcu->priv;

	/* write bootargs */
	write32(&dpm->twam_window_len, 0x0);
	write32(&dpm->twam_mon_type, 0x0);

	/* free RST */
	setbits32(&dpm->sw_rstn, DPM_SW_RSTN_RESET);
}

int dpm_init(void)
{
	if (CONFIG(DPM_FOUR_CHANNEL))
		if (dpm_4ch_init())
			return -1;

	/* config DPM SRAM layout */
	clrsetbits32(&mtk_dpm->sw_rstn, DPM_MEM_RATIO_MASK, DPM_MEM_RATIO_CFG1);

	if (dpm_init_mcu(dpm_mcu))
		return -1;

	if (CONFIG(DPM_FOUR_CHANNEL))
		if (dpm_4ch_para_setting())
			return -1;

	return 0;
}
