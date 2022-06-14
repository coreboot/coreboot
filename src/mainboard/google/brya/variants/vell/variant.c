/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <sar.h>
#include <soc/pci_devs.h>

const char *get_wifi_sar_cbfs_filename(void)
{
	return "wifi_sar_0.hex";
}

void variant_devtree_update(void)
{
	struct device *i2c0_cs35l53_0 = DEV_PTR(i2c0_cs35l53_0);
	struct device *i2c0_cs35l53_1 = DEV_PTR(i2c0_cs35l53_1);
	struct device *i2c7_cs35l53_0 = DEV_PTR(i2c7_cs35l53_0);
	struct device *i2c7_cs35l53_1 = DEV_PTR(i2c7_cs35l53_1);

	uint32_t board_ver = board_id();

	if (board_ver >= 2) {
		i2c0_cs35l53_0->enabled = 0;
		i2c0_cs35l53_1->enabled = 0;
		i2c7_cs35l53_0->enabled = 1;
		i2c7_cs35l53_1->enabled = 1;
	} else {
		i2c0_cs35l53_0->enabled = 1;
		i2c0_cs35l53_1->enabled = 1;
		i2c7_cs35l53_0->enabled = 0;
		i2c7_cs35l53_1->enabled = 0;
	}
}
