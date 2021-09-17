/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <fw_config.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>

WEAK_DEV_PTR(fpmcu);

bool variant_has_fpmcu(void)
{
	return is_dev_enabled(DEV_PTR(fpmcu));
}

bool variant_has_pcie_wwan(void)
{
	return is_dev_enabled(DEV_PTR(gpp_bridge_2));
}
