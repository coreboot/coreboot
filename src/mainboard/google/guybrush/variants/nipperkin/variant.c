/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <soc/gpio.h>
#include <string.h>

void variant_update_dxio_descriptors(fsp_dxio_descriptor *dxio_descriptors)
{
	uint32_t board_version = board_id();

	if (board_version >= 3) {
		dxio_descriptors[WLAN].link_aspm_L1_1 = true;
		dxio_descriptors[WLAN].link_aspm_L1_2 = true;
		/* Disable PSPP to avoid S0ix hangs - b/228830362 */
		memset(dxio_descriptors[WLAN].port_params, 0,
		       sizeof(dxio_descriptors[WLAN].port_params));
	} else {
		dxio_descriptors[WLAN].link_aspm_L1_1 = false;
		dxio_descriptors[WLAN].link_aspm_L1_2 = false;
	}
}
