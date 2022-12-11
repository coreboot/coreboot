/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <soc/mcu_common.h>
#include <soc/mcupm.h>
#include <soc/symbols.h>

#define ABNORMALBOOT_REG	0x0C55FAA0

static void reset_mcupm(struct mtk_mcu *mcu)
{
	/* Clear abnormal boot register */
	write32p(ABNORMALBOOT_REG, 0x0);
	write32(&mcupm_reg->sw_rstn, 0x1);
}

static struct mtk_mcu mcupm = {
	.firmware_name = CONFIG_MCUPM_FIRMWARE,
	.run_address = (void *)MCUPM_SRAM_BASE,
	.reset = reset_mcupm,
};

void mcupm_init(void)
{
	mcupm.load_buffer = _dram_dma;
	mcupm.buffer_size = REGION_SIZE(dram_dma);

	write32(&mcupm_reg->sw_rstn, 0x0);

	if (mtk_init_mcu(&mcupm))
		die("%s() failed\n", __func__);
}
