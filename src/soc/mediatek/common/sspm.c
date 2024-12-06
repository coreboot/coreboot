/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <soc/mcu_common.h>
#include <soc/sspm.h>
#include <soc/symbols.h>

static void reset_sspm(struct mtk_mcu *mcu)
{
	write32(&sspm_reg->sw_rstn, 0x1);
}

static struct mtk_mcu sspm = {
	.firmware_name = CONFIG_SSPM_FIRMWARE,
	.run_address = (void *)SSPM_SRAM_BASE,
	.reset = reset_sspm,
};

__weak void sspm_enable_sram(void)
{
	/* do nothing. */
}

void sspm_init(void)
{
	sspm_enable_sram();

	sspm.load_buffer = _dram_dma;
	sspm.buffer_size = REGION_SIZE(dram_dma);

	mtk_init_mcu(&sspm);
}
