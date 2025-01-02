/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dpm_v2.h>
#include <soc/mcu_common.h>

_Static_assert(!CONFIG(DPM_FOUR_CHANNEL),
	       "DPM_FOUR_CHANNEL not implemented in dpm_v2");

static struct mtk_mcu dpm_mcu_ch0[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE,
		.priv = (void *)DPM_CFG_CH0,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE,
		.priv = (void *)DPM_CFG_CH0,
		.reset = dpm_reset,
	},
	{},
};

static struct mtk_mcu dpm_mcu_ch1[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE2,
		.priv = (void *)DPM_CFG_CH1,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE2,
		.priv = (void *)DPM_CFG_CH1,
		.reset = dpm_reset,
	},
	{},
};

void dpm_reset(struct mtk_mcu *mcu)
{
	uintptr_t dpm_cfg_reg = (uintptr_t)(mcu->priv);
	/* free RST */
	setbits32p(dpm_cfg_reg + DPM_RST_OFFSET, DPM_SW_RSTN);
}

int dpm_init(void)
{
	u32 dramc_wbr_backup;

	if (CONFIG(DPM_BROADCAST)) {
		dramc_wbr_backup = read32p(DRAMC_WBR);
		setbits32p(DRAMC_WBR, ENABLE_DRAMC_WBR_MASK);
	}

	if (dpm_init_mcu(dpm_mcu_ch0))
		return -1;

	if (!CONFIG(DPM_BROADCAST)) {
		if (dpm_init_mcu(dpm_mcu_ch1))
			return -1;
	}

	if (CONFIG(DPM_BROADCAST))
		write32p(DRAMC_WBR, dramc_wbr_backup);

	return 0;
}
