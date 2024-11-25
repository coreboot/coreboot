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
	case 0x5A030010:
	case 0x5A030011:
	case 0x5A030012:
	case 0x5A030013:
	case 0x5A030014:
	case 0x5A030020:
	case 0x5A030040:
	case 0x5A030043:
	case 0x5A030080:
	case 0x5A030081:
	case 0x5A030082:
	case 0x5A030083:
	case 0x5A030084:
	case 0x5A030085:
	case 0x5A030086:
	case 0x5A030087:
	case 0x5A030090:
	case 0x5A030091:
	case 0x5A030092:
	case 0x5A030093:
	case 0x5A03009C:
	case 0x5A03009D:
	case 0x5A03009E:
	case 0x5A03009F:
	case 0x5A0300A0:
	case 0x5A0300A1:
	case 0x5A0300A2:
	case 0x5A0300A3:
	case 0x5A0300B8:
	case 0x5A0300BD:
	case 0x5A0300BF:
	case 0x5A0300C1:
	case 0x5A0300C3:
	case 0x5A0300C5:
	case 0x5A0300C7:
	case 0x5A0300C9:
	case 0x5A0300CB:
	case 0x5A0300CD:
	case 0x5A0300CF:
	case 0x5A0300D1:
	case 0x5A0300D3:
	case 0x5A0300D5:
	case 0x5A0300D7:
	case 0x5A0300D9:
	case 0x5A0300DB:
	case 0x5A0300DD:
	case 0x5A0300DF:
	case 0x5A0300E1:
	case 0x5A0300E3:
	case 0x5A0300E5:
	case 0x5A0300E7:
	case 0x5A0300E9:
	case 0x5A0300EB:
	case 0x5A0300ED:
	case 0x5A0300EF:
	case 0x5A0300F1:
	case 0x5A0300F3:
	case 0x5A0300F5:
	case 0x5A0300F7:
	case 0x5A0300F8:
	case 0x5A0300FA:
	case 0x5A0300FB:
	case 0x5A0300FD:
	case 0x5A0300FE:
	case 0x5A030100:
	case 0x5A030101:
	case 0x5A030103:
	case 0x5A030104:
	case 0x5A030106:
	case 0x5A030107:
	case 0x5A030109:
	case 0x5A03010B:
	case 0x5A03010C:
	case 0x5A03010E:
	case 0x5A03010F:
	case 0x5A030111:
	case 0x5A030112:
	case 0x5A030114:
	case 0x5A030115:
	case 0x5A030117:
	case 0x5A030118:
	case 0x5A03011A:
	case 0x5A03011B:
	case 0x5A03011D:
	case 0x5A03011E:
	case 0x5A030120:
	case 0x5A030121:
	case 0x5A030123:
		return false;
	default:
		return true;
	}
}
