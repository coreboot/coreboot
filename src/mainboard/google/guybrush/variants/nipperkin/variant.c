/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <soc/gpio.h>

void variant_update_dxio_descriptors(fsp_dxio_descriptor *dxio_descriptors)
{
	dxio_descriptors[WLAN].link_aspm_L1_1 = false;
	dxio_descriptors[WLAN].link_aspm_L1_2 = false;
}

uint8_t variant_sd_aux_reset_gpio(void)
{
	uint32_t board_ver = board_id();

	return (board_ver < 2) ? GPIO_69 : GPIO_5;
}
