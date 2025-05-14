/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dpm_v2.h>
#include <soc/mcu_common.h>

static struct mtk_mcu dpm_mcu[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE,
		.reset = dpm_reset,
	},
	{},
};

void dpm_reset(struct mtk_mcu *mcu)
{
	/* free RST */
	setbits32p(DPM_CFG_CH0 + DPM_RST_OFFSET, DPM_SW_RSTN);
}

int dpm_init(void)
{
	u32 dramc_wbr_backup = read32p(DRAMC_WBR);

	setbits32p(DRAMC_WBR, ENABLE_DRAMC_WBR_MASK);

	if (dpm_init_mcu(dpm_mcu))
		return -1;

	write32p(DRAMC_WBR, dramc_wbr_backup);

	return 0;
}
