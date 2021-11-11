/* SPDX-License-Identifier: GPL-2.0-only */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>

static void devtree_update_emmc_rtd3(uint32_t board_ver)
{
	struct device *emmc_rtd3 = DEV_PTR(emmc_rtd3);
	if (board_ver > 1)
		return;

	emmc_rtd3->enabled = 0;
}

void variant_devtree_update(void)
{
	uint32_t board_ver = board_id();
	devtree_update_emmc_rtd3(board_ver);
}
