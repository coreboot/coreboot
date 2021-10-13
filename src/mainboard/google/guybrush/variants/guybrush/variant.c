/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>

bool variant_has_pcie_wwan(void)
{
	return is_dev_enabled(DEV_PTR(gpp_bridge_2));
}
