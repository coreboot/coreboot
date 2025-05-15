/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/dpm_common.h>
#include <soc/mcu_common.h>
#include <soc/symbols.h>

int dpm_init_mcu(struct mtk_mcu *mcu_list)
{
	struct mtk_mcu *mcu = mcu_list;

	while (mcu->firmware_name) {
		mcu->load_buffer = _dram_dma;
		mcu->buffer_size = REGION_SIZE(dram_dma);
		if (mtk_init_mcu(mcu))
			return -1;

		mcu++;
	}

	return 0;
}
