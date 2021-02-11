/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/dpm.h>
#include <soc/mcu_common.h>
#include <soc/symbols.h>

static void reset_dpm(struct mtk_mcu *mcu)
{
	/* write bootargs */
	write32(&mtk_dpm->twam_window_len, 0x0);
	write32(&mtk_dpm->twam_mon_type, 0x0);

	/* free RST */
	setbits32(&mtk_dpm->sw_rstn, DPM_SW_RSTN_RESET);
}

static struct mtk_mcu dpm_mcu[] = {
	{
		.firmware_name = CONFIG_DPM_DM_FIRMWARE,
		.run_address = (void *)DPM_DM_SRAM_BASE,
	},
	{
		.firmware_name = CONFIG_DPM_PM_FIRMWARE,
		.run_address = (void *)DPM_PM_SRAM_BASE,
		.reset = reset_dpm,
	},
};

int dpm_init(void)
{
	int i;
	struct mtk_mcu *dpm;

	/* config DPM SRAM layout */
	clrsetbits32(&mtk_dpm->sw_rstn, DPM_MEM_RATIO_MASK, DPM_MEM_RATIO_CFG1);

	for (i = 0; i < ARRAY_SIZE(dpm_mcu); i++) {
		dpm = &dpm_mcu[i];
		dpm->load_buffer = _dram_dma;
		dpm->buffer_size = REGION_SIZE(dram_dma);
		if (mtk_init_mcu(dpm))
			return -1;
	}

	return 0;
}
