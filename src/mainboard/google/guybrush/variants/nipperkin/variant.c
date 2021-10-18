/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>

void variant_update_dxio_descriptors(fsp_dxio_descriptor *dxio_descriptors)
{
	dxio_descriptors[WLAN].link_aspm_L1_1 = false;
	dxio_descriptors[WLAN].link_aspm_L1_2 = false;
}
