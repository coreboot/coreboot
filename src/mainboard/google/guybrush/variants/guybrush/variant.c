/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <boardid.h>
#include <device/device.h>
#include <soc/gpio.h>

bool variant_has_pcie_wwan(void)
{
	return is_dev_enabled(DEV_PTR(gpp_bridge_2));
}

uint8_t variant_sd_aux_reset_gpio(void)
{
	return board_id() == 1 ? GPIO_70 : GPIO_69;
}
