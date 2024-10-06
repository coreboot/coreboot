/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <soc/pci_devs.h>
#include <ec/google/chromeec/ec.h>
#include <static.h>

void variant_devtree_update(void)
{
	/*
	 * Enable eMMC if eMMC bit is set in FW_CONFIG or device is unprovisioned.
	 */
	if (!(variant_has_emmc() || boot_is_factory_unprovisioned()))
		DEV_PTR(emmc)->enabled = 0;
}
