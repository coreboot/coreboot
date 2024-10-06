/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <static.h>

#define MORPHIUS_SKU_1  0x5A010010
#define MORPHIUS_SKU_2  0x5A010011
#define MORPHIUS_SKU_3  0x5A010012
#define MORPHIUS_SKU_4  0x5A010014
#define MORPHIUS_SKU_5  0x5A010029
#define MORPHIUS_SKU_11 0x5A010025
#define MORPHIUS_SKU_12 0x5A010026
#define MORPHIUS_SKU_13 0x5A010013
#define MORPHIUS_SKU_14 0x5A010028
#define MORPHIUS_SKU_15 0x5A010016

void variant_devtree_update(void)
{
	/*
	 * Enable eMMC if eMMC bit is set in FW_CONFIG or device is unprovisioned.
	 */
	if (!(variant_has_emmc() || boot_is_factory_unprovisioned()))
		DEV_PTR(emmc)->enabled = 0;
}

bool variant_has_fingerprint(void)
{
	switch (sku_id()) {
	case MORPHIUS_SKU_1:
	case MORPHIUS_SKU_2:
	case MORPHIUS_SKU_3:
	case MORPHIUS_SKU_4:
	case MORPHIUS_SKU_5:
	case MORPHIUS_SKU_11:
	case MORPHIUS_SKU_12:
	case MORPHIUS_SKU_13:
	case MORPHIUS_SKU_14:
	case MORPHIUS_SKU_15:
		return true;
	default:
		return false;
	}
}
