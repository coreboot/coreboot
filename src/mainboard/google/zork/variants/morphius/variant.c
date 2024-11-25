/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <static.h>

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
	case 0x5A010001:
	case 0x5A010002:
	case 0x5A010003:
	case 0x5A010020:
	case 0x5A010021:
	case 0x5A010022:
	case 0x5A010023:
	case 0x5A010024:
	case 0x5A01002A:
	case 0x5A01002B:
	case 0x5A01002C:
	case 0x5A01002D:
	case 0x5A01002E:
		return false;
	default:
		return true;
	}
}
