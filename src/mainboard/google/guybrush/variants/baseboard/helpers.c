/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <baseboard/variants.h>
#include <device/device.h>
#include <gpio.h>

bool __weak variant_has_pcie_wwan(void)
{
	return false;
}

uint8_t __weak variant_sd_aux_reset_gpio(void)
{
	return GPIO_69;
}
